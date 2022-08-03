//
// Created by chou on 22-8-1.
//

#include "XpmException.hpp"

XPMException::XPMException(const std::string &Str) : Message(Str) {

}

const char *XPMException::what() const noexcept {
    return Message.c_str();
}
