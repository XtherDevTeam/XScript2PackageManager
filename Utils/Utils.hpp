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

namespace Utils {
    constexpr const char* PMConfigFileName = "PackageManagerConfig.json";

    extern JSON PMConfig;

    extern bool IsGlobal;

    extern std::string DefaultMainFile;

    extern XBytes LastProgressContent;

    std::string GetPackageManagerDir();

    JSON &GetPMConfigFile();

    void StorePMConfigFile();

    void MakeDirectoryForPackage(const XBytes &Name);

    XBytes GetPackageDir(const XBytes &Name);

    void PrintProgress(const XBytes &File, XInteger Progress);

    void AddPackageInformation(const XBytes &Name, const XBytes &Version);

    void RemovePackage(const XBytes &Name);

    void MakeProject(const XBytes &NameOfTheProject);

    void AddDirectoryToTarget(XScript::Compiler::CompilerEnvironment &Env, const std::string &Path);

    void BuildProject();
}

#endif //XSCRIPT2PACKAGEMANAGER_UTILS_HPP
