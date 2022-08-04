#include <iostream>
#include "Utils/Utils.hpp"
#include "Utils/XpmException.hpp"
#include "Network/Mirror.hpp"

XMap<XBytes, XBytes> Arguments;

void ParseArguments(int argc, const char **argv) {
    /* defaults */
    Arguments["PATH"] = ".;";
    for (int i = 1; i < argc; i++) {
        XBytes Str = argv[i];
        XIndexType Equ = Str.find('=');
        if (Equ != XBytes::npos) {
            XBytes L = Str.substr(0, Equ);
            XBytes R = Str.substr(Equ + 1);
            if (L[0] == '\'' || L[0] == '"') {
                L = L.substr(1, L.size() - 2);
            }
            if (R[0] == '\'' || R[0] == '"') {
                R = R.substr(1, R.size() - 2);
            }
            Arguments[L] = R;
        } else {
            Arguments[Str] = {};
        }
    }
}

int main(int argc, const char **argv) {
    ParseArguments(argc, argv);
    if (Arguments.count("switch-mirror")) {
        Utils::IsGlobal = true;
        Utils::GetPMConfigFile();
        Utils::PMConfig["Mirror"] = Arguments["host"];
        Utils::StorePMConfigFile();
    } else if (Arguments.count("install")) {
        Utils::IsGlobal = true;
        Utils::GetPMConfigFile();
        Mirror M(Utils::PMConfig["Mirror"]);
        M.Download(Arguments["name"], Arguments["ver"]);
        Utils::StorePMConfigFile();

        Utils::PMConfig = {};
        if (!Arguments.count("global")) {
            Utils::IsGlobal = false;
            Utils::GetPMConfigFile();
            bool IsNotExist = true;
            for (auto Iter = Utils::PMConfig["Dependencies"].begin(); Iter != Utils::PMConfig["Dependencies"].end(); Iter++) {
                if ((*Iter)["Name"] ==     Arguments["name"]) {
                    IsNotExist = false;
                    break;
                }
            }
            if (IsNotExist) {
                Utils::PMConfig["Dependencies"].push_back(
                        (std::map<std::string, std::string>) {
                                {"Name",    Arguments["name"]},
                                {"Version", Arguments["ver"]}
                        });
                Utils::StorePMConfigFile();
            }
        }
    } else if (Arguments.count("uninstall")) {
        Utils::IsGlobal = true;
        Utils::GetPMConfigFile();
        Utils::RemovePackage(Arguments["name"]);
        Utils::StorePMConfigFile();

        Utils::PMConfig = {};
        if (!Arguments.count("global")) {
            Utils::IsGlobal = false;
            Utils::GetPMConfigFile();
            for (auto Iter = Utils::PMConfig["Dependencies"].begin(); Iter != Utils::PMConfig["Dependencies"].end(); Iter++) {
                if ((*Iter)["Name"] == Arguments["name"]) {
                    Utils::PMConfig["Dependencies"].erase(Iter);
                    break;
                }
            }
            Utils::StorePMConfigFile();
        }
    } else if (Arguments.count("create-project")) {
        Utils::MakeProject(Arguments["name"]);
    } else if (Arguments.count("build-project")) {
        Utils::GetPMConfigFile();
        Utils::BuildProject();
        Utils::StorePMConfigFile();
    } else if (Arguments.count("run-project")) {
        Utils::GetPMConfigFile();
        Utils::RunProject(Arguments["exec"]);
        Utils::StorePMConfigFile();
    } else if (Arguments.count("pack-build-dir")) {
        Utils::GetPMConfigFile();
        Utils::PackBuildDir(Utils::PMConfig["Name"]);
        Utils::StorePMConfigFile();
    } else if (Arguments.count("run-package")) {
        Utils::RunPackage(Arguments["package"], Arguments["exec"]);
    }

    return 0;
}
