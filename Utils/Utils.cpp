//
// Created by chou on 22-8-1.
//

#include "Utils.hpp"
#include "PackageAlreadyExist.hpp"
#include "PackageNotExist.hpp"
#include "../XScript2/Backend/CompilerEnvironment.hpp"
#include "../XScript2/Core/CompilerCore.hpp"

#include <unistd.h>
#include <fstream>
#include <iostream>

#if defined(_WIN32)

#include <direct.h>
#include <windows.h>
#define mkdir _mkdir

#elif defined(__APPLE__)

#import <Foundation/Foundation.h>
#include <mach-o/dyld.h>

#endif

namespace Utils {
    JSON PMConfig;

    bool IsGlobal;

    std::string DefaultMainFile =
            "def main () {\n"
            "    // Now you can try to do something interesting here.\n"
            "    // use xpm install name=XScript2Stdlib ver=latest to get the stdlib. : -)\n"
            "    // Remove these lines inside the function before you coding your program\n"
            "    return 0;\n"
            "}\n";

    XBytes LastProgressContent;
#if defined(_WIN32)
    std::string Utils::GetPackageManagerDir() {
        char FilePath[2048] = {0};
        ::GetModuleFileNameA(NULL, FilePath, 2048);

        std::string Result;
        Result.append(FilePath);
        int DelimPos = Result.rfind('\\');

        if (std::string::npos == DelimPos) {
            Result = "";
        } else {
            Result = Result.substr(0, DelimPos);
        }

        return Result;
    }
#elif defined(__APPLE__)
    std::string Utils::GetPackageManagerDir() {
        char buf[0];
        uint32_t size = 0;
        int res = _NSGetExecutablePath(buf,&size);

        char* path = new char[size+1];
        path[size] = 0;
        res = _NSGetExecutablePath(path,&size);

        char* p = strrchr(path, '/');
        *p = 0;
        std::string pathTemp;
        pathTemp.append(path);
        delete[] path;
        return pathTemp;
    }
#else

    std::string GetPackageManagerDir() {
        std::string Path;
        Path.resize(2048);

        ssize_t sz = readlink("/proc/self/exe", Path.data(), 2048);
        if (sz == -1)
            return "";
        Path = Path.substr(0, Path.rfind('/'));
        return Path;
    }

#endif

    JSON &GetPMConfigFile() {
        if (PMConfig == (JSON) {}) {
            if (IsGlobal) {
                return PMConfig = JSON::parse(std::ifstream(GetPackageManagerDir() + "/" + PMConfigFileName));
            } else {
                return PMConfig = JSON::parse(std::ifstream((std::string) ("./") + PMConfigFileName));
            }
        } else {
            return PMConfig;
        }
    }

    void StorePMConfigFile() {
        std::ofstream Out(GetPackageManagerDir() + "/" + PMConfigFileName);
        Out << std::setw(4) << PMConfig;
        Out.close();
    }

    void MakeDirectoryForPackage(const XBytes &Name) {
        if (IsGlobal) {
            std::filesystem::create_directories(GetPackageManagerDir() + "/InstalledPackages/" + Name);
            std::filesystem::create_directory(
                    GetPackageManagerDir() + "/InstalledPackages/" + Name + "/NativeLibraries");
        } else {
            std::filesystem::create_directories((std::string) ("./InstalledPackages/") + Name);
            std::filesystem::create_directory((std::string) ("./InstalledPackages/") + Name + "/NativeLibraries");
        }
    }

    XBytes GetPackageDir(const XBytes &Name) {
        if (IsGlobal) {
            return GetPackageManagerDir() + "/InstalledPackages/" + Name;
        } else {
            return (std::string) ("./InstalledPackages/") + Name;
        }
    }

    void PrintProgress(const XBytes &File, XInteger Progress) {
        std::string s(LastProgressContent.length(), '\b');
        printf("%s", s.c_str());
        LastProgressContent = "Downloading " + File + " CurrentLength: " + std::to_string(Progress);
        printf("%s", LastProgressContent.c_str());
    }

    void AddPackageInformation(const XBytes &Name, const XBytes &Version) {
        GetPMConfigFile();
        if (PMConfig["InstalledPackages"].count(Name)) {
            if (Version == PMConfig["InstalledPackages"][Name])
                return;
            throw PackageAlreadyExist(Name, nlohmann::to_string(PMConfig["InstalledPackages"][Name]));
        } else {
            PMConfig["InstalledPackages"][Name] = Version;
        }
    }

    void RemovePackage(const XBytes &Name) {
        if (std::filesystem::exists(GetPackageDir(Name))) {
            std::filesystem::remove(GetPackageDir(Name));
            PMConfig["InstalledPackages"].erase(Name);
        } else {
            throw PackageNotExist(Name);
        }
    }

    void MakeProject(const XBytes &NameOfTheProject) {
        std::filesystem::create_directories("./" + NameOfTheProject + "/XPMBuildDir");
        std::filesystem::create_directories("./" + NameOfTheProject + "/Sources/Main");
        JSON NewConfigFile;
        NewConfigFile["XPMVersion"] = XPMBuildNumber;
        NewConfigFile["Dependencies"] = (XMap<XBytes, XBytes>) {};
        NewConfigFile["InstalledPackages"] = (XMap<XBytes, XBytes>) {};
        NewConfigFile["Type"] = "Project";
        NewConfigFile["Name"] = NameOfTheProject;
        NewConfigFile["Description"] = "";
        NewConfigFile["Author"] = "";
        NewConfigFile["Repository"] = "";
        // when the compile process start, it will scan all files with extension ".xs", and add it to a list, compiler will compile them as one package.
        // the left side of each pair is package name, the right side is SourceDir of the package.
        NewConfigFile["SourcesDir"] = R"(
              [{"Name": "Main", "IsExecutable": true, "Path": "Sources/Main"}]
            )"_json;
        // the output folder of compiler
        NewConfigFile["BuildDir"] = "XPMBuildDir";
        std::ofstream Stream((std::string) ("./") + NameOfTheProject + "/" + PMConfigFileName);
        Stream << std::setw(4) << NewConfigFile;
        Stream.close();

        Stream = (std::ofstream) {(std::string) ("./") + NameOfTheProject + "/Sources/Main/Main.xs"};
        Stream << DefaultMainFile;
        Stream.close();
    }

    void BuildProject() {
        for (auto &I: PMConfig["SourcesDir"]) {
            XScript::Compiler::CompilerEnvironment Environ{};
            if (I["IsExecutable"]) {
                Environ.CompilerFlags.push_back(L"compile_as_executable.true");
            }
            Environ.PathsToSearch.push_back(L"./InstalledPackages");
            Environ.PathsToSearch.push_back(string2wstring(GetPackageManagerDir() + "/InstalledPackages"));
            AddDirectoryToTarget(Environ, "./" + I["Path"].get<std::string>());
            XScript::OutputBinary(
                    Environ,
                    string2wstring(
                            "./" + PMConfig["BuildDir"].get<std::string>() + "/" + I["Name"].get<std::string>()));
        }
    }

    void AddDirectoryToTarget(XScript::Compiler::CompilerEnvironment &Env, const std::string &Path) {
        for (auto &I: std::filesystem::directory_iterator(Path)) {
            if (I.is_directory()) {
                AddDirectoryToTarget(Env, I.path());
            } else {
                if (I.path().extension().string() == ".xs") {
                    printf("Compiling file: %s...\n", I.path().string().c_str());
                    XScript::CompileForFile(Env, string2wstring(I.path().string().c_str()));
                }
            }
        }
    }
}