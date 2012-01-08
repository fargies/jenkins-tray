/*
** Copyright (C) 2012 Fargier Sylvain <fargier.sylvain@free.fr>
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
** stub_tray.hh
**
**        Created on: Jan 07, 2012
**   Original Author: fargie_s
**
*/

#ifndef __STUB_TRAY_HH__
#define __STUB_TRAY_HH__

#include "Tray.hh"

class SettingsStub;

namespace jenkins
{
class Project;
class RSSParser;
}

class TrayStub : public jenkins::Tray
{
    Q_OBJECT;

public:
    TrayStub();
    ~TrayStub();

    SettingsStub &getSettingsStub();

    inline QMap<QString, jenkins::Project *> &getProjects()
    {
        return m_projects;
    }

    inline jenkins::RSSParser *getParser()
    {
        return m_parser;
    }
};

#endif

