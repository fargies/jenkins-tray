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
** jenkinsEngine.cpp
**
**        Created on: Nov 18, 2011
**   Original Author: fargie_s
**
*/

#include <QXmlSimpleReader>
#include <QXmlSimpleReader>
#include <QXmlDefaultHandler>
#include "jenkinsRSSEngine.hh"
#include "jenkinsRSSParser.hh"

JenkinsRSSEngine::JenkinsRSSEngine() :
    m_reader(new QXmlSimpleReader()),
    m_source(NULL)
{
    JenkinsRSSParser *parser = new JenkinsRSSParser();
    m_reader->setContentHandler(parser);
    m_reader->setErrorHandler(parser);
    connect(parser, SIGNAL(projectEvent(const QString &, const QUrl &, int)),
            this, SIGNAL(projectEvent(const QString &, const QUrl &, int)));
}

JenkinsRSSEngine::~JenkinsRSSEngine()
{
    delete m_reader->contentHandler();
    delete m_reader;
    if (m_source)
        delete m_source;
}

bool JenkinsRSSEngine::parse(QIODevice *input)
{
    bool ret;

    if (m_source)
        delete m_source;

    m_source = new QXmlInputSource(input);

    m_source->fetchData();
    ret = m_reader->parse(m_source, true);

    connect(input, SIGNAL(readyRead()), this, SLOT(parseContinue()));

    return ret;
}

void JenkinsRSSEngine::parseContinue()
{
    m_reader->parseContinue();
}

