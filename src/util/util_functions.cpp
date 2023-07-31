#include <ofutil/util_functions.h>

#include <sstream>

#ifdef WIN32
#include <windows.h>
#endif

namespace ofutil {

#ifdef WIN32

int run_process(std::string cmd)
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    LPSTR s = _strdup(cmd.c_str());

    // Start the child process.
    auto result = CreateProcessA(NULL,             // No module name (use command line)
                                 s,                // Command line
                                 NULL,             // Process handle not inheritable
                                 NULL,             // Thread handle not inheritable
                                 FALSE,            // Set handle inheritance to FALSE
                                 CREATE_NO_WINDOW, // No creation flags
                                 NULL,             // Use parent's environment block
                                 NULL,             // Use parent's starting directory
                                 &si,              // Pointer to STARTUPINFO structure
                                 &pi);             // Pointer to PROCESS_INFORMATION structure

    if (result == 0) {
        printf("CreateProcess failed (%d).\n", GetLastError());
        return 1;
    }

    free(s);

    // Wait until child process exits.
    WaitForSingleObject(pi.hProcess, INFINITE);

    // Close process and thread handles.
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return 0;
}

#endif

std::string to_string(float value)
{
    std::ostringstream ss;
    ss << value;
    return ss.str();
}

std::string to_string(double value)
{
    std::ostringstream ss;
    ss << value;
    return ss.str();
}

std::string to_string(int value)
{
    std::ostringstream ss;
    ss << value;
    return ss.str();
}

std::string to_string(size_t value)
{
    std::ostringstream ss;
    ss << value;
    return ss.str();
}

std::string to_coord_string(double x, double y)
{
    std::ostringstream ss;
    ss << "(" << x << ", " << y << ")";
    return ss.str();
}

std::string to_coord_string(double x, double y, double z)
{
    std::ostringstream ss;
    ss << "(" << x << ", " << y << ", " << z << ")";
    return ss.str();
}
float to_float(std::string str)
{
    std::stringstream ss(str);

    float f;
    if (ss >> f)
        return f;
    else
        return 0.0;
}
int to_int(std::string str)
{
    std::stringstream ss(str);

    int f;
    if (ss >> f)
        return f;
    else
        return -1;
}
} // namespace ofutil
