//
// Copyright 1999-2011 by Structural Mechanics, Lund University.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA.
//
// Please report all bugs and problems to "ivf@byggmek.lth.se".
//
//
// Written by Jonas Lindemann
//

#pragma once

#include <ofem/def.h>

namespace ofem {

template <class T> class Pointer {
private:
    T *m_object;

public:
    Pointer(T *object = nullptr)
    {
        m_object = object;
        if (m_object)
            m_object->addReference();
    }

    Pointer(const Pointer &femObject)
    {
        m_object = femObject.m_object;
        if (m_object)
            m_object->addReference();
    }

    virtual ~Pointer()
    {
        if (m_object) {
            m_object->deleteReference();
            if (!m_object->isReferenced()) {
                delete m_object;
                m_object = nullptr;
            }
        }
    }

    operator T *() const
    {
        return m_object;
    }

    T &operator*() const
    {
        return *m_object;
    }

    T *operator->() const
    {
        return m_object;
    }

    T *get() const
    {
        return m_object;
    }

    Pointer &operator=(const Pointer &femPointer)
    {
        if (m_object != femPointer.m_object) {
            if (m_object) {
                m_object->deleteReference();
                if (!m_object->isReferenced()) {
                    delete m_object;
                    m_object = nullptr;
                }
            }

            m_object = femPointer.m_object;

            if (m_object)
                m_object->addReference();
        }
        return *this;
    }

    Pointer &operator=(T *femObject)
    {
        if (m_object != femObject) {
            if (m_object) {
                m_object->deleteReference();
                if (!m_object->isReferenced()) {
                    delete m_object;
                    m_object = nullptr;
                }
            }

            m_object = femObject;

            if (m_object)
                m_object->addReference();
        }
        return *this;
    }

    bool operator==(T *femObject) const
    {
        return m_object == femObject;
    }
    bool operator!=(T *femObject) const
    {
        return m_object != femObject;
    }
    bool operator==(const Pointer &femPointer) const
    {
        return m_object == femPointer.m_object;
    }

    bool operator!=(const Pointer &femPointer) const
    {
        return m_object != femPointer.m_object;
    }
};
} // namespace ofem
