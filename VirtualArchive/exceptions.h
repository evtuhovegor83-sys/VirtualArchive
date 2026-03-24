#pragma once
#include <exception>
#include <string>

class ArchiveException : public std::exception {
private:
    std::string message;

public:
    ArchiveException(const std::string& msg) : message(msg) {}

    const char* what() const noexcept override {
        return message.c_str();
    }
};

class InvalidNameException : public ArchiveException {
public:
    InvalidNameException(const std::string& name)
        : ArchiveException("Invalid name: " + name) {
    }
};

class NotFoundException : public ArchiveException {
public:
    NotFoundException(const std::string& name)
        : ArchiveException("Resource not found: " + name) {
    }
};

class AccessDeniedException : public ArchiveException {
public:
    AccessDeniedException(const std::string& operation)
        : ArchiveException("Access denied: " + operation) {
    }
};