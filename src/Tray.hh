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

class QNetworkReply;

namespace Jenkins {

class Downloader;
class RSSParser;
class Menu;
class Project;


class Tray : public QSystemTrayIcon
{
    Q_OBJECT

public:
    Tray();
    virtual ~Tray();

protected slots:
    void update();

    void updateEvent(const QString &name, const QUrl &uri, int buildNum);

protected:
    Downloader *m_downloader;
    RSSParser *m_parser;
    Menu *m_menu;
    QTimer m_timer;
    QMap<QString, Project *> m_projects;
};

}

#endif

