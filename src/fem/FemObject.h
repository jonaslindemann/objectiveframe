#ifndef _CFemObject_H_
#define _CFemObject_H_

#include <iostream>
#include <string>

#include <json.hpp>

using json = nlohmann::json;

#include "FemDef.h"
#include "FemPointer.h"

FemSmartPointer(FemObject);

class FemObject {
private:
    int m_ref;
    void* m_user;
	bool m_readOnly;
public:
    FemObject ();
    virtual ~FemObject ();

    // Class info

    /**
     * Class name query.
     *
     * Returns true if name is equal to the name of the class.
     * @param name Class name to be queried.
     */
    virtual bool isClass(const std::string& name);

    /**
     * Returns the name of the class.
     *
     * The name buffer must be large enough to hold the
     * name of the class.
     * @param name Reference to a string buffer.
     */
    const std::string getClassNameThis();
    virtual const std::string getClassName();
    // Reference counting

    void addReference();
    void deleteReference();
    bool isReferenced();
    int getRefCount();

    // Methods

	bool isReadOnly();
	void setReadOnly(bool flag = true);

    virtual void saveToStream(std::ostream &out);
    virtual void readFromStream(std::istream &in);
    virtual void print(std::ostream &out);

    virtual json toJSON();
    virtual void fromJSON(json& j);

    // Get/set methods

    void setUser(void* user);
    void* getUser();
};

#endif
