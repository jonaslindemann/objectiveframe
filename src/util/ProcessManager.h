#ifndef _CProcessManager_h_
#define _CProcessManager_h_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

class CProcessManager
{
private:
public:
    CProcessManager();
    virtual ~CProcessManager();

    void executeWait(char* commandLine);
    void execute(char* commandLine);
};

#endif
