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
** jenkins_tray.hh
**
**        Created on: Nov 17, 2011
**   Original Author: fargie_s
**
*/

#ifndef __JENKINS_TRAY_HH__
#define __JENKINS_TRAY_HH__

#include <QSystemTrayIcon>
#include <QTimer>
#include <QUrl>
#include <QMap>
#include <QNetworkReply>
#include "Project.hh"

class QAuthenticator;

namespace jenkins {

class RSSParser;
class Menu;
class Settings;

class Tray : public QSystemTrayIcon
{
    Q_OBJECT

public:
    Tray();
    virtual ~Tray();

    void start();

    void stop();

protected:
    void setState(Project::State);

    void update(const QString &uri);

protected slots:
    void updateEvent(const QString &name, const QUrl &uri, int buildNum);
    void updateEvent(const Project &proj);

    void updateFinished();

    void timerEvent();

    void activate(QSystemTrayIcon::ActivationReason);

    void authenticationRequest(QNetworkReply *, QAuthenticator *);

    void networkError(QNetworkReply::NetworkError);

signals:
    void finished();

protected:
    Project::State m_globalState;
    RSSParser *m_parser;
    Menu *m_menu;
    Settings *m_settings;
    QTimer m_timer;
    QMap<QString, Project *> m_projects;
    bool m_first;
};

}

#endif

