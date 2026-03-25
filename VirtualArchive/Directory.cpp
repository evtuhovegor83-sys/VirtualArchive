#include "Directory.h"
#include "File.h"
#include "exceptions.h"
#include "Logger.h"
#include <iostream>
#include <algorithm>
#include <regex>
#include <fstream>
#include <functional>

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

// Поиск по маске
std::vector<Resource*> Directory::searchByMask(const std::string& mask) const {
    std::vector<Resource*> results;

    for (const auto& child : children) {
        std::string name = child->getName();
        if (name.find(mask) != std::string::npos) {
            results.push_back(child.get());
        }
    }

    for (const auto& child : children) {
        if (auto* subdir = dynamic_cast<Directory*>(child.get())) {
            auto subResults = subdir->searchByMask(mask);
            results.insert(results.end(), subResults.begin(), subResults.end());
        }
    }

    return results;
}

// Фильтрация по дате
std::vector<Resource*> Directory::filterByDate(const Date& start, const Date& end) const {
    std::vector<Resource*> results;

    if (creationDate >= start && creationDate <= end) {
        results.push_back(const_cast<Directory*>(this));
    }

    for (const auto& child : children) {
        Date childDate = child->getCreationDate();
        if (childDate >= start && childDate <= end) {
            results.push_back(child.get());
        }

        if (auto* subdir = dynamic_cast<Directory*>(child.get())) {
            auto subResults = subdir->filterByDate(start, end);
            results.insert(results.end(), subResults.begin(), subResults.end());
        }
    }

    return results;
}

// Перемещение ресурса
void Directory::moveChild(const std::string& name, Directory* newParent) {
    auto child = removeChild(name);
    if (newParent == nullptr) {
        throw ArchiveException("Invalid target directory");
    }
    newParent->addChild(std::move(child));
    Logger::getInstance()->info("Moved " + name + " to new parent");
}

// Сортировка детей
void Directory::sortChildren(SortBy sortBy) {
    switch (sortBy) {
    case SortBy::NAME:
        std::sort(children.begin(), children.end(),
            [](const std::unique_ptr<Resource>& a, const std::unique_ptr<Resource>& b) {
                return a->getName() < b->getName();
            });
        break;
    case SortBy::SIZE:
        std::sort(children.begin(), children.end(),
            [](const std::unique_ptr<Resource>& a, const std::unique_ptr<Resource>& b) {
                return a->getSize() < b->getSize();
            });
        break;
    case SortBy::DATE:
        std::sort(children.begin(), children.end(),
            [](const std::unique_ptr<Resource>& a, const std::unique_ptr<Resource>& b) {
                return a->getCreationDate() < b->getCreationDate();
            });
        break;
    }
    Logger::getInstance()->info("Directory sorted");
}

// Экспорт в CSV
void Directory::exportToCSV(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw ArchiveException("Cannot open file for CSV export: " + filename);
    }

    file << "Type,Name,Size(Bytes),CreationDate\n";

    std::function<void(const Directory*)> exportRecursive = [&](const Directory* dir) {
        for (const auto& child : dir->getChildren()) {
            if (auto* fileObj = dynamic_cast<File*>(child.get())) {
                file << "File," << fileObj->getName() << "." << fileObj->getExtension()
                    << "," << fileObj->getSize()
                    << "," << fileObj->getCreationDate().toString() << "\n";
            }
            else if (auto* subdir = dynamic_cast<Directory*>(child.get())) {
                file << "Directory," << subdir->getName()
                    << ",0," << subdir->getCreationDate().toString() << "\n";
                exportRecursive(subdir);
            }
        }
        };

    exportRecursive(this);
    file.close();

    Logger::getInstance()->info("CSV exported to " + filename);
}