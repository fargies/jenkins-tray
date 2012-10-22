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
** Settings.cpp
**
**        Created on: Nov 21, 2011
**   Original Author: fargie_s
**
*/

#include <QSettings>
#include <QSystemTrayIcon>
#include "Settings.hh"
#include "ui_Config.h"

namespace jenkins {

Settings::Settings()
{
    QSettings settings("Jenkins", "JenkinsTray");

    m_interval = settings.value("interval", DEFAULT_INTERVAL).toInt();
    m_url = settings.value("url", DEFAULT_URI).toString();
    m_trayNotif = settings.value("tray_notifications", false).toBool();
    m_user = settings.value("user").toString();
    m_token = settings.value("token").toString();
}

Settings::~Settings()
{
}

bool Settings::configure()
{
    QDialog widget;
    Ui::Config m_conf;

    m_conf.setupUi(&widget);
    m_conf.interval->setValue(m_interval);
    m_conf.serverUri->setText(m_url);
    m_conf.user->setText(m_user);
    m_conf.auth_token->setText(m_token);

    if (!QSystemTrayIcon::supportsMessages())
        m_conf.trayNotifs->setDisabled(true);
    m_conf.trayNotifs->setCheckState((m_trayNotif) ? Qt::Checked :
            Qt::Unchecked);

    if (widget.exec() == QDialog::Accepted)
    {
        m_interval = m_conf.interval->value();
        m_url = m_conf.serverUri->text();
        m_trayNotif = QSystemTrayIcon::supportsMessages() &&
            (m_conf.trayNotifs->checkState() == Qt::Checked);
        m_user = m_conf.user->text();
        m_token = m_conf.auth_token->text();

        save();
        return true;
    }
    return false;
}

void Settings::save()
{
    QSettings settings("Jenkins", "JenkinsTray");
    settings.setValue("interval", m_interval);
    settings.setValue("url", m_url);
    settings.setValue("tray_notifications", m_trayNotif);
    settings.setValue("user", m_user);
    settings.setValue("token", m_token);
    settings.sync();
}

}

