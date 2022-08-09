#include <iostream>
#include "Utils/Utils.hpp"
#include "Utils/XpmException.hpp"
#include "Network/Mirror.hpp"
#include "XScript2/Share/Exceptions/CompilerError.hpp"
#include "XScript2/Share/Exceptions/HeapOverflowException.hpp"
#include "XScript2/Share/Exceptions/InternalException.hpp"
#include "XScript2/Share/Exceptions/BytecodeInterpretError.hpp"
#include "XScript2/Share/Exceptions/ParserException.hpp"

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
    try {
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
                for (auto Iter = Utils::PMConfig["Dependencies"].begin();
                     Iter != Utils::PMConfig["Dependencies"].end(); Iter++) {
                    if ((*Iter)["Name"] == Arguments["name"]) {
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
            Mirror M(Utils::PMConfig["Mirror"]);
            M.Uninstall(Arguments["name"]);
            Utils::StorePMConfigFile();

            Utils::PMConfig = {};
            if (!Arguments.count("global")) {
                Utils::IsGlobal = false;
                Utils::GetPMConfigFile();
                for (auto Iter = Utils::PMConfig["Dependencies"].begin();
                     Iter != Utils::PMConfig["Dependencies"].end(); Iter++) {
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
        } else if (Arguments.count("init")) {
            Utils::Initialize();
        } else if (Arguments.count("version")) {
            std::cout << "XScript2 Package Manager\n";
            std::cout << "Package Manager version: " << wstring2string(Utils::CommitVersion) << "\n";
            std::cout << "XScript2 Core version: " << wstring2string(CommitVersion) << "\n";
            std::cout << "Developed by Jerry Chou(Winghong Zau).\n" << std::flush;
        }
    } catch (const XScript::CompilerError &E) {
        std::cout << E.what() << std::endl;
        return 1;
    } catch (const XScript::HeapOverflowException &E) {
        std::cout << E.what() << std::endl;
        return 1;
    } catch (const XScript::InternalException &E) {
        std::cout << E.what() << std::endl;
        return 1;
    }  catch (const XScript::BytecodeInterpretError &E) {
        std::cout << E.what() << std::endl;
        return 1;
    }  catch (const XScript::ParserException &E) {
        std::cout << E.what() << std::endl;
        return 1;
    }
    return 0;
}
