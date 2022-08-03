//
// Created by chou on 22-8-3.
//

#ifndef XSCRIPT2PACKAGEMANAGER_PACKAGEVERSIONNOTEXIST_HPP
#define XSCRIPT2PACKAGEMANAGER_PACKAGEVERSIONNOTEXIST_HPP


#include "XpmException.hpp"

class PackageVersionNotExist : XPMException {
public:
    PackageVersionNotExist(const std::string &N, const std::string &V);

    const char * what() const noexcept override;
};


#endif //XSCRIPT2PACKAGEMANAGER_PACKAGEVERSIONNOTEXIST_HPP
