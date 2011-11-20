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

namespace Jenkins {

Tray::Tray() :
    QSystemTrayIcon(QIcon(":/icons/gear")),
    m_downloader(new Downloader()),
    m_parser(new RSSParser()),
    m_menu(new Menu(NULL)),
    m_timer()
{
    connect(&m_timer, SIGNAL(timeout()),
            this, SLOT(update()));

    connect(m_parser, SIGNAL(projectEvent(const QString &, const QUrl &, int)),
            this, SLOT(updateEvent(const QString &, const QUrl &, int)));

    setContextMenu(m_menu);

    m_timer.setInterval(1000);
    m_timer.setSingleShot(true);
    m_timer.start();
}

Tray::~Tray()
{
    delete m_downloader;
    delete m_parser;
    delete m_menu;
}

void Tray::update()
{
    /** @todo: parameterize the url */
    QNetworkReply *reply = m_downloader->get(QUrl("http://localhost:8080/rssLatest"));
    m_parser->parse(reply);
}

void Tray::updateEvent(const QString &name, const QUrl &uri, int buildNum)
{
    QMap<QString, Project *>::iterator it = m_projects.find(name);

    if (it == m_projects.end())
    {
        it = m_projects.insert(name, new Project(name, uri, buildNum));
        it.value()->update();
        m_menu->addProject(*(it.value()));
    }
    else
    {
        if (it.value()->getNum() != buildNum)
            it.value()->update();
    }
}

}

