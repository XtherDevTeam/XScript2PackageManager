//
// Created by chou on 22-8-1.
//

#include "Mirror.hpp"
#include "../Utils/PackageNotExist.hpp"
#include "../Utils/PackageVersionNotExist.hpp"
#include "../Utils/CannotCreateFile.hpp"

Mirror::Mirror(const XBytes &MirrorUrl) : MirrorUrl(MirrorUrl), Client(MirrorUrl) {
    if (Utils::PMConfig == (JSON) {}) {
        Utils::GetPMConfigFile();
    }
}

JSON Mirror::Query(const XBytes &PackageName) {
    auto InformationResp = Client.Get("/packages/" + PackageName + "/info.json");
    if (InformationResp->status == 404) {
        throw PackageNotExist(PackageName);
    }
    return JSON::parse(InformationResp.value().body);
}

void Mirror::Download(const XBytes &PackageName, const XBytes &Version) {
    JSON PackageInfo = Query(PackageName);
    if (Utils::PMConfig["InstalledPackages"].count(PackageName)) {
        std::cout << "Version " << Utils::PMConfig["InstalledPackages"][PackageName]["InstalledVersion"] << " of package " << PackageName << " already installed." << std::endl;
    } else {
        if (PackageInfo["Versions"].count(Version)) {
            for (auto &I : PackageInfo["Versions"][Version]["Dependencies"]) {
                Download(I["Name"], I["Version"]);
            }
            Utils::MakeDirectoryForPackage(PackageName);
            for (auto &File: PackageInfo["Version"][Version]["PackageFiles"]) {
                XBytes Path = Utils::GetPackageDir(PackageName) + "/" + nlohmann::to_string(File["Name"]);
                FILE *FilePointer = fopen(Path.c_str(), "wb+");
                if (FilePointer) {
                    XInteger Len = 0;
                    auto Result = Client.Get(File["Url"], [&](const char *data, size_t data_length) {
                        Len += static_cast<XInteger>(data_length);
                        Utils::PrintProgress(File["Name"], Len);
                        fwrite(data, data_length, 1, FilePointer);
                        return true;
                    });
                } else {
                    throw CannotCreateFile();
                }
            }
            for (auto &File: PackageInfo["Version"][Version]["NativeLibrariesFiles"]) {
                XBytes Path =
                        Utils::GetPackageDir(PackageName) + "/NativeLibraries/" + nlohmann::to_string(File["Name"]);
                FILE *FilePointer = fopen(Path.c_str(), "wb+");
                if (FilePointer) {
                    XInteger Len = 0;
                    auto Result = Client.Get(File["Url"], [&](const char *data, size_t data_length) {
                        Len += static_cast<XInteger>(data_length);
                        Utils::PrintProgress(File["Name"], Len);
                        fwrite(data, data_length, 1, FilePointer);
                        return true;
                    });
                } else {
                    throw CannotCreateFile();
                }
            }
        } else {
            throw PackageVersionNotExist(PackageName, Version);
        }
        Utils::AddPackageInformation(PackageName, Version);
    }
}
