//
// Created by chou on 22-8-1.
//

#include "PackageNotExist.hpp"

PackageNotExist::PackageNotExist() : XPMException("Cannot find package in network mirror.") {

}

PackageNotExist::PackageNotExist(const std::string &S) : XPMException("Cannot find package " + S + " in network mirror.") {

}
