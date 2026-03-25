#pragma once
#include "Resource.h"
#include <vector>
#include <memory>

enum class AccessLevel {
    GUEST,
    USER,
    ADMIN
};

enum class SortBy {
    NAME,
    SIZE,
    DATE
};

class Directory : public Resource {
private:
    std::vector<std::unique_ptr<Resource>> children;
    AccessLevel accessLevel;

public:
    Directory(const std::string& name, AccessLevel level = AccessLevel::USER);

    void addChild(std::unique_ptr<Resource> child);
    std::unique_ptr<Resource> removeChild(const std::string& name);
    Resource* findChild(const std::string& name) const;
    void exportToCSV(const std::string& filename) const;

    AccessLevel getAccessLevel() const;
    void setAccessLevel(AccessLevel level);
    void sortChildren(SortBy sortBy);
    long long getSize() const override;
    void print(int depth = 0) const override;
    std::unique_ptr<Resource> clone() const override;

    const std::vector<std::unique_ptr<Resource>>& getChildren() const;

    static bool validateName(const std::string& name);

    // Поиск по маске
    std::vector<Resource*> searchByMask(const std::string& mask) const;

    // Фильтрация по дате
    std::vector<Resource*> filterByDate(const Date& start, const Date& end) const;

    // Перемещение ресурса (ШАГ 2)
    void moveChild(const std::string& name, Directory* newParent);
};