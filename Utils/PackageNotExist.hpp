//
// Created by chou on 22-8-1.
//

#ifndef XSCRIPT2PACKAGEMANAGER_PACKAGENOTEXIST_HPP
#define XSCRIPT2PACKAGEMANAGER_PACKAGENOTEXIST_HPP


#include "XpmException.hpp"

class PackageNotExist : XPMException {
public:
    PackageNotExist();

    PackageNotExist(const std::string &PkgName);
};


#endif //XSCRIPT2PACKAGEMANAGER_PACKAGENOTEXIST_HPP
