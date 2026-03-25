#include <iostream>
#include <string>
#include <memory>
#include <functional>
#include "Directory.h"
#include "File.h"
#include "exceptions.h"
#include "Logger.h"
#include "Serializer.h"

using namespace std;

// Глобальная переменная для корневой папки
unique_ptr<Directory> root;

// Функция вывода меню
void showMenu() {
    cout << "\n========================================\n";
    cout << "   СИСТЕМА ВИРТУАЛЬНОГО АРХИВА\n";
    cout << "========================================\n";
    cout << "1. Создать файл\n";
    cout << "2. Создать папку\n";
    cout << "3. Показать структуру\n";
    cout << "4. Удалить ресурс\n";
    cout << "5. Переместить ресурс\n";
    cout << "6. Скопировать ресурс\n";
    cout << "7. Поиск по маске (имя/расширение)\n";
    cout << "8. Фильтрация по дате\n";
    cout << "9. Статистика\n";
    cout << "10. Сохранить архив\n";
    cout << "11. Загрузить архив\n";
    cout << "12. Сортировка корневой папки\n";
    cout << "0. Выход\n";
    cout << "========================================\n";
    cout << "Ваш выбор: ";
}

// Функция для создания файла
void createFile() {
    string name, ext;
    long long size;

    cout << "Введите имя файла: ";
    cin >> name;
    cout << "Введите расширение: ";
    cin >> ext;
    cout << "Введите размер (байт): ";
    cin >> size;

    try {
        auto file = make_unique<File>(name, ext, size);
        root->addChild(move(file));
        cout << "Файл успешно создан!\n";
        Logger::getInstance()->info("File created: " + name + "." + ext);
    }
    catch (const exception& e) {
        cout << "Ошибка: " << e.what() << endl;
        Logger::getInstance()->error("Failed to create file " + name + ": " + e.what());
    }
}

// Функция для создания папки
void createDirectory() {
    string name;
    int level;

    cout << "Введите имя папки: ";
    cin >> name;
    cout << "Выберите уровень доступа (0-GUEST, 1-USER, 2-ADMIN): ";
    cin >> level;

    AccessLevel access;
    switch (level) {
    case 0: access = AccessLevel::GUEST; break;
    case 2: access = AccessLevel::ADMIN; break;
    default: access = AccessLevel::USER;
    }

    try {
        auto dir = make_unique<Directory>(name, access);
        root->addChild(move(dir));
        cout << "Папка успешно создана!\n";
        Logger::getInstance()->info("Directory created: " + name + " (level: " + to_string(level) + ")");
    }
    catch (const exception& e) {
        cout << "Ошибка: " << e.what() << endl;
        Logger::getInstance()->error("Failed to create directory " + name + ": " + e.what());
    }
}

// Функция для удаления ресурса
void deleteResource() {
    string name;
    cout << "Введите имя ресурса для удаления: ";
    cin >> name;

    try {
        auto removed = root->removeChild(name);
        cout << "Ресурс \"" << name << "\" удалён\n";
        Logger::getInstance()->warning("Resource deleted: " + name);
    }
    catch (const exception& e) {
        cout << "Ошибка: " << e.what() << endl;
        Logger::getInstance()->error("Failed to delete " + name + ": " + e.what());
    }
}

// Функция для поиска по маске
void searchByMask() {
    string mask;
    cout << "Введите маску для поиска (часть имени или расширения): ";
    cin >> mask;

    auto results = root->searchByMask(mask);

    if (results.empty()) {
        cout << "Ничего не найдено по маске \"" << mask << "\"\n";
        Logger::getInstance()->warning("Search by mask \"" + mask + "\" found nothing");
    }
    else {
        cout << "Найдено " << results.size() << " ресурсов:\n";
        for (auto* res : results) {
            res->print();
        }
        Logger::getInstance()->info("Search by mask \"" + mask + "\" found " + to_string(results.size()) + " results");
    }
}

// Функция для фильтрации по дате
void filterByDate() {
    int day1, month1, year1, day2, month2, year2;

    cout << "Введите начальную дату (день месяц год): ";
    cin >> day1 >> month1 >> year1;
    cout << "Введите конечную дату (день месяц год): ";
    cin >> day2 >> month2 >> year2;

    Date start(day1, month1, year1);
    Date end(day2, month2, year2);

    auto results = root->filterByDate(start, end);

    if (results.empty()) {
        cout << "Ничего не найдено в указанном диапазоне дат\n";
        Logger::getInstance()->warning("Filter by date found nothing");
    }
    else {
        cout << "Найдено " << results.size() << " ресурсов:\n";
        for (auto* res : results) {
            res->print();
        }
        Logger::getInstance()->info("Filter by date found " + to_string(results.size()) + " results");
    }
}

// Функция для статистики
void showStatistics() {
    cout << "\n=== СТАТИСТИКА АРХИВА ===\n";
    cout << "Общий размер: " << root->getSize() << " байт\n";

    int fileCount = 0;
    int dirCount = 0;

    function<void(Directory*)> countResources = [&](Directory* dir) {
        const auto& children = dir->getChildren();
        for (size_t i = 0; i < children.size(); i++) {
            const auto& child = children[i];
            if (dynamic_cast<File*>(child.get())) {
                fileCount++;
            }
            else if (dynamic_cast<Directory*>(child.get())) {
                dirCount++;
                countResources(static_cast<Directory*>(child.get()));
            }
        }
        };

    countResources(root.get());

    cout << "Количество файлов: " << fileCount << "\n";
    cout << "Количество папок: " << dirCount << "\n";

    if (fileCount > 0) {
        cout << "Средний размер файла: " << root->getSize() / fileCount << " байт\n";
    }

    Logger::getInstance()->info("Statistics viewed: " + to_string(fileCount) + " files, " + to_string(dirCount) + " directories");
}

// Сохранение в файл
void saveArchive() {
    string filename = "archive.dat";
    try {
        Serializer::save(filename, root.get());
        cout << "Архив сохранён в файл " << filename << "\n";
        Logger::getInstance()->info("Archive saved to " + filename);
    }
    catch (const exception& e) {
        cout << "Ошибка сохранения: " << e.what() << "\n";
        Logger::getInstance()->error("Save failed: " + string(e.what()));
    }
}

// Загрузка из файла
void loadArchive() {
    string filename = "archive.dat";
    try {
        auto loadedRoot = Serializer::load(filename);
        if (loadedRoot) {
            root = std::move(loadedRoot);
            cout << "Архив загружен из файла " << filename << "\n";
            Logger::getInstance()->info("Archive loaded from " + filename);
        }
    }
    catch (const exception& e) {
        cout << "Ошибка загрузки: " << e.what() << "\n";
        Logger::getInstance()->error("Load failed: " + string(e.what()));
    }
}

// Функция для перемещения ресурса
void moveResource() {
    string name;
    string targetName;

    cout << "Введите имя ресурса для перемещения: ";
    cin >> name;
    cout << "Введите имя целевой папки: ";
    cin >> targetName;

    Resource* target = root->findChild(targetName);

    if (target == nullptr) {
        cout << "Целевая папка не найдена\n";
        Logger::getInstance()->warning("Move failed: target directory not found: " + targetName);
        return;
    }

    Directory* targetDir = dynamic_cast<Directory*>(target);
    if (targetDir == nullptr) {
        cout << "Целевой ресурс не является папкой\n";
        Logger::getInstance()->warning("Move failed: target is not a directory: " + targetName);
        return;
    }

    try {
        root->moveChild(name, targetDir);
        cout << "Ресурс \"" << name << "\" перемещён в папку \"" << targetName << "\"\n";
        Logger::getInstance()->info("Moved " + name + " to " + targetName);
    }
    catch (const exception& e) {
        cout << "Ошибка перемещения: " << e.what() << "\n";
        Logger::getInstance()->error("Move failed: " + string(e.what()));
    }
}

// Функция для копирования ресурса
void copyResource() {
    string name;
    string targetName;

    cout << "Введите имя ресурса для копирования: ";
    cin >> name;
    cout << "Введите имя целевой папки: ";
    cin >> targetName;

    Resource* source = root->findChild(name);
    if (source == nullptr) {
        cout << "Исходный ресурс не найден\n";
        Logger::getInstance()->warning("Copy failed: source not found: " + name);
        return;
    }

    Resource* target = root->findChild(targetName);
    if (target == nullptr) {
        cout << "Целевая папка не найдена\n";
        Logger::getInstance()->warning("Copy failed: target not found: " + targetName);
        return;
    }

    Directory* targetDir = dynamic_cast<Directory*>(target);
    if (targetDir == nullptr) {
        cout << "Целевой ресурс не является папкой\n";
        Logger::getInstance()->warning("Copy failed: target is not a directory");
        return;
    }

    try {
        auto copy = source->clone();
        targetDir->addChild(std::move(copy));
        cout << "Ресурс \"" << name << "\" скопирован в папку \"" << targetName << "\"\n";
        Logger::getInstance()->info("Copied " + name + " to " + targetName);
    }
    catch (const exception& e) {
        cout << "Ошибка копирования: " << e.what() << "\n";
        Logger::getInstance()->error("Copy failed: " + string(e.what()));
    }
}

// Функция для сортировки корневой папки (ШАГ 4)
void sortRoot() {
    int choice;
    cout << "Сортировка по: 1-Имя, 2-Размер, 3-Дата: ";
    cin >> choice;

    SortBy sortBy;
    switch (choice) {
    case 1: sortBy = SortBy::NAME; break;
    case 2: sortBy = SortBy::SIZE; break;
    case 3: sortBy = SortBy::DATE; break;
    default:
        cout << "Неверный выбор\n";
        return;
    }

    root->sortChildren(sortBy);
    cout << "Сортировка выполнена\n";
    Logger::getInstance()->info("Root directory sorted");
}

int main() {
    setlocale(LC_ALL, "Russian");

    root = make_unique<Directory>("root", AccessLevel::ADMIN);

    cout << "=== СИСТЕМА ВИРТУАЛЬНОГО АРХИВА ===\n";
    cout << "Добро пожаловать!\n";

    Logger::getInstance()->info("Program started");

    int choice;

    do {
        showMenu();
        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Ошибка! Введите число.\n";
            Logger::getInstance()->warning("Invalid input (not a number)");
            continue;
        }

        switch (choice) {
        case 1:
            createFile();
            break;
        case 2:
            createDirectory();
            break;
        case 3:
            cout << "\nСтруктура архива:\n";
            root->print();
            Logger::getInstance()->info("Structure viewed");
            break;
        case 4:
            deleteResource();
            break;
        case 5:
            moveResource();
            break;
        case 6:
            copyResource();
            break;
        case 7:
            searchByMask();
            break;
        case 8:
            filterByDate();
            break;
        case 9:
            showStatistics();
            break;
        case 10:
            saveArchive();
            break;
        case 11:
            loadArchive();
            break;
        case 12:
            sortRoot();
            break;
        case 0:
            cout << "Выход из программы...\n";
            Logger::getInstance()->info("Program exited");
            break;
        default:
            cout << "Неверный выбор! Введите число от 0 до 12.\n";
            Logger::getInstance()->warning("Invalid menu choice: " + to_string(choice));
        }
    } while (choice != 0);

    Logger::destroy();

    return 0;
}