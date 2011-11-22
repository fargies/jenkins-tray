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
#include "Settings.hh"
#include "ui_Config.h"

namespace Jenkins {

Settings::Settings()
{
    QSettings settings("Jenkins", "JenkinsTray");

    m_interval = settings.value("interval", DEFAULT_INTERVAL).toInt();
    m_url = settings.value("url", DEFAULT_URI).toString();
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

    if (widget.exec() == QDialog::Accepted)
    {
        m_interval = m_conf.interval->value();
        m_url = m_conf.serverUri->text();

        QSettings settings("Jenkins", "JenkinsTray");
        settings.setValue("interval", m_interval);
        settings.setValue("url", m_url);
        settings.sync();
        return true;
    }
    return false;
}

}

