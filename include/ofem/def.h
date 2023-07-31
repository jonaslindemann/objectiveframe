#pragma once

#include <memory>

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

#define StdPointer(classname)                                                                                          \
    class classname;                                                                                                   \
    typedef classname *classname##Ptr

#undef USE_SHARED_PTR

#ifdef USE_SHARED_PTR

#define SmartPointer(classname)                                                                                        \
    class classname;                                                                                                   \
    typedef std::shared_ptr<classname> classname##Ptr;                                                                 \
    typedef classname *classname##StdPtr;

#define StdFactory(classname)                                                                                          \
    static classname##Ptr create()                                                                                     \
    {                                                                                                                  \
        return std::make_shared<classname>();                                                                          \
    }

#else

#define SmartPointer(classname)                                                                                        \
    class classname;                                                                                                   \
    typedef ofem::Pointer<classname> classname##Ptr;                                                                   \
    typedef classname *classname##StdPtr;

#define StdFactory(classname)                                                                                          \
    static classname##Ptr create()                                                                                     \
    {                                                                                                                  \
        return classname##Ptr(new classname());                                                                        \
    }

#endif

#define ClassInfo(femclassname, femparent)                                                                             \
    const std::string getClassNameThis()                                                                               \
    {                                                                                                                  \
        return femclassname;                                                                                           \
    }                                                                                                                  \
    virtual const std::string getClassName() override                                                                  \
    {                                                                                                                  \
        return femclassname;                                                                                           \
    }                                                                                                                  \
    virtual bool isClass(const std::string &name) override                                                             \
    {                                                                                                                  \
        std::string className;                                                                                         \
        className = getClassNameThis();                                                                                \
        if (!className.empty()) {                                                                                      \
            if (className == name)                                                                                     \
                return true;                                                                                           \
            else                                                                                                       \
                return femparent::isClass(name);                                                                       \
        }                                                                                                              \
        else                                                                                                           \
            return false;                                                                                              \
    }

#define ClassInfoTop(femclassname)                                                                                     \
    void getClassNameThis(std::string &name)                                                                           \
    {                                                                                                                  \
        name = femclassname;                                                                                           \
    }                                                                                                                  \
    const std::string getClassNameThis()                                                                               \
    {                                                                                                                  \
        return femclassname;                                                                                           \
    }                                                                                                                  \
    virtual void getClassName(std::string &name)                                                                       \
    {                                                                                                                  \
        name = femclassname;                                                                                           \
    }                                                                                                                  \
    virtual const std::string getClassName()                                                                           \
    {                                                                                                                  \
        return femclassname;                                                                                           \
    }                                                                                                                  \
    virtual bool isClass(const std::string &name)                                                                      \
    {                                                                                                                  \
        std::string className = "";                                                                                    \
        className = getClassNameThis();                                                                                \
        if (!className.empty()) {                                                                                      \
            if (className == name)                                                                                     \
                return true;                                                                                           \
            else                                                                                                       \
                return false;                                                                                          \
        }                                                                                                              \
        else                                                                                                           \
            return false;                                                                                              \
    }
