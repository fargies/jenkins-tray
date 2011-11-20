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
    m_parser(new RSSParser()),
    m_menu(new Menu(NULL)),
    m_timer()
{
    connect(&m_timer, SIGNAL(timeout()),
            this, SLOT(update()));

    connect(m_parser, SIGNAL(projectEvent(const QString &, const QUrl &, int)),
            this, SLOT(updateEvent(const QString &, const QUrl &, int)));
    connect(m_parser, SIGNAL(finished()),
            this, SLOT(updateFinished()));

    setContextMenu(m_menu);

    m_timer.setInterval(1000);
    m_timer.setSingleShot(true);
    m_timer.start();
}

Tray::~Tray()
{
    delete m_parser;
    delete m_menu;
}

void Tray::update()
{
    /** @todo: parameterize the url */
    QNetworkReply *reply = Downloader::instance()->get(QUrl("http://localhost:8080/rssLatest"));
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
    m_timer.setInterval(60 * 1000);
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
}

void Tray::updateEvent(const QString &name, const QUrl &uri, int buildNum)
{
    QMap<QString, Project *>::iterator it = m_projects.find(name);

    if (it == m_projects.end())
    {
        it = m_projects.insert(name, new Project(name, uri));
        connect(it.value(), SIGNAL(updated(const Project &)),
                    this, SLOT(updateEvent(const Project &)));
        it.value()->buildEvent(buildNum);
        m_menu->addProject(*(it.value()));
    }
    else
    {
        if (it.value()->getNum() != buildNum)
            it.value()->buildEvent(buildNum);
    }
}

}

