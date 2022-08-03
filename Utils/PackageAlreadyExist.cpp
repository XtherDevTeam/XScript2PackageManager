//
// Created by chou on 22-8-3.
//

#include "PackageAlreadyExist.hpp"

PackageAlreadyExist::PackageAlreadyExist(const std::string &N, const std::string &V) : XPMException("Version " + V + " of package " + N + " already exist") {

}
