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

#include <QDebug>
#include "jenkinsTray.hh"
#include "jenkinsEngine.hh"
#include "jenkinsDownloader.hh"

JenkinsTray::JenkinsTray() :
    QSystemTrayIcon(QIcon(":/icons/gear")),
    m_downloader(new JenkinsDownloader()),
    m_engine(new JenkinsEngine()),
    m_timer()
{
    connect(m_downloader, SIGNAL(finished(QNetworkReply *)),
            this, SLOT(requestFinished(QNetworkReply *)));

    connect(&m_timer, SIGNAL(timeout()),
            this, SLOT(update()));

    connect(m_engine, SIGNAL(buildEvent(const JenkinsStatus &)),
            this, SLOT(buildEvent(const JenkinsStatus &)));

            m_timer.setInterval(1000);
    m_timer.setSingleShot(true);
    m_timer.start();
}

JenkinsTray::~JenkinsTray()
{
    delete m_downloader;
    delete m_engine;
}

void JenkinsTray::update()
{
    /** @todo: parameterize the url */
    QNetworkReply *reply = m_downloader->get(QUrl("http://localhost:8080/rssLatest"));
    m_engine->parse(reply);
}

void JenkinsTray::requestFinished(QNetworkReply *reply)
{
    m_engine->parseContinue();
    reply->deleteLater();
}

void JenkinsTray::buildEvent(const JenkinsStatus &status)
{
    qDebug() << "status updated " << status.getName();
    showMessage("", status.getID());
}

