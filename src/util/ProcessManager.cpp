// ProcessManager.cpp: implementation of the CProcessManager class.
//
//////////////////////////////////////////////////////////////////////

#include "ProcessManager.h"

#if defined(WIN32) && !defined(CYGNUS)
#  include <windows.h>
#else
#  include <unistd.h>
#endif

#ifdef HAVE_CSTRING
#include <cstring>
#else
#include <string.h>
#endif

#ifdef HAVE_CSTDIO
#include <cstdio>
#else
#include <stdio.h>
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CProcessManager::CProcessManager()
{

}

CProcessManager::~CProcessManager()
{

}

void CProcessManager::executeWait(char *commandLine)
{

    // Code ripped from FLTK demo, demo.cxx

#ifdef WIN32
    STARTUPINFO		suInfo;		// Process startup information
    PROCESS_INFORMATION	prInfo;		// Process information
    DWORD       exitCode;


    memset(&suInfo, 0, sizeof(suInfo));
    suInfo.cb = sizeof(suInfo);

    auto icommand_length = strlen(commandLine);

    char* copy_of_icommand = new char[icommand_length+1];
    strcpy(copy_of_icommand,commandLine); 

    // On WIN32 the .exe suffix needs to be appended to the command
    // whilst leaving any additional parameters unchanged - this
    // is required to handle the correct conversion of cases such as :
    // `../fluid/fluid valuators.fl' to '../fluid/fluid.exe valuators.fl'.

    // skip leading spaces.
    char* start_command = copy_of_icommand;
    while(*start_command == ' ') ++start_command;

    // find the space between the command and parameters if one exists.
    char* start_parameters = strchr(start_command,' ');

    char* command = new char[icommand_length+6]; // 6 for extra 'd.exe\0'

    if (start_parameters==NULL) { // no parameters required.
        sprintf(command, "%s.exe", start_command);
    } else { // parameters required.
        // break the start_command at the intermediate space between
        // start_command and start_parameters.
        *start_parameters = 0;
        // move start_paremeters to skip over the intermediate space.
        ++start_parameters;
    }

    sprintf(command, "%s.exe %s", start_command, start_parameters);

    CreateProcess(NULL, command, NULL, NULL, FALSE,
                  NORMAL_PRIORITY_CLASS, NULL, NULL, &suInfo, &prInfo);

    // Wait for program to finish....

    exitCode = STILL_ACTIVE;
    while ( exitCode == STILL_ACTIVE )
    {
        Sleep(250);
        if ( !GetExitCodeProcess(prInfo.hProcess, &exitCode) )
        {
            sprintf(commandLine, "Decompress: GetExitCodeProcess failed: %i",
                    GetLastError());
        }
    }

    delete command;
    delete copy_of_icommand;

#else // NON WIN32 systems.
#ifdef __APPLE__

#else
    int icommand_length = strlen(commandLine);
    char* command = new char[icommand_length+5]; // 5 for extra './' and ' &\0'

    sprintf(command, "./%s", commandLine);
    system(command);

    delete command;
#endif
#endif // WIN32

}

void CProcessManager::execute(char *commandLine)
{

    // Code ripped from FLTK demo, demo.cxx

#ifdef WIN32
    STARTUPINFO		suInfo;		// Process startup information
    PROCESS_INFORMATION	prInfo;		// Process information


    memset(&suInfo, 0, sizeof(suInfo));
    suInfo.cb = sizeof(suInfo);

    int icommand_length = strlen(commandLine);

    char* copy_of_icommand = new char[icommand_length+1];
    strcpy(copy_of_icommand,commandLine);

    // On WIN32 the .exe suffix needs to be appended to the command
    // whilst leaving any additional parameters unchanged - this
    // is required to handle the correct conversion of cases such as :
    // `../fluid/fluid valuators.fl' to '../fluid/fluid.exe valuators.fl'.

    // skip leading spaces.
    char* start_command = copy_of_icommand;
    while(*start_command == ' ') ++start_command;

    // find the space between the command and parameters if one exists.
    char* start_parameters = strchr(start_command,' ');

    char* command = new char[icommand_length+6]; // 6 for extra 'd.exe\0'

    if (start_parameters==NULL) { // no parameters required.
        sprintf(command, "%s.exe", start_command);
    } else { // parameters required.
        // break the start_command at the intermediate space between
        // start_command and start_parameters.
        *start_parameters = 0;
        // move start_paremeters to skip over the intermediate space.
        ++start_parameters;
    }

    sprintf(command, "%s.exe %s", start_command, start_parameters);

    CreateProcess(NULL, command, NULL, NULL, FALSE,
                  NORMAL_PRIORITY_CLASS, NULL, NULL, &suInfo, &prInfo);

    delete command;
    delete copy_of_icommand;

#else // NON WIN32 systems.
#ifdef __APPLE__

#else
    int icommand_length = strlen(commandLine);
    char* command = new char[icommand_length+5]; // 5 for extra './' and ' &\0'

    sprintf(command, "./%s &", commandLine);
    system(command);

    delete command;
#endif
#endif // WIN32

}

