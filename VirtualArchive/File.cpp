#include "File.h"
#include "exceptions.h"
#include <iostream>
#include <regex>
#include <iomanip>

File::File(const std::string& name, const std::string& ext, long long sz)
    : Resource(name), extension(ext), size(sz) {

    if (!validateName(name)) {
        throw InvalidNameException(name);
    }
}

std::string File::getExtension() const {
    return extension;
}

long long File::getSize() const {
    return size;
}

void File::setSize(long long sz) {
    size = sz;
}

void File::print(int depth) const {
    std::cout << std::string(depth * 2, ' ')
        << "📄 " << name << "." << extension
        << " (" << size << " bytes)"
        << " [created: " << creationDate.toString() << "]\n";
}

std::unique_ptr<Resource> File::clone() const {
    return std::make_unique<File>(name, extension, size);
}

bool File::validateName(const std::string& name) {
    std::regex pattern(R"(^[a-zA-Z0-9_\-\.]+$)");
    return std::regex_match(name, pattern);
}