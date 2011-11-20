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
** jenkinsMenu.hh
**
**        Created on: Nov 19, 2011
**   Original Author: fargie_s
**
*/

#ifndef __JENKINS_MENU_HH__
#define __JENKINS_MENU_HH__

#include <QMenu>
#include <QAction>

namespace Jenkins {

class Project;

class ProjectAction : public QAction
{
    Q_OBJECT;
public:
    ProjectAction(
            QObject *parent,
            const Project &);
    ~ProjectAction();

protected slots:
    void updateEvent(const Project &);

    void open();

protected:
    const Project &m_proj;
};

class Menu : public QMenu
{
    Q_OBJECT;
public:
    Menu(QWidget *parent);
    virtual ~Menu();

    void addProject(const Project &);
};

}

#endif

