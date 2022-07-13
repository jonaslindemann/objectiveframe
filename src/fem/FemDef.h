#ifndef FEMDEF_H
#define FEMDEF_H

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

#ifndef BOOL
typedef int BOOL;
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef NULL
#ifdef __cplusplus
#define NULL 0
#else
#define NULL ((void*) 0)
#endif
#endif

#define FemStdPointer(classname) \
	class classname; \
	typedef classname* classname##Ptr

#define FemSmartPointer(classname) \
	class classname; \
	typedef FemPointer<classname> classname##Ptr; \
	typedef classname* classname##StdPtr;

#define FemSmartPointerRefBase(classname,refbase) \
	class classname; \
	typedef FemPointerRefBase<classname,refbase> classname##Ptr; \
	typedef classname* classname##StdPtr;

#define FemClassInfo(femclassname,femparent) \
	const std::string getClassNameThis() { return femclassname; } \
	virtual const std::string getClassName() { return femclassname; } \
	virtual bool isClass(const std::string& name) { \
		std::string className; \
		className = getClassNameThis(); \
		if (!className.empty()) { \
			if (className == name) \
				return true; \
			else \
				return femparent::isClass(name); \
		}\
		else \
			return false; \
	}

#define FemClassInfoTop(femclassname) \
	void getClassNameThis(std::string& name) { name = femclassname; } \
	const std::string getClassNameThis() { return femclassname; } \
	virtual void getClassName(std::string& name) { name = femclassname; } \
	virtual const std::string getClassName() { return femclassname; } \
	virtual bool isClass(const std::string& name) { \
	    std::string className = ""; \
		className = getClassNameThis(); \
		if (!className.empty()) { \
			if (className == name) \
				return true; \
			else \
				return false; \
		}\
		else \
			return false; \
	}

#endif
