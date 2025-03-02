#include <ofutil/util_functions.h>

#include <filesystem>
#include <format>
#include <iostream>
#include <fstream>
#include <sstream>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#ifdef WIN32
#include <shlobj.h>
#include <windows.h>
#pragma comment(lib, "shell32.lib")
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

    if (result == 0)
    {
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
    return std::format("{}", value);
    /*
    std::ostringstream ss;
    ss << value;
    return ss.str();
    */
}

std::string to_string(double value)
{
    return std::format("{}", value);
    /*
    std::ostringstream ss;
    ss << value;
    return ss.str();
    */
}

std::string to_string(int value)
{
    return std::format("{}", value);
    /*
    std::ostringstream ss;
    ss << value;
    return ss.str();
    */
}

std::string to_string(size_t value)
{
    return std::format("{}", value);
    /*
    std::ostringstream ss;
    ss << value;
    return ss.str();
    */
}

std::string to_coord_string(double x, double y)
{
    return std::format("({}, {})", x, y);
    /*
    std::ostringstream ss;
    ss << "(" << x << ", " << y << ")";
    return ss.str();
    */
}

std::string to_coord_string(double x, double y, double z)
{
    return std::format("({}, {}, {})", x, y, z);
    /*
    std::ostringstream ss;
    ss << "(" << x << ", " << y << ", " << z << ")";
    return ss.str();
    */
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

std::string read_file(std::string filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cout << "Couldn't open file: " << filename << std::endl;
        return std::string();
    }

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    return content;
}

std::string doc_folder()
{
#ifdef WIN32
    CHAR my_documents[MAX_PATH];
    HRESULT result = SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, my_documents);

    if (result == S_OK)
        return std::string(my_documents);
    else
        return std::string();
#else
    return std::string(getenv("HOME")) + "/Documents";
#endif

    return std::string(my_documents);
}

std::string samples_folder()
{
    namespace fs = std::filesystem;
    auto docs = doc_folder();

    fs::path samples = fs::path(docs) / fs::path("ObjectiveFrame Samples");

    return samples.string();
}

std::string get_config_value(std::string key, std::string default_value)
{
    std::string subKey{"Software\\ObjectiveFrame"};
    std::string value{""};

    HKEY hKey;
    LONG lResult = RegOpenKeyEx(HKEY_CURRENT_USER, subKey.c_str(), 0, KEY_READ, &hKey);
    if (lResult != ERROR_SUCCESS)
    {
        std::cout << "Couldn't open registry key - returning default value" << std::endl;
        return default_value;
    }

    DWORD dataSize = 0;
    lResult = RegQueryValueEx(hKey, key.c_str(), NULL, NULL, NULL, &dataSize);
    if (lResult != ERROR_SUCCESS)
    {
        std::cout << "Couldn't read registry key size - returning default value" << std::endl;
        RegCloseKey(hKey);
        return default_value;
    }

    std::string dataBuffer(dataSize / sizeof(char8_t), '\0');
    lResult = RegQueryValueEx(hKey, key.c_str(), NULL, NULL, reinterpret_cast<LPBYTE>(&dataBuffer[0]), &dataSize);
    if (lResult == ERROR_SUCCESS)
    {
        // Remove the trailing null character
        value = std::string(dataBuffer.c_str());
    }
    else
    {
        std::cout << "Couldn't read registry key - returning default value" << std::endl;
        value = default_value;
    }

    RegCloseKey(hKey);

    return value;
}

bool set_config_value(std::string key, std::string value)
{
    std::string subKey = "Software\\ObjectiveFrame";

    HKEY hKey;
    LONG lResult = RegCreateKeyEx(HKEY_CURRENT_USER, subKey.c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL,
                                  &hKey, NULL);

    if (lResult != ERROR_SUCCESS)
    {
        return false;
    }

    lResult =
        RegSetValueEx(hKey, key.c_str(), 0, REG_SZ, (const BYTE *)value.c_str(), (value.size() + 1) * sizeof(char8_t));
    RegCloseKey(hKey);

    return lResult == ERROR_SUCCESS;
}

void flipImageVertically(GLubyte *data, unsigned width, unsigned height)
{
    size_t rowSize = width * 3; // RGB
    GLubyte *rowBuffer = (GLubyte *)malloc(rowSize);
    for (int i = 0; i < height / 2; ++i)
    {
        memcpy(rowBuffer, data + i * rowSize, rowSize);
        memcpy(data + i * rowSize, data + (height - i - 1) * rowSize, rowSize);
        memcpy(data + (height - i - 1) * rowSize, rowBuffer, rowSize);
    }
    free(rowBuffer);
}

void saveImage(std::string &filename, GLubyte *data, int width, int height)
{
    flipImageVertically(data, width, height);
    stbi_write_png(filename.c_str(), width, height, 3, data, width * 3);
}

void rand_seed()
{
    srand(time(NULL));
}

double rand_float(double min, double max)
{
    return min + (max - min) * (rand() / (RAND_MAX + 1.0));
}

int rand_int(int min, int max)
{
    return min + (rand() % (max - min + 1));
}

} // namespace ofutil
