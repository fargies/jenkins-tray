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
** jenkinsMenu.cpp
**
**        Created on: Nov 19, 2011
**   Original Author: fargie_s
**
*/

#include <QDesktopServices>
#include "Menu.hh"
#include "Project.hh"

namespace Jenkins {

ProjectAction::ProjectAction(
        QObject *parent,
        const Project &project) :
    QAction(parent),
    m_proj(project)
{
    connect(&project, SIGNAL(updated(const Project &)),
            this, SLOT(updateEvent(const Project &)));

    connect(this, SIGNAL(triggered()), this, SLOT(open()));
    updateEvent(project);
}

ProjectAction::~ProjectAction()
{
}

void ProjectAction::open()
{
    QDesktopServices::openUrl(m_proj.getUrl());
}

void ProjectAction::updateEvent(const Project &proj)
{
    setText(proj.getName());
    switch (proj.getState())
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

Menu::Menu(QWidget *parent) :
    QMenu(parent)
{
}

Menu::~Menu()
{
}

void Menu::addProject(const Project &proj)
{
    addAction(new ProjectAction(this, proj));
}

}

