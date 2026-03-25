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
    cout << "7. Поиск по имени\n";
    cout << "8. Фильтрация по дате\n";
    cout << "9. Статистика\n";
    cout << "10. Сохранить архив\n";
    cout << "11. Загрузить архив\n";
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

// Функция для поиска
void searchResource() {
    string name;
    cout << "Введите имя для поиска: ";
    cin >> name;

    Resource* found = root->findChild(name);
    if (found) {
        cout << "Найден: ";
        found->print();
        Logger::getInstance()->info("Search found: " + name);
    }
    else {
        cout << "Ресурс \"" << name << "\" не найден\n";
        Logger::getInstance()->warning("Search not found: " + name);
    }
}

// Функция для статистики
void showStatistics() {
    cout << "\n=== СТАТИСТИКА АРХИВА ===\n";
    cout << "Общий размер: " << root->getSize() << " байт\n";

    // Подсчёт количества файлов и папок
    int fileCount = 0;
    int dirCount = 0;

    // Рекурсивная функция для подсчёта
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

// Сохранение в файл (заглушка)
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

// Загрузка из файла (заглушка)
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

// Функция для перемещения (заглушка)
void moveResource() {
    cout << "Перемещение (будет реализовано позже)...\n";
    Logger::getInstance()->info("Move resource requested");
}

// Функция для копирования (заглушка)
void copyResource() {
    cout << "Копирование (будет реализовано позже)...\n";
    Logger::getInstance()->info("Copy resource requested");
}

// Функция для фильтрации по дате (заглушка)
void filterByDate() {
    cout << "Фильтрация по дате (будет реализовано позже)...\n";
    Logger::getInstance()->info("Filter by date requested");
}

int main() {
    setlocale(LC_ALL, "Russian");

    // Инициализация корневой папки
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
            searchResource();
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
        case 0:
            cout << "Выход из программы...\n";
            Logger::getInstance()->info("Program exited");
            break;
        default:
            cout << "Неверный выбор! Введите число от 0 до 11.\n";
            Logger::getInstance()->warning("Invalid menu choice: " + to_string(choice));
        }
    } while (choice != 0);

    Logger::destroy();

    return 0;
}