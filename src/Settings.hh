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
** Settings.hh
**
**        Created on: Nov 21, 2011
**   Original Author: fargie_s
**
*/

#ifndef __JENKINS_SETTINGS_HH__
#define __JENKINS_SETTINGS_HH__

#include <QString>

namespace jenkins {

#define DEFAULT_INTERVAL (60)
#define DEFAULT_URI "http://localhost:8080"

class Settings
{
public:
    Settings(QString &instance);
    virtual ~Settings();

    inline int getInterval() const
    {
        return m_interval;
    }

    inline const QString &getUrl() const
    {
        return m_url;
    }

    inline const QString &getAuthToken() const
    {
        return m_token;
    }

    inline const QString &getUser() const
    {
        return m_user;
    }

    inline bool isTrayNotifications() const
    {
        return m_trayNotif;
    }

    inline void setUrl(const QString &uri)
    {
        m_url = uri;
    }

    bool configure();
    void save();

protected:
    QString m_instance;
    int m_interval;
    QString m_url;
    QString m_user;
    QString m_token;
    bool m_trayNotif;
};

}

#endif

