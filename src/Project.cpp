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
** jenkinsProject.cpp
**
**        Created on: Nov 19, 2011
**   Original Author: fargie_s
**
*/

#include "Project.hh"

namespace Jenkins {

Project::Project(const QString &name, const QUrl &uri) :
    m_name(name), m_uri(uri), m_num(-1)
{
}

Project::Project(const QString &name, const QUrl &uri, int num) :
    m_name(name), m_uri(uri), m_num(num)
{
}

Project::~Project()
{
}

void Project::update()
{
}

}

