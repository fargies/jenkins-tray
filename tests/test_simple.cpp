/*
** Copyright (C) 2012 Fargier Sylvain <fargier.sylvain@free.fr>
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
** test_simple.cpp
**
**        Created on: Jan 07, 2012
**   Original Author: fargie_s
**
*/

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include <QApplication>

#include "RSSParser.hh"
#include "Downloader.hh"

#include "test_helpers.hh"
#include "stub_Tray.hh"
#include "stub_Settings.hh"
#include "stub_Jenkins.hh"

using namespace jenkins;

class simpleTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(simpleTest);
    CPPUNIT_TEST(simple);
    CPPUNIT_TEST(auth);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp()
    {
        m_argc = 0;

        m_app = new QApplication(m_argc, m_argv);
        m_app->setQuitOnLastWindowClosed(false);
        m_jenkins = new JenkinsServerStub(m_app);

        Q_INIT_RESOURCE(Tray);
        Q_INIT_RESOURCE(test_resources);
        m_tray = new TrayStub();
    }

    void tearDown()
    {
        delete m_jenkins;
        delete m_tray;
        jenkins::Downloader::destroy();
        delete m_app;
    }

protected:
    int m_argc;
    char *m_argv[0];
    JenkinsServerStub *m_jenkins;
    QApplication      *m_app;
    TrayStub          *m_tray;

    bool wait(int timeout)
    {
        QTimer timer;

        if (timeout > 0)
        {
            QObject::connect(&timer, SIGNAL(timeout()), m_app, SLOT(quit()));
            timer.setInterval(timeout);
            timer.setSingleShot(true);
            timer.start();
        }
        m_app->exec();
        return !timer.isActive();
    }

    /**
     * @details
     * Tests details :
     *  - First we create stub informations in JenkinsServerStub.
     *  - Then the tray is launched.
     *  - Loaded information is checked.
     *  - Server's information is modified.
     *  - Since the Tray application has an RSS polling interval of 1 it will
     *  update the information.
     *  - Updated information is checked.
     */
    void simple()
    {
        m_jenkins->add(QString("/simple/rssLatest"),
                ":/simple_rss");

        SettingsStub &stub = m_tray->getSettingsStub();
        stub.setUrl(m_jenkins->baseUri() + "/simple");
        stub.setInterval(1);

        m_tray->start();

        QObject::connect(m_tray->getParser(), SIGNAL(finished()), m_app,
                SLOT(quit()));
        CPPUNIT_ASSERT(!wait(30000));
        QObject::disconnect(m_tray->getParser(), SIGNAL(finished()), m_app,
                SLOT(quit()));

        QMap<QString, Project *> &projs = m_tray->getProjects();
        CPPUNIT_ASSERT_EQUAL(4, projs.size());

        foreach(Project *p, projs)
        {
            CPPUNIT_ASSERT_EQUAL(Project::UNKNOWN, p->getState());
        }

        m_jenkins->add(QString("/job/jenkins-target/13/api/xml"),
                ":/simple_target_13");

        QMap<QString, Project *>::const_iterator it = projs.find("jenkins-target");
        CPPUNIT_ASSERT(it != projs.end());
        QObject::connect(it.value(), SIGNAL(updated(const Project &)),
                m_app, SLOT(quit()));
        CPPUNIT_ASSERT(!wait(30000));

        it = projs.find("jenkins-target");
        CPPUNIT_ASSERT(it != projs.end());
        CPPUNIT_ASSERT_EQUAL(Project::SUCCESS, it.value()->getState());
    }

    void auth()
    {
        m_jenkins->setAuth("bob", "bob's token");
        m_jenkins->add(QString("/simple/rssLatest"),
                ":/simple_rss");

        SettingsStub &stub = m_tray->getSettingsStub();
        stub.setUrl(m_jenkins->baseUri() + "/simple");
        stub.setInterval(1);
        stub.setUser("");
        stub.setAuthToken("");

        m_tray->start();

        QObject::connect(m_tray, SIGNAL(finished()), m_app,
                SLOT(quit()));
        CPPUNIT_ASSERT(!wait(30000));
        CPPUNIT_ASSERT(m_tray->getProjects().isEmpty());

        stub.setUser("roger");
        stub.setAuthToken("roger's token");
        m_tray->start(); /* restart the timer */

        CPPUNIT_ASSERT(!wait(30000));
        CPPUNIT_ASSERT(m_tray->getProjects().isEmpty());

        stub.setUser("bob");
        stub.setAuthToken("bob's token");
        m_tray->start(); /* restart the timer */

        CPPUNIT_ASSERT(!wait(30000));
        QMap<QString, Project *> &projs = m_tray->getProjects();
        CPPUNIT_ASSERT_EQUAL(4, projs.size());

        foreach(Project *p, projs)
        {
            CPPUNIT_ASSERT_EQUAL(Project::UNKNOWN, p->getState());
        }
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(simpleTest);

