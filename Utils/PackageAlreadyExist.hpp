//
// Created by chou on 22-8-3.
//

#ifndef XSCRIPT2PACKAGEMANAGER_PACKAGEALREADYEXIST_HPP
#define XSCRIPT2PACKAGEMANAGER_PACKAGEALREADYEXIST_HPP


#include "XpmException.hpp"

class PackageAlreadyExist : XPMException {
public:
    PackageAlreadyExist(const std::string &N, const std::string &V);
};


#endif //XSCRIPT2PACKAGEMANAGER_PACKAGEALREADYEXIST_HPP
