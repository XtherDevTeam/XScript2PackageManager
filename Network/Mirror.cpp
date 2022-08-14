//
// Created by chou on 22-8-1.
//

#include "Mirror.hpp"
#include "../Utils/PackageNotExist.hpp"
#include "../Utils/PackageVersionNotExist.hpp"
#include "../Utils/CannotCreateFile.hpp"
#include "../Utils/XArchive/Sources/ArchiveFormat.hpp"

Mirror::Mirror(const XBytes &MirrorUrl) : MirrorUrl(MirrorUrl) {
    if (Utils::PMConfig == (JSON) {}) {
        Utils::GetPMConfigFile();
    }
}

JSON Mirror::Query(const XBytes &PackageName, const XBytes &OS, const XBytes &Arch) {
    httplib::Client Client(MirrorUrl);
    if (auto InformationResp = Client.Get(
            "/xpm-mirror/packages/" + PackageName + "/info_" + OS + "_" + Arch + ".json")) {
        if (InformationResp->status == 404) {
            throw PackageNotExist(PackageName);
        }
        return JSON::parse(InformationResp.value().body);
    } else {
        auto err = InformationResp.error();
        std::cout << "HTTP error: " << httplib::to_string(err) << std::endl;
        throw PackageNotExist(PackageName);
    }
}

void Mirror::Download(const XBytes &PackageName, const XBytes &Version) {
    httplib::Client Client(MirrorUrl);
    JSON PackageInfo = Query(PackageName, OperatingSystem, Architecture);
    if (Utils::PMConfig["InstalledPackages"].count(PackageName)) {
        std::cout << "Version " << Utils::PMConfig["InstalledPackages"][PackageName]["Version"].get<std::string>()
                  << " of package " << PackageName << " already installed." << std::endl;
    } else {
        if (PackageInfo["Versions"].count(Version)) {
            for (auto &I: PackageInfo["Versions"][Version]["Dependencies"]) {
                Download(I["Name"], I["Version"]);
                Utils::PMConfig["InstalledPackages"][I["Name"].get<std::string>()]["ReferenceCount"] =
                        Utils::PMConfig["InstalledPackages"][I["Name"].get<std::string>()]["ReferenceCount"].get<XInteger>() + 1;
            }
            XArchive::ArchiveFormat Format{};

            Utils::MakeDirectoryForPackage(PackageName);
            if (!PackageInfo["Versions"][Version]["BytecodePackage"].get<std::string>().empty()) {
                XIndexType Step = 0;
                FILE *FilePointer = fopen((std::filesystem::temp_directory_path() / "temp.xar").string().c_str(),
                                          "wb+");
                if (FilePointer) {
                    XInteger Len = 0;
                    auto Result = Client.Get(
                            "/xpm-mirror" + PackageInfo["Versions"][Version]["BytecodePackage"].get<std::string>(),
                            [&](const char *data, size_t data_length) {
                                Len += static_cast<XInteger>(data_length);
                                if (!(Step % 10000))
                                    Utils::PrintProgress(PackageInfo["Versions"][Version]["BytecodePackage"], Len);
                                fwrite(data, data_length, 1, FilePointer);
                                return true;
                            });
                    fclose(FilePointer);
                    Format.DecompressToDirectory(std::filesystem::temp_directory_path() / "temp.xar",
                                                 Utils::GetPackageDir(PackageName));
                    std::cout << "\nBytecode packages installed.\n";
                } else {
                    throw CannotCreateFile();
                }
            }

            if (!PackageInfo["Versions"][Version]["NativeLibrariesPackage"].get<std::string>().empty()) {
                XIndexType Step = 0;
                FILE *FilePointer = fopen((std::filesystem::temp_directory_path() / "temp.xar").string().c_str(),
                                          "wb+");
                if (FilePointer) {
                    std::cout << "/xpm-mirror" +
                                 PackageInfo["Versions"][Version]["NativeLibrariesPackage"].get<std::string>() << std::endl << std::flush;
                    XInteger Len = 0;
                    auto Result = Client.Get(
                            "/xpm-mirror" +
                            PackageInfo["Versions"][Version]["NativeLibrariesPackage"].get<std::string>(),
                            [&](const char *data, size_t data_length) {
                                Len += static_cast<XInteger>(data_length);
                                if (!(Step % 10000))
                                    Utils::PrintProgress(PackageInfo["Versions"][Version]["NativeLibrariesPackage"], Len);
                                fwrite(data, data_length, 1, FilePointer);
                                return true;
                            });
                    fclose(FilePointer);
                    std::cout << "\nNative library packages installed.\n";
                    Format.DecompressToDirectory(std::filesystem::temp_directory_path() / "temp.xar",
                                                 Utils::GetPackageDir(PackageName) / "NativeLibraries");
                } else {
                    throw CannotCreateFile();
                }
                std::filesystem::remove(std::filesystem::temp_directory_path() / "temp.xar");
            }
        } else {
            throw PackageVersionNotExist(PackageName, Version);
        }
        Utils::AddPackageInformation(PackageName, Version);
    }
}

void Mirror::Uninstall(const XBytes &Package) {
    if (Utils::PMConfig["InstalledPackages"].count(Package)) {
        auto Version = Utils::PMConfig["InstalledPackages"][Package]["Version"].get<std::string>();

        JSON PackageInfo = Query(Package, OperatingSystem, Architecture);
        for (auto &I: PackageInfo["Versions"][Version]["Dependencies"]) {
            Uninstall(I["Name"].get<std::string>());
        }
        Utils::RemovePackage(Package);
    } else {
        throw PackageNotExist(Package);
    }
}
