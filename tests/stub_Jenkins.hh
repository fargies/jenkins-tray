/*
** stub_Jenkins.hh
**
**        Created on: Jan 08, 2012
*/

#ifndef __STUB_JENKINS_HH__
#define __STUB_JENKINS_HH__

#include <QTcpServer>
#include <QMap>
#include <QString>
#include <QIODevice>

class JenkinsServerStub : public QTcpServer
{
    Q_OBJECT;

public:
    JenkinsServerStub(QObject* parent = 0);
    virtual ~JenkinsServerStub();

    void incomingConnection(int socket);

    inline void add(const QString &path, const QString &data)
    {
        m_data.insert(path, data);
    }

    inline void clear()
    {
        m_data.clear();
    }

    QString baseUri() const;

private slots:
    void readClient();

    void discardClient();

protected:
    QMap<QString, QString> m_data;
};

#endif

