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

#include <QXmlStreamReader>
#include "BuildParser.hh"

namespace Jenkins {

BuildParser::BuildParser() :
    m_xml(new QXmlStreamReader()),
    m_accu(false),
    m_start(&BuildParser::parserStart),
    m_end(NULL)
{
}

BuildParser::~BuildParser()
{
    delete m_xml;
}

void BuildParser::parse(QIODevice *device)
{
    clear();
    m_xml->setDevice(device);
    if (device)
    {
        connect(device, SIGNAL(readyRead()),
                this, SLOT(parse()));
        if (!device->atEnd())
            parse();
    }
}

void BuildParser::clear()
{
    if (m_xml->device() != NULL)
    {
        m_xml->device()->disconnect(this);
        m_xml->device()->deleteLater();
    }
    m_xml->clear();
    m_start = &BuildParser::parserStart;
    m_end = NULL;
}

void BuildParser::parse()
{
    while (!m_xml->atEnd() && !m_xml->error())
    {
        m_xml->readNext();
        if (m_xml->isStartElement())
        {
            if (m_start)
                (this->*m_start)(m_xml->name());
        }
        else if (m_xml->isEndElement())
        {
            if (m_end)
                (this->*m_end)(m_xml->name());
        }
        else if (m_xml->isCharacters() && !m_xml->isWhitespace())
        {
            if (m_accu)
                m_data += m_xml->text();

        }
    }
    if (m_xml->error() && m_xml->error() !=
            QXmlStreamReader::PrematureEndOfDocumentError)
    {
        qWarning("[BuildParser]: xml parsing error line %lli: %s",
                m_xml->lineNumber(), qPrintable(m_xml->errorString()));
        clear();
    }
}

bool BuildParser::parserStart(const QStringRef &name)
{
    if (name == "result")
    {
        m_start = NULL;
        m_end = &BuildParser::resultEnd;
        m_accu = true;
    }
    else if (name != "freeStyleBuild" &&
            name != "matrixBuild")
    {
        m_start = NULL;
        m_end = &BuildParser::waitEnd;
        m_opened = name.toString();
    }
    return true;
}

bool BuildParser::resultEnd(const QStringRef &name)
{
    if (name == "result")
    {
        Project::State state;
        if (m_data == "SUCCESS")
            state = Project::SUCCESS;
        else if (m_data == "UNSTABLE")
            state = Project::UNSTABLE;
        else if (m_data == "FAILURE")
            state = Project::FAILURE;
        else
            state = Project::UNKNOWN;

        emit projectEvent(state);

        m_start = &BuildParser::parserStart;
        m_end = NULL;
        m_accu = false;
    }
    return true;
}

bool BuildParser::waitEnd(const QStringRef &name)
{
    if (name == m_opened)
    {
        m_start = &BuildParser::parserStart;
        m_end = NULL;
    }
}

}

