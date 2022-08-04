//
// Created by chou on 22-8-1.
//

#ifndef XSCRIPT2PACKAGEMANAGER_UTILS_HPP
#define XSCRIPT2PACKAGEMANAGER_UTILS_HPP

#define XPMBuildNumber 0

#include <string>

#include "../XScript2/Share/Utils.hpp"

using namespace XScript;

#include "json.hpp"
#include "../XScript2/Backend/CompilerEnvironment.hpp"

using JSON = nlohmann::json;

#if defined(__linux__)
#define OperatingSystem "linux"
#elif defined(__APPLE__)
#define OperatingSystem "darwin"
#elif defined(_WIN32)
#define OperatingSystem "win32"
#else
#define OperatingSystem "unknown"
#endif

#if defined(__aarch64__)
#define Architecture "arm64"
#elif defined(__x86_64__)
#define Architecture "amd64"
#elif defined(__i386__)
#define Architecture "i386"
#elif defined(__arm__)
#define Architecture "arm"
#endif

namespace Utils {
    constexpr const char *PMConfigFileName = "PackageManagerConfig.json";

    extern JSON PMConfig;

    extern bool IsGlobal;

    extern std::string DefaultMainFile;

    extern XBytes LastProgressContent;

    std::string GetPackageManagerDir();

    JSON &GetPMConfigFile();

    void StorePMConfigFile();

    void MakeDirectoryForPackage(const XBytes &Name);

    std::filesystem::path GetPackageDir(const XBytes &Name);

    void PrintProgress(const XBytes &File, XInteger Progress);

    void AddPackageInformation(const XBytes &Name, const XBytes &Version);

    void RemovePackage(const XBytes &Name);

    void MakeProject(const XBytes &NameOfTheProject);

    void AddDirectoryToTarget(XScript::Compiler::CompilerEnvironment &Env, const std::string &Path);

    void BuildProject();

    void RunProject(const std::string &PkgFileName);

    void RunPackage(const std::string &Package, const std::string &PkgFileName);

    void PackBuildDir(const std::string &PackageName);

    void Initialize();
}

#endif //XSCRIPT2PACKAGEMANAGER_UTILS_HPP
