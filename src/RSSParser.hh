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
** RSSParser.hh
**
**        Created on: Nov 18, 2011
**   Original Author: fargie_s
**
*/

#ifndef __JENKINS_RSSPARSER_HH__
#define __JENKINS_RSSPARSER_HH__

#include <QObject>
#include <QString>
#include <QRegExp>
#include <QUrl>

class QXmlStreamReader;

namespace Jenkins {

class RSSParser :
    public QObject
{
    Q_OBJECT;

public:
    typedef bool (RSSParser::*startHandler)(
            const QStringRef &name);
    typedef bool (RSSParser::*endHandler)(
            const QStringRef &name);

    RSSParser();
    virtual ~RSSParser();

    /**
     * @brief Start to parse a new xml stream.
     *
     * @details Stops to parse current document and start a new parsing
     * context.
     *
     * @param[in] device the input device.
     */
    void parse(QIODevice *device);

    void clear();

protected:
    bool parserStart(const QStringRef &name);
    bool entryStart(const QStringRef &name);
    bool entryEnd(const QStringRef &name);

signals:
    void projectEvent(const QString &, const QUrl &, int);
    void finished();

protected slots:
    void parse();

protected:
    QXmlStreamReader *m_xml;
    bool m_accu;
    QString m_data;
    startHandler m_start;
    endHandler m_end;
    QRegExp m_titleEx;
    QRegExp m_uriEx;

    QUrl m_uri;
    QString m_name;
    int m_num;
};

}

#endif

