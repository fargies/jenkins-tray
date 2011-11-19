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
** jenkinsParser.hh
**
**        Created on: Nov 18, 2011
**   Original Author: fargie_s
**
*/

#ifndef __JENKINS_PARSER_HH__
#define __JENKINS_PARSER_HH__

#include <QObject>
#include <QString>
#include <QRegExp>
#include <QUrl>
#include <QXmlDefaultHandler>
#include "jenkinsProject.hh"

class JenkinsBuildParser :
    public QObject,
    public QXmlDefaultHandler
{
    Q_OBJECT;

public:
    typedef bool (JenkinsBuildParser::*startHandler)(
            const QString &name,
            const QXmlAttributes &attrs);
    typedef bool (JenkinsBuildParser::*endHandler)(
            const QString &name);

    JenkinsBuildParser();
    virtual ~JenkinsBuildParser();

    virtual bool startElement(
            const QString &namespaceURI,
            const QString &localName,
            const QString &qName,
            const QXmlAttributes &attrs);
    virtual bool endElement(
            const QString &namespaceURI,
            const QString &localName,
            const QString &qName);
    virtual bool characters(
            const QString &st);

    virtual bool fatalError(
            const QXmlParseException &exception);

    bool parserStart(
            const QString &name,
            const QXmlAttributes &attrs);
    bool waitEnd(
            const QString &name);
    bool resultEnd(
            const QString &name);

signals:
    void projectEvent(JenkinsProject::State);

protected:
    bool m_accu;
    QString m_data;
    startHandler m_start;
    endHandler m_end;
    QString m_opened;
};

#endif

