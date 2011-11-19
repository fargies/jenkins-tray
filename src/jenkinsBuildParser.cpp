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
#include "jenkinsBuildParser.hh"

JenkinsBuildParser::JenkinsBuildParser() :
    m_accu(false),
    m_start(&JenkinsBuildParser::parserStart),
    m_end(NULL)
{
}

JenkinsBuildParser::~JenkinsBuildParser()
{
}

bool JenkinsBuildParser::startElement(
        const QString &namespaceURI,
        const QString &localName,
        const QString &qName,
        const QXmlAttributes &attrs)
{
    if (m_start)
        return (this->*m_start)(localName, attrs);
    return true;
}

bool JenkinsBuildParser::characters(const QString &st)
{
    if (m_accu)
        m_data += st;
    return true;
}

bool JenkinsBuildParser::endElement(
        const QString &namespaceURI,
        const QString &localName,
        const QString &qName)
{
    if (m_end)
        return (this->*m_end)(localName);
    return true;
}

bool JenkinsBuildParser::parserStart(
        const QString &name,
        const QXmlAttributes &attrs)
{
    if (name == "result")
    {
        m_start = NULL;
        m_end = &JenkinsBuildParser::resultEnd;
        m_accu = true;
    }
    else if (name != "freeStyleBuild" &&
            name != "matrixBuild")
    {
        m_start = NULL;
        m_end = &JenkinsBuildParser::waitEnd;
        m_opened = name;
    }
    return true;
}

bool JenkinsBuildParser::resultEnd(const QString &name)
{
    if (name == "result")
    {
        JenkinsProject::State state;
        if (m_data == "SUCCESS")
            state = JenkinsProject::SUCCESS;
        else if (m_data == "UNSTABLE")
            state = JenkinsProject::UNSTABLE;
        else if (m_data == "FAILURE")
            state = JenkinsProject::FAILURE;
        else
            state = JenkinsProject::UNKNOWN;

        emit projectEvent(state);

        m_start = &JenkinsBuildParser::parserStart;
        m_end = NULL;
        m_accu = false;
    }
    return true;
}

bool JenkinsBuildParser::waitEnd(const QString &name)
{
    if (name == m_opened)
    {
        m_start = &JenkinsBuildParser::parserStart;
        m_end = NULL;
    }
}

bool JenkinsBuildParser::fatalError(const QXmlParseException &exception)
{
    return false;
}

