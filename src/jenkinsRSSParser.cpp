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
** jenkinsParser.cpp
**
**        Created on: Nov 18, 2011
**   Original Author: fargie_s
**
*/

#include <QDebug>
#include <QRegExp>
#include "jenkinsRSSParser.hh"

JenkinsRSSParser::JenkinsRSSParser() :
    m_accu(false),
    m_start(&JenkinsRSSParser::parserStart),
    m_end(NULL),
    m_titleEx("^(.+\\S)\\s*#(\\d+)\\s*\\((.+)\\)$"),
    m_uriEx("^(.*/)\\d+/?$")
{
}

JenkinsRSSParser::~JenkinsRSSParser()
{
}

bool JenkinsRSSParser::startElement(
        const QString &namespaceURI,
        const QString &localName,
        const QString &qName,
        const QXmlAttributes &attrs)
{
    if (m_start)
        return (this->*m_start)(localName, attrs);
    return true;
}

bool JenkinsRSSParser::characters(const QString &st)
{
    if (m_accu)
        m_data += st;
    return true;
}

bool JenkinsRSSParser::endElement(
        const QString &namespaceURI,
        const QString &localName,
        const QString &qName)
{
    if (m_end)
        return (this->*m_end)(localName);
    return true;
}

bool JenkinsRSSParser::parserStart(
        const QString &name,
        const QXmlAttributes &attrs)
{
    if (name == "entry")
    {
        m_start = &JenkinsRSSParser::entryStart;
        m_end = &JenkinsRSSParser::entryEnd;

        m_name.clear();
        m_uri.clear();
        m_num = -1;
    }
    return true;
}

bool JenkinsRSSParser::entryStart(
        const QString &name,
        const QXmlAttributes &attrs)
{
    if (name == "title")
    {
        m_data.clear();
        m_accu = true;
    }
    else if (name == "link")
    {
        if (m_uriEx.indexIn(attrs.value("href")) != -1)
            m_uri = m_uriEx.cap(1);
        else
        {
            /** @todo: set error ? */
        }
    }
    return true;
}

bool JenkinsRSSParser::entryEnd(const QString &name)
{
    if (name == "entry")
    {
        if (!m_uri.isEmpty() && !m_name.isEmpty() && m_num != -1)
            emit projectEvent(m_name, m_uri, m_num);
        /** @todo: else send error */

        m_start = &JenkinsRSSParser::parserStart;
        m_end = NULL;
    }
    else if (name == "title")
    {
        if (m_titleEx.indexIn(m_data) != -1)
        {
            m_name = m_titleEx.cap(1);
            m_num = m_titleEx.cap(2).toInt();
        }
        else
        {
            /** @todo: set error ? */
        }
    }
    return true;
}

bool JenkinsRSSParser::fatalError(const QXmlParseException &exception)
{
    return false;
}

