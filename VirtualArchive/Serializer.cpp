#define _CRT_SECURE_NO_WARNINGS

#include "Serializer.h"
#include "exceptions.h"
#include "Logger.h"
#include <iostream>

void Serializer::writeString(std::ofstream& out, const std::string& str) {
    uint32_t len = static_cast<uint32_t>(str.length());
    out.write(reinterpret_cast<const char*>(&len), sizeof(len));
    out.write(str.c_str(), len);
}

std::string Serializer::readString(std::ifstream& in) {
    uint32_t len;
    in.read(reinterpret_cast<char*>(&len), sizeof(len));
    std::string str(len, '\0');
    in.read(&str[0], len);
    return str;
}

void Serializer::writeDirectory(std::ofstream& out, Directory* dir) {
    uint8_t type = 1;
    out.write(reinterpret_cast<const char*>(&type), sizeof(type));

    writeString(out, dir->getName());

    int32_t accessLevel = static_cast<int32_t>(dir->getAccessLevel());
    out.write(reinterpret_cast<const char*>(&accessLevel), sizeof(accessLevel));

    Date date = dir->getCreationDate();
    out.write(reinterpret_cast<const char*>(&date), sizeof(date));

    const auto& children = dir->getChildren();
    uint32_t childCount = static_cast<uint32_t>(children.size());
    out.write(reinterpret_cast<const char*>(&childCount), sizeof(childCount));

    for (const auto& child : children) {
        if (auto* subdir = dynamic_cast<Directory*>(child.get())) {
            writeDirectory(out, subdir);
        }
        else if (auto* fileObj = dynamic_cast<File*>(child.get())) {
            writeFile(out, fileObj);
        }
    }
}

void Serializer::writeFile(std::ofstream& out, File* fileObj) {
    uint8_t type = 0;
    out.write(reinterpret_cast<const char*>(&type), sizeof(type));

    writeString(out, fileObj->getName());
    writeString(out, fileObj->getExtension());

    long long size = fileObj->getSize();
    out.write(reinterpret_cast<const char*>(&size), sizeof(size));

    Date date = fileObj->getCreationDate();
    out.write(reinterpret_cast<const char*>(&date), sizeof(date));
}

std::unique_ptr<Resource> Serializer::readResource(std::ifstream& in) {
    uint8_t type;
    in.read(reinterpret_cast<char*>(&type), sizeof(type));

    if (type == 0) {
        std::string name = readString(in);
        std::string ext = readString(in);

        long long size;
        in.read(reinterpret_cast<char*>(&size), sizeof(size));

        Date date;
        in.read(reinterpret_cast<char*>(&date), sizeof(date));

        auto fileObj = std::make_unique<File>(name, ext, size);

        return fileObj;

    }
    else if (type == 1) {
        std::string name = readString(in);

        int32_t accessLevel;
        in.read(reinterpret_cast<char*>(&accessLevel), sizeof(accessLevel));

        Date date;
        in.read(reinterpret_cast<char*>(&date), sizeof(date));

        uint32_t childCount;
        in.read(reinterpret_cast<char*>(&childCount), sizeof(childCount));

        auto dir = std::make_unique<Directory>(name, static_cast<AccessLevel>(accessLevel));

        for (uint32_t i = 0; i < childCount; i++) {
            auto child = readResource(in);
            if (child) {
                dir->addChild(std::move(child));
            }
        }

        return dir;
    }

    return nullptr;
}

bool Serializer::checkMagicNumber(std::ifstream& in) {
    uint32_t magic;
    in.read(reinterpret_cast<char*>(&magic), sizeof(magic));
    return magic == MAGIC_NUMBER;
}

void Serializer::save(const std::string& filename, Directory* root) {
    std::ofstream out(filename, std::ios::binary);
    if (!out.is_open()) {
        throw ArchiveException("Cannot open file for writing: " + filename);
    }

    uint32_t magic = MAGIC_NUMBER;
    out.write(reinterpret_cast<const char*>(&magic), sizeof(magic));

    writeDirectory(out, root);

    out.close();
    Logger::getInstance()->info("Archive saved to " + filename);
}

std::unique_ptr<Directory> Serializer::load(const std::string& filename) {
    std::ifstream in(filename, std::ios::binary);
    if (!in.is_open()) {
        throw ArchiveException("Cannot open file for reading: " + filename);
    }

    if (!checkMagicNumber(in)) {
        throw ArchiveException("Invalid file format: magic number mismatch");
    }

    auto root = readResource(in);
    if (!root) {
        throw ArchiveException("Failed to load root directory");
    }

    in.close();
    Logger::getInstance()->info("Archive loaded from " + filename);

    return std::unique_ptr<Directory>(static_cast<Directory*>(root.release()));
}