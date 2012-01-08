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
** stub_Settings.hh
**
**        Created on: Jan 07, 2012
**   Original Author: fargie_s
**
*/

#ifndef __STUB_SETTINGS_HH__
#define __STUB_SETTINGS_HH__

#include "Settings.hh"

class SettingsStub : public jenkins::Settings
{
public:
    SettingsStub();
    virtual ~SettingsStub();

    inline void setInterval(int interval)
    {
        m_interval = interval;
    }

    inline void setUrl(const QString &url)
    {
        m_url = url;
    }

};

#endif

