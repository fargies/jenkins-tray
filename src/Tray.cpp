/*
** Copyright (C) 2011 Fargier Sylvain <fargier.sylvain@free.fr>
**
** This software is provided 'as-is', without any express or implied
** warranty.  In no event will the authors be held liable for any damages
** arising from the use of this software.
**
** Permission is granted to anyone to use this software for any purpose,
** including commercial applications, and to alter it and redistribute it
** freely, subject to the following restrictions:
**
** 1. The origin of this software must not be misrepresented; you must not
**    claim that you wrote the original software. If you use this software
**    in a product, an acknowledgment in the product documentation would be
**    appreciated but is not required.
** 2. Altered source versions must be plainly marked as such, and must not be
**    misrepresented as being the original software.
** 3. This notice may not be removed or altered from any source distribution.
**
** jenkins_tray.cpp
**
**        Created on: Nov 17, 2011
**   Original Author: fargie_s
**
*/

#include <QAuthenticator>

#include "Tray.hh"
#include "RSSParser.hh"
#include "Downloader.hh"
#include "Project.hh"
#include "Menu.hh"
#include "Settings.hh"

#define NETWORK_RETRY_DELAY (30 * 1000)

namespace jenkins {

#define RSS_LATEST_SUFFIX "/rssLatest"
#define RSS_LATEST_SUFFIX_LEN (sizeof (RSS_LATEST_SUFFIX) - 1)

Tray::Tray(QString &instance) :
    QSystemTrayIcon(QIcon(":/icons/gear")),
    m_globalState(Project::UNKNOWN),
    m_parser(new RSSParser()),
    m_menu(new Menu(NULL)),
    m_settings(new Settings(instance)),
    m_timer(), m_first(true)
{
    connect(&m_timer, SIGNAL(timeout()),
            this, SLOT(timerEvent()));

    connect(m_parser, SIGNAL(projectEvent(const QString &, const QUrl &, int)),
            this, SLOT(updateEvent(const QString &, const QUrl &, int)));

    connect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(activate(QSystemTrayIcon::ActivationReason)));

    connect(Downloader::instance(),
            SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)),
            this, SLOT(authenticationRequest(QNetworkReply*,QAuthenticator*)));

    m_timer.setSingleShot(true);
}

Tray::~Tray()
{
    delete m_parser;
    delete m_menu;
    delete m_settings;
}

void Tray::start()
{
    show();
    m_timer.setInterval(0);
    m_timer.start();
}

void Tray::stop()
{
    m_timer.stop();
    QIODevice *dev = m_parser->device();
    if (dev)
    {
        dev->disconnect(this);
        dev->close();
        m_parser->clear();
        emit finished();
    }
}

void Tray::authenticationRequest(QNetworkReply *reply, QAuthenticator *ator)
{
    if ((ator->user() != m_settings->getUser())
            || (ator->password() != m_settings->getAuthToken()))
    {
        ator->setUser(m_settings->getUser());
        ator->setPassword(m_settings->getAuthToken());
    }
    else
    {
        showMessage("Authentication failed",
                "Wrong user or token",
                QSystemTrayIcon::Critical, NETWORK_RETRY_DELAY);
        stop();
        m_timer.setInterval(NETWORK_RETRY_DELAY);
        m_timer.start();
    }
}

void Tray::networkError(QNetworkReply::NetworkError err)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    switch (err)
    {
        case QNetworkReply::ContentAccessDenied:
        case QNetworkReply::ContentOperationNotPermittedError:
            showMessage("Authentication required",
                    "User and token required",
                    QSystemTrayIcon::Critical, NETWORK_RETRY_DELAY);
            break;
        default:
            if (reply)
            {
                showMessage("Network error",
                        reply->errorString(),
                        QSystemTrayIcon::Critical, NETWORK_RETRY_DELAY);
            }
            else
                showMessage("Network error",
                        "Unkown critical error",
                        QSystemTrayIcon::Critical, NETWORK_RETRY_DELAY);
            break;
    }
    stop();
    m_timer.setInterval(NETWORK_RETRY_DELAY);
    m_timer.start();
}

void Tray::activate(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
        case QSystemTrayIcon::Trigger:
            m_menu->popup(QCursor::pos());
            break;
        case QSystemTrayIcon::Context:
        case QSystemTrayIcon::DoubleClick:

            stop();

            if (m_settings->configure())
            {
                m_menu->clear();
                foreach(Project *p, m_projects)
                {
                    p->deleteLater();
                }
                m_projects.clear();
            }
            m_timer.setInterval(0);
            m_timer.start();
            break;
    };
}

void Tray::timerEvent()
{
    update(m_settings->getUrl());
    m_first = false;
}

void Tray::update(const QString &uri)
{
    Downloader *down = Downloader::instance();

    down->enableAuth(!m_settings->getUser().isEmpty());
    QNetworkReply *reply = down->get(QUrl(uri + RSS_LATEST_SUFFIX));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(networkError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()),
            this, SLOT(updateFinished()));
    m_parser->parse(reply);
}

void Tray::setState(Project::State state)
{
    m_globalState = state;
    switch (state)
    {
        case Project::UNKNOWN:
            setIcon(QIcon(":/icons/gear"));
            break;
        case Project::SUCCESS:
            setIcon(QIcon(":/icons/blue"));
            break;
        case Project::FAILURE:
            setIcon(QIcon(":/icons/red"));
            break;
        case Project::UNSTABLE:
            setIcon(QIcon(":/icons/yellow"));
            break;
    };
}

void Tray::updateFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    bool is_finished = true;

    if (reply != NULL)
    {
        int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

        if (status == HTTP_MOVED_PERM)
        {
            QString new_uri =
                reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toString();
            if (!new_uri.isEmpty() && new_uri.endsWith(RSS_LATEST_SUFFIX))
            {
                new_uri.truncate(new_uri.size() - RSS_LATEST_SUFFIX_LEN);
                m_settings->setUrl(new_uri);
                m_settings->save();
                update(m_settings->getUrl());
                is_finished = false;
            }
        }
        else if (status == HTTP_FOUND)
        {
            QString new_uri =
                reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toString();
            if (!new_uri.isEmpty() && new_uri.endsWith(RSS_LATEST_SUFFIX))
            {
                new_uri.truncate(new_uri.size() - RSS_LATEST_SUFFIX_LEN);
                update(new_uri);
                is_finished = false;
            }
        }
        reply->deleteLater();
    }

    if (is_finished)
    {
        emit finished();
        if (!m_timer.isActive())
        {
            m_timer.setInterval(m_settings->getInterval() * 1000);
            m_timer.start();
        }
    }
}

void Tray::updateEvent(const Project &proj)
{
    Project::State state = proj.getState();

    if (m_globalState == Project::UNKNOWN)
        setState(state);
    else if (state != Project::UNKNOWN)
    {
        if (state < m_globalState)
            setState(state);
        else if (state > m_globalState)
        {
            state = Project::SUCCESS;
            foreach(Project *p, m_projects)
            {
                if (p->getState() < state &&
                        p->getState() != Project::UNKNOWN)
                    state = p->getState();
            }
            if (state != m_globalState)
                setState(state);
        }
    }
    if (m_settings->isTrayNotifications() && !m_first)
    {
        switch (proj.getState())
        {
            case Project::SUCCESS:
                showMessage("Build Successful",
                        proj.getName() + " #" + QString::number(proj.getNum()),
                        QSystemTrayIcon::Information, 30 * 1000);
                break;
            case Project::UNSTABLE:
                showMessage("Build Successful (unstable)",
                        proj.getName() + " #" + QString::number(proj.getNum()),
                        QSystemTrayIcon::Warning, 30 * 1000);
                break;
            case Project::FAILURE:
                showMessage("Build Failed",
                        proj.getName() + " #" + QString::number(proj.getNum()),
                        QSystemTrayIcon::Critical, -1);
                break;
            case Project::UNKNOWN:
                showMessage("Build in Progress",
                        proj.getName() + " #" + QString::number(proj.getNum()),
                        QSystemTrayIcon::Information, 10 * 1000);
                break;
        }

    }
}

void Tray::updateEvent(const QString &name, const QUrl &uri, int buildNum)
{
    QMap<QString, Project *>::iterator it = m_projects.find(name);

    if (it == m_projects.end())
    {
        it = m_projects.insert(name, new Project(this, name, uri));
        connect(it.value(), SIGNAL(updated(const Project &)),
                    this, SLOT(updateEvent(const Project &)));
        it.value()->buildEvent(buildNum);
        m_menu->addProject(*(it.value()));
    }
    else
    {
        if (it.value()->getNum() != buildNum)
            it.value()->buildEvent(buildNum);
        else if (it.value()->getState() == Project::UNKNOWN &&
                !it.value()->isUpdating())
            it.value()->update();
    }
}

}

