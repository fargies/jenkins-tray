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
#include "jenkinsParser.hh"

JenkinsParser::JenkinsParser() :
    m_accu(false),
    m_start(&JenkinsParser::parserStart),
    m_end(NULL),
    m_titleEx("^(.+\\S)\\s*#(\\d+)\\s*\\((.+)\\)$")
{
}

JenkinsParser::~JenkinsParser()
{
}

bool JenkinsParser::startElement(
        const QString &namespaceURI,
        const QString &localName,
        const QString &qName,
        const QXmlAttributes &attrs)
{
    if (m_start)
        return (this->*m_start)(localName, attrs);
    return true;
}

bool JenkinsParser::characters(const QString &st)
{
    if (m_accu)
        m_data += st;
    return true;
}

bool JenkinsParser::endElement(
        const QString &namespaceURI,
        const QString &localName,
        const QString &qName)
{
    if (m_end)
        return (this->*m_end)(localName);
    return true;
}

bool JenkinsParser::parserStart(
        const QString &name,
        const QXmlAttributes &attrs)
{
    if (name == "entry")
    {
        m_start = &JenkinsParser::entryStart;
        m_end = &JenkinsParser::entryEnd;
        m_event.clear();
    }
    return true;
}

bool JenkinsParser::entryStart(
        const QString &name,
        const QXmlAttributes &attrs)
{
    if (name == "title" ||
            name == "id")
    {
        m_data.clear();
        m_accu = true;
    }
    else if (name == "link")
    {
        /** @todo: get the link */
    }
    return true;
}

bool JenkinsParser::entryEnd(const QString &name)
{
    if (name == "entry")
    {
        emit buildEvent(m_event);
        m_start = &JenkinsParser::parserStart;
        m_end = NULL;
    }
    else if (name == "title")
    {
        if (m_titleEx.indexIn(m_data) != -1)
        {
            m_event.setName(m_titleEx.cap(1));
            m_event.setNum(m_titleEx.cap(2).toInt());
        }
        else
        {
            /** @todo: set error ? */
        }
    }
    else if (name == "id")
        m_event.setID(m_data);
    return true;
}

bool JenkinsParser::fatalError(const QXmlParseException &exception)
{
    return false;
}

