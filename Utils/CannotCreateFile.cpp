//
// Created by chou on 22-8-3.
//

#include "CannotCreateFile.hpp"

CannotCreateFile::CannotCreateFile() : XPMException("Cannot create file") {

}

const char *CannotCreateFile::what() const noexcept {
    return XPMException::what();
}
