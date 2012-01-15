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

#include "Tray.hh"
#include "RSSParser.hh"
#include "Downloader.hh"
#include "Project.hh"
#include "Menu.hh"
#include "Settings.hh"

namespace jenkins {

Tray::Tray() :
    QSystemTrayIcon(QIcon(":/icons/gear")),
    m_globalState(Project::UNKNOWN),
    m_parser(new RSSParser()),
    m_menu(new Menu(NULL)),
    m_settings(new Settings()),
    m_timer(), m_first(true)
{
    connect(&m_timer, SIGNAL(timeout()),
            this, SLOT(timerEvent()));

    connect(m_parser, SIGNAL(projectEvent(const QString &, const QUrl &, int)),
            this, SLOT(updateEvent(const QString &, const QUrl &, int)));
    connect(m_parser, SIGNAL(finished()),
            this, SLOT(updateFinished()));

    connect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(activate(QSystemTrayIcon::ActivationReason)));

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

void Tray::activate(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
        case QSystemTrayIcon::Trigger:
            m_menu->popup(QCursor::pos());
            break;
        case QSystemTrayIcon::Context:
        case QSystemTrayIcon::DoubleClick:
            if (m_settings->configure())
            {
                if (m_timer.isActive())
                {
                    m_timer.setInterval(0);
                    m_timer.start();
                }
            }
            break;
    };
}

void Tray::timerEvent()
{
    update();
    m_first = false;
}

void Tray::update()
{
    QNetworkReply *reply =
        Downloader::instance()->get(QUrl(m_settings->getUrl() + "/rssLatest"));
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
    m_timer.setInterval(m_settings->getInterval() * 1000);
    m_timer.start();
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

