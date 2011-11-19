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
** jenkinsEngine.hh
**
**        Created on: Nov 17, 2011
**   Original Author: fargie_s
**
*/

#ifndef __JENKINS_ENGINE_HH__
#define __JENKINS_ENGINE_HH__

#include <QObject>
#include <QUrl>

class QIODevice;
class QXmlSimpleReader;
class QXmlInputSource;

class JenkinsRSSEngine : public QObject
{
    Q_OBJECT;

public:
    JenkinsRSSEngine();
    virtual ~JenkinsRSSEngine();

    bool parse(QIODevice *input);

signals:
    void projectEvent(const QString &, const QUrl &, int);

public slots:
    void parseContinue();

protected:
    QXmlSimpleReader *m_reader;
    QXmlInputSource  *m_source;
};

#endif

