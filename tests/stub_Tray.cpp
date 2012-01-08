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
** stub_tray.cpp
**
**        Created on: Jan 07, 2012
**   Original Author: fargie_s
**
*/

#include <cppunit/TestAssert.h>

#include <QApplication>
#include <QDebug>

#include "stub_Tray.hh"
#include "stub_Settings.hh"

TrayStub::TrayStub() :
    Tray()
{
    delete m_settings;
    m_settings = new SettingsStub();
}

TrayStub::~TrayStub()
{
}

SettingsStub &TrayStub::getSettingsStub()
{
    SettingsStub *stub = dynamic_cast<SettingsStub *>(m_settings);

    CPPUNIT_ASSERT(stub != NULL);
    return *stub;
}

