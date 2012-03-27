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
** jenkinsDownloader.cpp
**
**        Created on: Nov 17, 2011
**   Original Author: fargie_s
**
*/

#include <QNetworkRequest>
#include "Downloader.hh"

namespace jenkins {

Downloader::Downloader() :
    m_auth(false)
{
}

QNetworkReply *Downloader::get(const QUrl &url)
{
    QNetworkRequest request;
    request.setUrl(url);
    request.setRawHeader("User-Agent", "JenkinsTray 1.0");

    /* activate the preemptive authentication by giving a wrong login/password,
     * authenticationRequired signal will be triggered and next requests will
     * use the cached tokens */
    if (m_auth)
        request.setRawHeader("Authorization", "Basic " +  QByteArray(":").toBase64());

    QNetworkReply *reply = QNetworkAccessManager::get(request);
    connect(reply, SIGNAL(finished()), reply, SLOT(deleteLater()));
    return reply;
}

void Downloader::enableAuth(bool enable)
{
    m_auth = enable;
}

Downloader::~Downloader()
{
}

Downloader *Downloader::instance()
{
    if (m_instance == NULL)
        m_instance = new Downloader();
    return m_instance;
}

void Downloader::destroy()
{
    if (m_instance != NULL)
    {
        delete m_instance;
        m_instance = NULL;
    }
}

Downloader *Downloader::m_instance = NULL;

}

