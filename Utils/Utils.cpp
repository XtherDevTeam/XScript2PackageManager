//
// Created by chou on 22-8-1.
//

#include "Utils.hpp"
#include "PackageAlreadyExist.hpp"
#include "PackageNotExist.hpp"
#include "../XScript2/Backend/CompilerEnvironment.hpp"
#include "../XScript2/Core/CompilerCore.hpp"
#include "../XScript2/Executor/Executor.hpp"
#include "XArchive/Sources/ArchiveFormat.hpp"
#include "whereami/whereami.h"

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

    std::string GetPackageManagerDir() {
        std::string path;
        int length, dirname_length;

        length = wai_getExecutablePath(NULL, 0, &dirname_length);
        path.resize(length + 1);
        wai_getExecutablePath(path.data(), length, &dirname_length);
        path[length] = '\0';
        return path.substr(0, path.rfind('/'));
    }

    JSON &GetPMConfigFile() {
        if (PMConfig == (JSON) {}) {
            if (IsGlobal) {
//                puts(("global: " + GetPackageManagerDir() + "/" + PMConfigFileName).c_str());
                return PMConfig = JSON::parse(std::ifstream(GetPackageManagerDir() + "/" + PMConfigFileName));
            } else {
//                puts(("local: " + (std::string) ("./") + PMConfigFileName).c_str());
                return PMConfig = JSON::parse(std::ifstream((std::string) ("./") + PMConfigFileName));
            }
        } else {
            return PMConfig;
        }
    }

    void StorePMConfigFile() {
        if (IsGlobal) {
            std::ofstream Out(GetPackageManagerDir() + "/" + PMConfigFileName);
            Out << std::setw(4) << PMConfig;
            Out.close();
        } else {
            std::ofstream Out((std::string) ("./") + PMConfigFileName);
            Out << std::setw(4) << PMConfig;
            Out.close();
        }
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

    std::filesystem::path GetPackageDir(const XBytes &Name) {
        return GetPackageManagerDir() + "/InstalledPackages/" + Name;

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
            std::filesystem::remove_all(GetPackageDir(Name));
            PMConfig["InstalledPackages"].erase(Name);
        } else {
            throw PackageNotExist(Name);
        }
    }

    void MakeProject(const XBytes &NameOfTheProject) {
        std::filesystem::create_directories("./" + NameOfTheProject + "/XPMBuildDir/NativeLibraries");
        std::filesystem::create_directories("./" + NameOfTheProject + "/Sources/Main");
        JSON NewConfigFile;
        NewConfigFile["XPMVersion"] = XPMBuildNumber;
        NewConfigFile["Dependencies"] = (XArray<XBytes>) {};
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
            Environ.PathsToSearch.push_back(L"./" + string2wstring(PMConfig["BuildDir"].get<std::string>()));
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

    void RunProject(const std::string &PkgFileName) {
        XScript::Executor VM;
        VM.VM.PathsToSearch.push_back(L"./" + string2wstring(PMConfig["BuildDir"].get<std::string>()));
        VM.VM.PathsToSearch.push_back(string2wstring(GetPackageManagerDir() + "/InstalledPackages"));
        VM.Load(L"./" + string2wstring(PMConfig["BuildDir"].get<std::string>()) + L"/" + string2wstring(PkgFileName));
        VM.Init();
        VM.Start();
    }

    void RunPackage(const std::string &Package, const std::string &PkgFileName) {
        XScript::Executor VM;
        VM.VM.PathsToSearch.push_back(string2wstring(GetPackageDir(Package)));
        VM.VM.PathsToSearch.push_back(string2wstring(GetPackageManagerDir() + "/InstalledPackages"));
        VM.Init();
        VM.Start();
    }

    void PackBuildDir(const std::string &PackageName) {
        std::filesystem::path Path = "./" + PMConfig["BuildDir"].get<std::string>();
        XArchive::ArchiveFormat Format{};
        auto IgnoreFiles = XArchive::ArchiveFormat::MakeIgnoreFileList(Path, {"NativeLibraries"});
        Format.CompressDirectory("", Path, Path / (PackageName + ".bytecode_package.xar"), IgnoreFiles);

        Format.CompressDirectory("", Path / "NativeLibraries",
                                 Path / (PackageName + ".native_" + OperatingSystem + "_" + Architecture + ".xar"), {});
    }

    void Initialize() {
        JSON NewConfigFile;
        NewConfigFile["XPMVersion"] = XPMBuildNumber;
        NewConfigFile["Mirror"] = "http://www.xiaokang00010.top:4002";
        NewConfigFile["InstalledPackages"] = (std::map<XBytes, XBytes>) {};
        std::ofstream Stream(GetPackageManagerDir() + "/" + PMConfigFileName);
        Stream << std::setw(4) << NewConfigFile;
        Stream.close();
    }
};
