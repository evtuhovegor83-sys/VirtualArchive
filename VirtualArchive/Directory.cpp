#include "Directory.h"
#include "exceptions.h"
#include <iostream>
#include <algorithm>
#include <regex>

Directory::Directory(const std::string& name, AccessLevel level)
    : Resource(name), accessLevel(level) {

    if (!validateName(name)) {
        throw InvalidNameException(name);
    }
}

void Directory::addChild(std::unique_ptr<Resource> child) {
    if (findChild(child->getName()) != nullptr) {
        throw ArchiveException("Resource already exists: " + child->getName());
    }
    children.push_back(std::move(child));
}

std::unique_ptr<Resource> Directory::removeChild(const std::string& name) {
    auto it = std::find_if(children.begin(), children.end(),
        [&name](const std::unique_ptr<Resource>& child) {
            return child->getName() == name;
        });

    if (it == children.end()) {
        throw NotFoundException(name);
    }

    std::unique_ptr<Resource> result = std::move(*it);
    children.erase(it);
    return result;
}

Resource* Directory::findChild(const std::string& name) const {
    auto it = std::find_if(children.begin(), children.end(),
        [&name](const std::unique_ptr<Resource>& child) {
            return child->getName() == name;
        });

    if (it != children.end()) {
        return it->get();
    }
    return nullptr;
}

AccessLevel Directory::getAccessLevel() const {
    return accessLevel;
}

void Directory::setAccessLevel(AccessLevel level) {
    accessLevel = level;
}

long long Directory::getSize() const {
    long long total = 0;
    for (const auto& child : children) {
        total += child->getSize();
    }
    return total;
}

void Directory::print(int depth) const {
    std::string levelStr;
    switch (accessLevel) {
    case AccessLevel::GUEST: levelStr = "[GUEST]"; break;
    case AccessLevel::USER: levelStr = "[USER]"; break;
    case AccessLevel::ADMIN: levelStr = "[ADMIN]"; break;
    }

    std::cout << std::string(depth * 2, ' ')
        << "📁 " << name << " " << levelStr
        << " [created: " << creationDate.toString() << "]\n";

    for (const auto& child : children) {
        child->print(depth + 1);
    }
}

std::unique_ptr<Resource> Directory::clone() const {
    auto newDir = std::make_unique<Directory>(name, accessLevel);
    for (const auto& child : children) {
        newDir->addChild(child->clone());
    }
    return newDir;
}

const std::vector<std::unique_ptr<Resource>>& Directory::getChildren() const {
    return children;
}

bool Directory::validateName(const std::string& name) {
    std::regex pattern(R"(^[a-zA-Z0-9_\-]+$)");
    return std::regex_match(name, pattern);
}

// НОВЫЙ МЕТОД: поиск по маске
std::vector<Resource*> Directory::searchByMask(const std::string& mask) const {
    std::vector<Resource*> results;

    // Поиск в текущей папке
    for (const auto& child : children) {
        std::string name = child->getName();
        if (name.find(mask) != std::string::npos) {
            results.push_back(child.get());
        }
    }

    // Рекурсивный поиск во вложенных папках
    for (const auto& child : children) {
        if (auto* subdir = dynamic_cast<Directory*>(child.get())) {
            auto subResults = subdir->searchByMask(mask);
            results.insert(results.end(), subResults.begin(), subResults.end());
        }
    }

    return results;
}

std::vector<Resource*> Directory::filterByDate(const Date& start, const Date& end) const {
    std::vector<Resource*> results;

    // Проверяем текущий ресурс
    if (creationDate >= start && creationDate <= end) {
        results.push_back(const_cast<Directory*>(this));
    }

    // Проверяем детей
    for (const auto& child : children) {
        Date childDate = child->getCreationDate();
        if (childDate >= start && childDate <= end) {
            results.push_back(child.get());
        }

        // Рекурсивно ищем во вложенных папках
        if (auto* subdir = dynamic_cast<Directory*>(child.get())) {
            auto subResults = subdir->filterByDate(start, end);
            results.insert(results.end(), subResults.begin(), subResults.end());
        }
    }

    return results;
}