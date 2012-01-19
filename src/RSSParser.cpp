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
** RSSParser.cpp
**
**        Created on: Nov 18, 2011
**   Original Author: fargie_s
**
*/

#include <QXmlStreamReader>
#include "RSSParser.hh"

namespace jenkins {

RSSParser::RSSParser() :
    m_xml(new QXmlStreamReader()),
    m_accu(false),
    m_start(&RSSParser::parserStart),
    m_end(NULL),
    m_titleEx("^(.+\\S)\\s*#(\\d+)\\s*\\((.+)\\)$"),
    m_uriEx("^(.*/)\\d+/?$")
{
}

RSSParser::~RSSParser()
{
    delete m_xml;
}

void RSSParser::parse(QIODevice *device)
{
    clear();
    m_xml->setDevice(device);
    if (device)
    {
        connect(device, SIGNAL(readyRead()),
                this, SLOT(parse()));
        /** @todo: fixme this signal should not be available here */
        connect(device, SIGNAL(finished()),
                this, SLOT(clear()));
        if (!device->atEnd())
            parse();
    }
}

void RSSParser::clear()
{
    if (m_xml->device() != NULL)
    {
        m_xml->device()->disconnect(this);
        emit finished();
    }
    m_xml->clear();
    m_start = &RSSParser::parserStart;
    m_end = NULL;
    m_accu = false;
    m_data.clear();
}

void RSSParser::parse()
{
    while (!m_xml->atEnd())
    {
        switch (m_xml->readNext())
        {
            case QXmlStreamReader::StartElement:
                if (m_start)
                    (this->*m_start)(m_xml->name());
                break;
            case QXmlStreamReader::EndElement:
                if (m_end)
                    (this->*m_end)(m_xml->name());
                break;
            case QXmlStreamReader::Characters:
                if (m_accu)
                    m_data += m_xml->text();
                break;
            case QXmlStreamReader::EndDocument:
                clear();
                break;
        }
        if (m_xml->error() != 0)
            break;
    }
    if (m_xml->error() && m_xml->error() !=
            QXmlStreamReader::PrematureEndOfDocumentError)
    {
        qWarning("[RSSParser]: xml parsing error line %lli: %s",
                m_xml->lineNumber(), qPrintable(m_xml->errorString()));
        clear();
    }
}

QIODevice *RSSParser::device()
{
    return m_xml->device();
}

bool RSSParser::parserStart(const QStringRef &name)
{
    if (name == "entry")
    {
        m_start = &RSSParser::entryStart;
        m_end = &RSSParser::entryEnd;

        m_name.clear();
        m_uri.clear();
        m_num = -1;
    }
    return true;
}

bool RSSParser::entryStart(const QStringRef &name)
{
    if (name == "title")
    {
        m_data.clear();
        m_accu = true;
    }
    else if (name == "link")
    {
        if (m_uriEx.indexIn(m_xml->attributes().value("href").toString()) != -1)
            m_uri = m_uriEx.cap(1);
        else
        {
            m_xml->raiseError("[RSSParser]: failed to parse link");
            return false;
        }
    }
    return true;
}

bool RSSParser::entryEnd(const QStringRef &name)
{
    if (name == "entry")
    {
        if (!m_uri.isEmpty() && !m_name.isEmpty() && m_num != -1)
            emit projectEvent(m_name, m_uri, m_num);
        else
            return false;

        m_start = &RSSParser::parserStart;
        m_end = NULL;
    }
    else if (name == "title")
    {
        m_accu = false;
        if (m_titleEx.indexIn(m_data) != -1)
        {
            m_name = m_titleEx.cap(1);
            m_num = m_titleEx.cap(2).toInt();
        }
        else
        {
            m_xml->raiseError("[RSSParser]: failed to parse title");
            return false;
        }
    }
    return true;
}

}

