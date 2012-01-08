/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtTest module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qsignaldumper.h"

#include <QList>
#include <QMetaObject>
#include <QMetaType>
#include <QMetaMethod>
#include <QObject>
#include <QVariant>

#include <QDebug>


inline static void qPrintMessage(const QByteArray &ba)
{
    qDebug("%s", ba.constData());
}

Q_GLOBAL_STATIC(QList<QByteArray>, ignoreClasses)
static int iLevel = 0;
static int ignoreLevel = 0;
enum { IndentSpacesCount = 4 };

static QByteArray memberName(const QMetaMethod &member)
{
    QByteArray ba = member.signature();
    return ba.left(ba.indexOf('('));
}

static void qSignalDumperCallback(QObject *caller, int method_index, void **argv)
{
    Q_ASSERT(caller); Q_ASSERT(argv); Q_UNUSED(argv);
    const QMetaObject *mo = caller->metaObject();
    Q_ASSERT(mo);
    QMetaMethod member = mo->method(method_index);
    Q_ASSERT(member.signature());

    if (ignoreClasses() && ignoreClasses()->contains(mo->className())) {
        ++ignoreLevel;
        return;
    }

    QByteArray str;
    str.fill(' ', iLevel++ * IndentSpacesCount);
    str += "Signal: ";
    str += mo->className();
    str += '(';

    QString objname = caller->objectName();
    str += objname.toLocal8Bit();
    if (!objname.isEmpty())
        str += ' ';
    str += QByteArray::number(quintptr(caller), 16);

    str += ") ";
    str += memberName(member);
    str += " (";

    QList<QByteArray> args = member.parameterTypes();
    for (int i = 0; i < args.count(); ++i) {
        const QByteArray &arg = args.at(i);
        int typeId = QMetaType::type(args.at(i).constData());
        if (arg.endsWith('*') || arg.endsWith('&')) {
            str += '(';
            str += arg;
            str += ')';
            if (arg.endsWith('&'))
                str += '@';

            quintptr addr = quintptr(*reinterpret_cast<void **>(argv[i + 1]));
            str.append(QByteArray::number(addr, 16));
        } else if (typeId != QMetaType::Void) {
            str.append(arg)
                .append('(')
                .append(QVariant(typeId, argv[i + 1]).toString().toLocal8Bit())
                .append(')');
        }
        str.append(", ");
    }
    if (str.endsWith(", "))
        str.chop(2);
    str.append(')');
    qPrintMessage(str);
}

static void qSignalDumperCallbackSlot(QObject *caller, int method_index, void **argv)
{
    Q_ASSERT(caller); Q_ASSERT(argv); Q_UNUSED(argv);
    const QMetaObject *mo = caller->metaObject();
    Q_ASSERT(mo);
    QMetaMethod member = mo->method(method_index);
    if (!member.signature())
        return;

    if (ignoreLevel ||
            (ignoreClasses() && ignoreClasses()->contains(mo->className())))
        return;

    QByteArray str;
    str.fill(' ', iLevel * IndentSpacesCount);
    str += "Slot: ";
    str += mo->className();
    str += '(';

    QString objname = caller->objectName();
    str += objname.toLocal8Bit();
    if (!objname.isEmpty())
        str += ' ';
    str += QByteArray::number(quintptr(caller), 16);

    str += ") ";
    str += member.signature();
    qPrintMessage(str);
}

static void qSignalDumperCallbackEndSignal(QObject *caller, int /*method_index*/)
{
    Q_ASSERT(caller); Q_ASSERT(caller->metaObject());
    if (ignoreClasses()
            && ignoreClasses()->contains(caller->metaObject()->className())) {
        --ignoreLevel;
        Q_ASSERT(ignoreLevel >= 0);
        return;
    }
    --iLevel;
    Q_ASSERT(iLevel >= 0);
}

// this struct is copied from qobject_p.h to prevent us
// from including private Qt headers.
struct QSignalSpyCallbackSet
{
    typedef void (*BeginCallback)(QObject *caller, int method_index, void **argv);
    typedef void (*EndCallback)(QObject *caller, int method_index);
    BeginCallback signal_begin_callback,
                  slot_begin_callback;
    EndCallback signal_end_callback,
                slot_end_callback;
};
extern void Q_CORE_EXPORT qt_register_signal_spy_callbacks(const QSignalSpyCallbackSet &);

void QSignalDumper::startDump()
{
    static QSignalSpyCallbackSet set = { qSignalDumperCallback,
        qSignalDumperCallbackSlot, qSignalDumperCallbackEndSignal, 0 };
    qt_register_signal_spy_callbacks(set);
}

void QSignalDumper::endDump()
{
    static QSignalSpyCallbackSet nset = { 0, 0, 0 ,0 };
    qt_register_signal_spy_callbacks(nset);
}

void QSignalDumper::ignoreClass(const QByteArray &klass)
{
    if (ignoreClasses())
        ignoreClasses()->append(klass);
}

void QSignalDumper::clearIgnoredClasses()
{
    if (ignoreClasses())
        ignoreClasses()->clear();
}

