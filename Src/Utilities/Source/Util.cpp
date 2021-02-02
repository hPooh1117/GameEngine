#include "Util.h"
#include "Log.h"
#include <fstream>
#include <sstream>
#ifdef _WIN32
#include <direct.h>
#endif //WIN32
namespace StringOp
{
    void StringConvert(const std::string& from, std::wstring& to)
    {
#ifdef WIN32
        int num = MultiByteToWideChar(CP_UTF8, 0, from.c_str(), -1, NULL, 0);
        if (num > 0)
        {
            to.resize(size_t(num) - 1);
            MultiByteToWideChar(CP_UTF8, 0, from.c_str(), -1, &to[0], num);
        }
#endif //WIN32
    }

    void StringConvert(const std::wstring& from, std::string& to)
    {
#ifdef WIN32
        int num = WideCharToMultiByte(CP_UTF8, 0, from.c_str(), -1, NULL, 0, NULL, NULL);
        if (num > 0)
        {
            to.resize(size_t(num) - 1);
            WideCharToMultiByte(CP_UTF8, 0, from.c_str(), -1, &to[0], num, NULL, NULL);
        }
#endif //WIN32
    }

    int StringConvert(const char* from, wchar_t* to)
    {
#ifdef WIN32
        int num = MultiByteToWideChar(CP_UTF8, 0, from, -1, NULL, 0);
        if (num > 0)
        {
            MultiByteToWideChar(CP_UTF8, 0, from, -1, &to[0], num);
        }
#endif //WIN32
        return num;
    }

    int StringConvert(const wchar_t* from, char* to)
    {
#ifdef WIN32
        int num = WideCharToMultiByte(CP_UTF8, 0, from, -1, NULL, 0, NULL, NULL);
        if (num > 0)
        {
            WideCharToMultiByte(CP_UTF8, 0, from, -1, &to[0], num, NULL, NULL);
        }
#endif //WIN32
        return num;
    }
}


namespace FileOp
{
    std::string GetApplicationDirectory()
    {
#ifdef WIN32
        static std::string appDir;
        if (appDir.empty())
        {
            CHAR filename[1024] = {};
            GetModuleFileNameA(NULL, filename, ARRAYSIZE(filename));
            appDir = GetDirectoryFromPath(filename);
        }
#endif //WIN32
        return appDir;
    }

    std::string GetOriginalWorkingDirectory()
    {
#ifdef WIN32
        static std::string originalWorkingDir = std::string(_getcwd(NULL, 0)) + "/";
#endif //WIN32
        return originalWorkingDir;
    }

    //static std::string workingDir = GetOriginalWorkingDirectory();
    //std::string GetWorkingDirectory()
    //{
    //    return workingDir;
    //}

    //void SetWorkingDirectory(const std::string& path)
    //{
    //    workingDir = path;
    //}

    void SplitPath(const std::string& fullPath, std::string& dir, std::string& filename)
    {
        size_t found;
        found = fullPath.find_last_of("/\\");
        dir = fullPath.substr(0, found + 1);
        filename = fullPath.substr(found + 1);
    }


    std::string GetFilenameFromPath(const std::string& fullPath)
    {
        if (fullPath.empty())
        {
            return fullPath;
        }

        std::string ret, empty;
        SplitPath(fullPath, empty, ret);
        return ret;
    }

    std::string GetDirectoryFromPath(const std::string& fullPath)
    {
        if (fullPath.empty())
        {
            return fullPath;
        }

        std::string ret, empty;
        SplitPath(fullPath, ret, empty);
        return ret;
    }

    std::string GetExtensionFromFilename(const std::string& filename)
    {
        size_t idx = filename.rfind('.');

        if (idx != std::string::npos)
        {
            std::string extension = filename.substr(idx + 1);
            return extension;
        }
        return "";
    }

    void RemoveExtensionFromFilename(std::string& filename)
    {
        std::string extension = GetExtensionFromFilename(filename);

        if (!extension.empty())
        {
            filename = filename.substr(0, filename.length() - extension.length() - 1);
        }
    }

    //std::string ExpandPath(const std::string& path)
    //{
    //    std::string expanded = path;

    //    // path Ç…ê‚ëŒÉpÉXÇ™ë∂ç›ÇµÇ»Ç¢Ç»ÇÁÇ¬ÇØÇÈÅB
    //    if (expanded.find(":\\") == std::string::npos)
    //    {
    //        expanded = GetWorkingDirectory() + expanded;
    //    }

    //    size_t pos;
    //    while ((pos = expanded.find("..")) != std::string::npos)
    //    {
    //        std::string parent = expanded.substr(0, pos - 1);
    //        size_t pos2 = parent.find_last_of("\\");
    //        parent = parent.substr(0, pos2);
    //        std::string child = expanded.substr(pos + 2);
    //        expanded = parent + child;
    //    }
    //    return expanded;
    //}


    bool FileRead(const std::string& filename, std::vector<uint8_t>& data)
    {
#ifndef PLATFORM_UWP
        std::ifstream file(filename, std::ios::binary | std::ios::ate);
        if (file.is_open())
        {
            size_t dataSize = (size_t)file.tellg();
            file.seekg(0, file.beg);
            data.resize(dataSize);
            file.read((char*)data.data(), dataSize);
            file.close();
            return true;
        }
#endif //PLATFORM_UWP
        Log::Warning("File not found: %s", filename.c_str());
        return false;
    }

    bool FileWrite(const std::string& filename, const uint8_t* data, size_t size)
    {
        if (size <= 0)
        {
            return false;
        }

#ifndef PLATFORM_UWP
        std::ofstream file(filename, std::ios::binary | std::ios::trunc);
        if (file.is_open())
        {
            file.write((const char*)data, (std::streamsize)size);
            file.close();
            return true;
        }
#endif //PLATFORM_UWP
        return false;
    }

    bool FileExists(const std::string& filename)
    {
#ifndef PLATFORM_UWP
        std::ifstream file(filename);
        bool exists = file.is_open();
        file.close();
        return exists;
#endif //PLATFORM_UWP
    }
}