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

#include "jenkinsTray.hh"
#include "jenkinsRSSEngine.hh"
#include "jenkinsDownloader.hh"
#include "jenkinsProject.hh"
#include "jenkinsMenu.hh"

JenkinsTray::JenkinsTray() :
    QSystemTrayIcon(QIcon(":/icons/gear")),
    m_downloader(new JenkinsDownloader()),
    m_engine(new JenkinsRSSEngine()),
    m_menu(new JenkinsMenu(NULL)),
    m_timer()
{
    connect(m_downloader, SIGNAL(finished(QNetworkReply *)),
            this, SLOT(updateFinished(QNetworkReply *)));

    connect(&m_timer, SIGNAL(timeout()),
            this, SLOT(update()));

    connect(m_engine, SIGNAL(projectEvent(const QString &, const QUrl &, int)),
            this, SLOT(updateEvent(const QString &, const QUrl &, int)));

    setContextMenu(m_menu);

    m_timer.setInterval(1000);
    m_timer.setSingleShot(true);
    m_timer.start();
}

JenkinsTray::~JenkinsTray()
{
    delete m_downloader;
    delete m_engine;
    delete m_menu;
}

void JenkinsTray::update()
{
    /** @todo: parameterize the url */
    QNetworkReply *reply = m_downloader->get(QUrl("http://localhost:8080/rssLatest"));
    m_engine->parse(reply);
}

void JenkinsTray::updateFinished(QNetworkReply *reply)
{
    m_engine->parseContinue();
    reply->deleteLater();
}

void JenkinsTray::updateEvent(const QString &name, const QUrl &uri, int buildNum)
{
    QMap<QString, JenkinsProject *>::iterator it = m_projects.find(name);

    if (it == m_projects.end())
    {
        it = m_projects.insert(name, new JenkinsProject(name, uri, buildNum));
        it.value()->update();
        m_menu->addProject(*(it.value()));
    }
    else
    {
        if (it.value()->getNum() != buildNum)
            it.value()->update();
    }
}

