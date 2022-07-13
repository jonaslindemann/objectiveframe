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

#ifndef _CFemPointer_h_
#define _CFemPointer_h_

#include "FemDef.h"

/**
 * Smart pointer class
 *
 * CIvfPointer handles the Ivf++ reference counting scheme of
 * the CIvfBase class. To declare a Ivf++ smart pointer use the
 * IvfSmartPointer() macro. See the following example:
 *
 * \code
 * int main()
 * {
 *    CIvfPointer<CIvfMaterial> material = new CIvfMaterial(); // addReference() called.
 *    CIvfPointer<CIvfMaterial> material2;
 *    material2 = material; // addReference() called
 *    .
 *    .
 *
 *    return 0;
 * }
 * // material calls deleteReference()
 * // material2 calls deleteRefernce() and deletes CIvfMaterial object
 * \endcode
 */
template <class T,class R> class FemPointerRefBase {
private:
    T* m_object;
public:
    FemPointerRefBase(T* object = 0)
    {
        m_object = object;
        if (m_object)
            m_object->R::addReference();
    }

    FemPointerRefBase(const FemPointerRefBase& femObject)
    {
        m_object = femObject.m_object;
        if (m_object)
            m_object->R::addReference();
    }

    virtual ~FemPointerRefBase()
    {
        if (m_object)
        {
            m_object->R::deleteReference();
            if (!m_object->R::referenced())
                delete m_object;
        }
    }

    operator T* () const {
        return m_object;
    }
    T& operator* () const {
        return *m_object;
    }
    T* operator-> () const {
        return m_object;
    }

    FemPointerRefBase& operator= (const FemPointerRefBase& FemPointerRefBase)
    {
        if (m_object!=FemPointerRefBase.m_object)
        {
            if (m_object)
            {
                m_object->R::deleteReference();
                if (!m_object->R::isReferenced())
                    delete m_object;
            }

            m_object = FemPointerRefBase.m_object;

            if (m_object)
                m_object->R::addReference();
        }
        return *this;
    }

    FemPointerRefBase& operator= (T* femObject)
    {
        if (m_object!=femObject)
        {
            if (m_object)
            {
                m_object->R::deleteReference();
                if (!m_object->R::referenced())
                    delete m_object;
            }

            m_object = femObject;

            if (m_object)
                m_object->R::addReference();
        }
        return *this;
    }

    bool operator== (T* femObject) const {
        return m_object == femObject;
    }
    bool operator!= (T* femObject) const {
        return m_object != femObject;
    }
    bool operator== (const FemPointerRefBase& FemPointerRefBase) const
    {
        return m_object == FemPointerRefBase.m_object;
    }

    bool operator!= (const FemPointerRefBase& FemPointerRefBase) const
    {
        return m_object != FemPointerRefBase.m_object;
    }
};

template <class T> class FemPointer {
private:
    T* m_object;
public:
    FemPointer(T* object = 0)
    {
        m_object = object;
        if (m_object)
            m_object->addReference();
    }

    FemPointer(const FemPointer& femObject)
    {
        m_object = femObject.m_object;
        if (m_object)
            m_object->addReference();
    }

    virtual ~FemPointer()
    {
        if (m_object)
        {
            m_object->deleteReference();
            if (!m_object->isReferenced())
                delete m_object;
        }
    }

    operator T* () const {
        return m_object;
    }
    T& operator* () const {
        return *m_object;
    }
    T* operator-> () const {
        return m_object;
    }

    FemPointer& operator= (const FemPointer& femPointer)
    {
        if (m_object!=femPointer.m_object)
        {
            if (m_object)
            {
                m_object->deleteReference();
                if (!m_object->isReferenced())
                    delete m_object;
            }

            m_object = femPointer.m_object;

            if (m_object)
                m_object->addReference();
        }
        return *this;
    }

    FemPointer& operator= (T* femObject)
    {
        if (m_object!=femObject)
        {
            if (m_object)
            {
                m_object->deleteReference();
                if (!m_object->isReferenced())
                    delete m_object;
            }

            m_object = femObject;

            if (m_object)
                m_object->addReference();
        }
        return *this;
    }

    bool operator== (T* femObject) const {
        return m_object == femObject;
    }
    bool operator!= (T* femObject) const {
        return m_object != femObject;
    }
    bool operator== (const FemPointer& femPointer) const
    {
        return m_object == femPointer.m_object;
    }

    bool operator!= (const FemPointer& femPointer) const
    {
        return m_object != femPointer.m_object;
    }
};


#endif
