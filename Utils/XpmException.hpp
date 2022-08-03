//
// Created by chou on 22-8-1.
//

#ifndef XSCRIPT2PACKAGEMANAGER_XPMEXCEPTION_HPP
#define XSCRIPT2PACKAGEMANAGER_XPMEXCEPTION_HPP


#include <exception>
#include <string>

class XPMException : std::exception {
    std::string Message;
public:
    XPMException(const std::string &Str);

    const char * what() const noexcept override;
};


#endif //XSCRIPT2PACKAGEMANAGER_XPMEXCEPTION_HPP
