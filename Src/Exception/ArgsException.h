#pragma once

#include <exception>
#include <string>

class ArgsException : public std::exception {

public:
    ArgsException(const char* msg) : m_message{ msg } {}

    const char* what() const throw() {
        return m_message.c_str();
    }

private:
    std::string m_message;
};
