#include "Resource.h"

Resource::Resource(const std::string& name) : name(name) {
    creationDate = Date::now();
}

std::string Resource::getName() const {
    return name;
}

Date Resource::getCreationDate() const {
    return creationDate;
}

void Resource::setName(const std::string& newName) {
    name = newName;
}