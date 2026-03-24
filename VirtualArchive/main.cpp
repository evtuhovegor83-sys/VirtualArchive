#include <iostream>
#include <string>
#include <memory>
#include <functional>
#include <algorithm>
#include "Directory.h"
#include "File.h"
#include "exceptions.h"

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
    }
    catch (const exception& e) {
        cout << "Ошибка: " << e.what() << endl;
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
    }
    catch (const exception& e) {
        cout << "Ошибка: " << e.what() << endl;
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
    }
    catch (const exception& e) {
        cout << "Ошибка: " << e.what() << endl;
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
    }
    else {
        cout << "Ресурс \"" << name << "\" не найден\n";
    }
}

// Функция для статистики (без использования count)
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
}

// Сохранение в файл (заглушка)
void saveArchive() {
    cout << "Сохранение архива (будет реализовано позже)...\n";
}

// Загрузка из файла (заглушка)
void loadArchive() {
    cout << "Загрузка архива (будет реализовано позже)...\n";
}

int main() {
    setlocale(LC_ALL, "Russian");

    // Инициализация корневой папки
    root = make_unique<Directory>("root", AccessLevel::ADMIN);

    cout << "=== СИСТЕМА ВИРТУАЛЬНОГО АРХИВА ===\n";
    cout << "Добро пожаловать!\n";

    int choice;

    do {
        showMenu();
        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Ошибка! Введите число.\n";
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
            break;
        case 4:
            deleteResource();
            break;
        case 5:
            cout << "Перемещение (будет реализовано позже)...\n";
            break;
        case 6:
            cout << "Копирование (будет реализовано позже)...\n";
            break;
        case 7:
            searchResource();
            break;
        case 8:
            cout << "Фильтрация по дате (будет реализовано позже)...\n";
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
            break;
        default:
            cout << "Неверный выбор! Введите число от 0 до 11.\n";
        }
    } while (choice != 0);

    return 0;
}