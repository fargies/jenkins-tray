/*
** stub_Jenkins.cpp
**
**        Created on: Jan 08, 2012
*/

#include <QtGlobal>
#include <QTcpSocket>
#include <QHostAddress>
#include <QRegExp>
#include <QStringList>
#include <QFile>

#include "stub_Jenkins.hh"

#define HTTP_200 "HTTP/1.0 200 Ok\r\n" \
    "Content-Type: text/html; charset=\"utf-8\"\r\n" \
    "\r\n"
#define HTTP_404 "HTTP/1.0 404 Not Found\r\n" \
    "Content-Type: text/html; charset=\"utf-8\"\r\n" \
    "\r\n"
#define SERVER_PORT 12345

JenkinsServerStub::JenkinsServerStub(QObject *parent) :
    QTcpServer(parent)
{
        listen(QHostAddress::LocalHost, SERVER_PORT);
}

JenkinsServerStub::~JenkinsServerStub()
{
}

void JenkinsServerStub::incomingConnection(int socket)
{
    QTcpSocket* s = new QTcpSocket(this);

    connect(s, SIGNAL(readyRead()), this, SLOT(readClient()));
    connect(s, SIGNAL(disconnected()), this, SLOT(discardClient()));
    s->setSocketDescriptor(socket);
}

QString JenkinsServerStub::baseUri() const
{
    return QString("http://%1:%2").arg(serverAddress().toString()).arg(serverPort());
}

void JenkinsServerStub::readClient()
{
    QTcpSocket* socket = (QTcpSocket*) sender();
    if (socket->canReadLine()) {
        QStringList tokens = QString(socket->readLine()).split(QRegExp("[ \r\n][ \r\n]*"));
        if (tokens.size() >= 2 && tokens[0] == "GET") {
            QTextStream os(socket);
            os.setAutoDetectUnicode(true);

            QMap<QString, QString>::const_iterator it =
                m_data.find(tokens[1]);

            if (it != m_data.end())
            {
                QFile f(it.value());

                if (f.open(QIODevice::ReadOnly))
                {
                    os << HTTP_200 << f.readAll();
                    f.close();
                }
                else
                {
                    qWarning("[JenkinsServerStub]: Failed to open file: %s",
                            qPrintable(it.value()));
                    os << HTTP_404 << "<h1>Failed to open \"" << tokens[1]
                        << "\"</h1>\n";
                }
            }
            else
            {
                qWarning("[JenkinsServerStub]: No such file: %s",
                    qPrintable(tokens[1]));
                os << HTTP_404 << "<h1>Nothing to see here</h1>\n";
            }
            socket->close();

            if (socket->state() == QTcpSocket::UnconnectedState) {
                delete socket;
            }
        }
    }
}

void JenkinsServerStub::discardClient()
{
    QTcpSocket* socket = (QTcpSocket*) sender();
    socket->deleteLater();
}

