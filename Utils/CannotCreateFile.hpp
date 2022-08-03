//
// Created by chou on 22-8-3.
//

#ifndef XSCRIPT2PACKAGEMANAGER_CANNOTCREATEFILE_HPP
#define XSCRIPT2PACKAGEMANAGER_CANNOTCREATEFILE_HPP


#include "XpmException.hpp"

class CannotCreateFile : XPMException {
public:
    CannotCreateFile();

    const char * what() const noexcept override;
};


#endif //XSCRIPT2PACKAGEMANAGER_CANNOTCREATEFILE_HPP
