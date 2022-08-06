//
// Created by chou on 22-8-1.
//

#ifndef XSCRIPT2PACKAGEMANAGER_MIRROR_HPP
#define XSCRIPT2PACKAGEMANAGER_MIRROR_HPP

#include "../Utils/Utils.hpp"
#include "httplib.hpp"

class Mirror {
    XBytes MirrorUrl;
public:
    Mirror(const XBytes &MirrorUrl);

    JSON Query(const XBytes &PackageName, const XBytes &OS, const XBytes &Arch);

    void Download(const XBytes &PackageName, const XBytes &Version);

    void Uninstall(const XBytes &PackageName);
};


#endif //XSCRIPT2PACKAGEMANAGER_MIRROR_HPP
