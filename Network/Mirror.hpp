//
// Created by chou on 22-8-1.
//

#ifndef XSCRIPT2PACKAGEMANAGER_MIRROR_HPP
#define XSCRIPT2PACKAGEMANAGER_MIRROR_HPP

#include "../Utils/Utils.hpp"
#include "httplib.hpp"

class Mirror {
    httplib::Client Client;
    XBytes MirrorUrl;
public:
    Mirror(const XBytes &MirrorUrl);

    JSON Query(const XBytes &PackageName);

    void Download(const XBytes &PackageName, const XBytes &Version);
};


#endif //XSCRIPT2PACKAGEMANAGER_MIRROR_HPP
