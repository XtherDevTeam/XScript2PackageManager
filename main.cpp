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
        if (Arguments.count("global")) {
            Utils::IsGlobal = true;
        }
        Utils::GetPMConfigFile();
        Utils::PMConfig["Mirror"] = Arguments["url"];
    }
    else if (Arguments.count("install")) {
        if (Arguments.count("global")) {
            Utils::IsGlobal = true;
        }
        Utils::GetPMConfigFile();
        Mirror M(Utils::PMConfig["Mirror"]);
        M.Download(Arguments["name"], Arguments["ver"]);
    }
    else if (Arguments.count("uninstall")) {
        if (Arguments.count("global")) {
            Utils::IsGlobal = true;
        }
        Utils::GetPMConfigFile();
        Utils::RemovePackage(Arguments["name"]);
    }
    else if (Arguments.count("create-project")) {
        Utils::MakeProject(Arguments["name"]);
    }
    else if (Arguments.count("build-project")) {
        Utils::GetPMConfigFile();
        Utils::BuildProject();
    }
    return 0;
}
