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
** jenkinsProject.cpp
**
**        Created on: Nov 19, 2011
**   Original Author: fargie_s
**
*/

#include "Downloader.hh"
#include "Project.hh"
#include "BuildParser.hh"

namespace jenkins {

#define XML_API_SUFFIX "/api/xml"
#define XML_API_SUFFIX_LEN (sizeof (XML_API_SUFFIX) - 1)

Project::Project(QObject *parent, const QString &name, const QUrl &uri) :
    QObject(parent),
    m_parser(new BuildParser()),
    m_name(name), m_uri(uri), m_num(-1)
{
    connect(m_parser, SIGNAL(projectEvent(Project::State)),
                this, SLOT(stateEvent(Project::State)));
}

Project::Project(QObject *parent, const QString &name, const QUrl &uri, int num) :
    QObject(parent),
    m_parser(new BuildParser()),
    m_name(name), m_uri(uri), m_num(num)
{
    connect(m_parser, SIGNAL(projectEvent(Project::State)),
                this, SLOT(stateEvent(Project::State)));
}

Project::~Project()
{
    delete m_parser;
}

void Project::stateEvent(Project::State state)
{
    m_state = state;
    emit updated(*this);
}

void Project::buildEvent(int build)
{
    if (m_num != build)
    {
        m_state = Project::UNKNOWN;
        m_num = build;
        emit updated(*this);
        update(m_uri);
    }
}

void Project::update(const QUrl &uri)
{
    QString url(uri.toString());
    if (!url.endsWith('/'))
            url += "/";
    url += QString::number(m_num) + XML_API_SUFFIX;
    QNetworkReply *reply = Downloader::instance()->get(QUrl(url));

    m_parser->parse(reply);
    /* connect after the parser to handle redirections */
    connect(reply, SIGNAL(finished()), this,
            SLOT(networkReplyFinished()));
}

void Project::networkReplyFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());

    if (reply == NULL)
        return;

    int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    static const QRegExp build_exp("/[0-9]+" XML_API_SUFFIX "$");

    if (status == HTTP_MOVED_PERM)
    {
        QString new_uri =
            reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toString();
        if (!new_uri.isEmpty() && new_uri.contains(build_exp))
        {
            m_uri = new_uri.remove(build_exp);
            update(m_uri);
        }

    }
    else if (status == HTTP_FOUND)
    {
        QString new_uri =
            reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toString();
        if (!new_uri.isEmpty() && new_uri.contains(build_exp))
        {
            update(QUrl(new_uri.remove(build_exp)));
        }
    }
}

bool Project::isUpdating() const
{
    return m_parser->isParsing();
}

}

