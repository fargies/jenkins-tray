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
** jenkinsStatus.hh
**
**        Created on: Nov 18, 2011
**   Original Author: fargie_s
**
*/

#ifndef __JENKINS_STATUS_HH__
#define __JENKINS_STATUS_HH__

#include <QString>

class JenkinsStatus
{
public:
    enum State
    {
        SUCCESS;
        FAILURE;
        UNSTABLE;
    };

    JenkinsStatus();
    ~JenkinsStatus();

    inline const QString &getName() const
    {
        return m_name;
    }

    inline void setName(const QString &name)
    {
        m_name = name;
    }

    inline const QString &getID() const
    {
        return m_id;
    }

    inline void setID(const QString &id)
    {
        m_id = id;
    }

    inline int getNum() const
    {
        return m_num;
    }

    inline void setNum(int num)
    {
        m_num = num;
    }

    void clear();

protected:
    QString m_id;
    QString m_name;
    int     m_num;
};

#endif

