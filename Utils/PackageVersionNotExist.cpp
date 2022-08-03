//
// Created by chou on 22-8-3.
//

#include "PackageVersionNotExist.hpp"

PackageVersionNotExist::PackageVersionNotExist(const std::string &N, const std::string &V) : XPMException("Version " + V + " of package " + N + "does not exist") {

}

const char *PackageVersionNotExist::what() const noexcept {
    return XPMException::what();
}
