#include "ui.h"
#include "list.h"
#include "file_io.h"
#include "special.h"

// ---------- печать таблиц (UTF-8 aware) ----------
static const int W_CODE = 6, W_TYPE = 6, W_MAN = 16, W_MODEL = 20,
                 W_PAR = 26, W_PRICE = 12, W_AVAIL = 10;

void printTypes() {
    if (!types) { std::cout << "Список типов пуст.\n"; return; }
    std::cout << padRight("Код", W_CODE) << padRight("Название", 30) << "\n";
    std::cout << std::string(36, '-') << "\n";
    for (Type* p = types; p; p = p->next)
        std::cout << padRight(std::to_string(p->code), W_CODE)
                  << padRight(p->name, 30) << "\n";
}

void printComponents() {
    if (!components) { std::cout << "Список комплектующих пуст.\n"; return; }

    std::cout << padRight("Код",     W_CODE)
              << padRight("Тип",     W_TYPE)
              << padRight("Фирма",   W_MAN)
              << padRight("Модель",  W_MODEL)
              << padRight("Параметры", W_PAR)
              << padLeft ("Цена",    W_PRICE)
              << "  "
              << padLeft ("Наличие", W_AVAIL)
              << "\n";
    std::cout << std::string(W_CODE + W_TYPE + W_MAN + W_MODEL + W_PAR + W_PRICE + 2 + W_AVAIL, '-') << "\n";

    for (Component* p = components; p; p = p->next) {
        std::cout << padRight(std::to_string(p->code),     W_CODE)
                  << padRight(std::to_string(p->typeCode), W_TYPE)
                  << padRight(p->manufacturer,             W_MAN)
                  << padRight(p->model,                    W_MODEL)
                  << padRight(p->params,                   W_PAR)
                  << padLeft (fmtPrice(p->price),          W_PRICE)
                  << "  "
                  << padLeft (std::to_string(p->available),W_AVAIL)
                  << "\n";
    }
}

void printCompats() {
    if (!compats) { std::cout << "Список совместимости пуст.\n"; return; }
    std::cout << padRight("Код 1", 10) << padRight("Код 2", 10) << "\n";
    std::cout << std::string(20, '-') << "\n";
    for (Compat* p = compats; p; p = p->next)
        std::cout << padRight(std::to_string(p->compCode),   10)
                  << padRight(std::to_string(p->compatCode), 10) << "\n";
}

// ---------- п.1 ----------
void menuRead() {
    clearAll();
    if (!fileExists(FILE_TYPES) || !fileExists(FILE_COMPONENTS) || !fileExists(FILE_COMPAT)) {
        std::cout << "Файлы не найдены. Создаю тестовые данные...\n";
        createSampleFiles();
    }
    readTypesFromFile();
    readComponentsFromFile();
    readCompatsFromFile();
    std::cout << "Данные загружены в динамические списки.\n";
}

// ---------- п.2 ----------
void menuView() {
    int ch;
    do {
        clearScreen();
        std::cout << "=== Просмотр списков ===\n"
                  << "1. Типы комплектующих\n"
                  << "2. Комплектующие\n"
                  << "3. Совместимость\n"
                  << "4. Все списки\n"
                  << "0. Назад\n";
        ch = inputInt("Выбор: ");
        clearScreen();
        switch (ch) {
            case 1: printTypes(); break;
            case 2: printComponents(); break;
            case 3: printCompats(); break;
            case 4: printTypes(); std::cout << "\n"; printComponents();
                    std::cout << "\n"; printCompats(); break;
            case 0: break;
            default: std::cout << "Неверный пункт.\n"; break;
        }
        if (ch != 0) pauseScreen();
    } while (ch != 0);
}

// ---------- п.3 ----------
void menuSort() {
    int ch;
    do {
        clearScreen();
        std::cout << "=== Сортировка ===\n"
                  << "1. Типы по коду\n"
                  << "2. Типы по названию\n"
                  << "3. Комплектующие по коду\n"
                  << "4. Комплектующие по цене (возр.)\n"
                  << "5. Комплектующие по цене (убыв.)\n"
                  << "6. Комплектующие по коду типа\n"
                  << "7. Комплектующие по фирме\n"
                  << "8. Совместимость по коду 1\n"
                  << "9. Совместимость по коду 2\n"
                  << "0. Назад\n";
        ch = inputInt("Выбор: ");
        clearScreen();
        switch (ch) {
            case 1: sortTypes(1);      std::cout << "Отсортировано.\n"; break;
            case 2: sortTypes(2);      std::cout << "Отсортировано.\n"; break;
            case 3: sortComponents(1); std::cout << "Отсортировано.\n"; break;
            case 4: sortComponents(2); std::cout << "Отсортировано.\n"; break;
            case 5: sortComponents(3); std::cout << "Отсортировано.\n"; break;
            case 6: sortComponents(4); std::cout << "Отсортировано.\n"; break;
            case 7: sortComponents(5); std::cout << "Отсортировано.\n"; break;
            case 8: sortCompats(1);    std::cout << "Отсортировано.\n"; break;
            case 9: sortCompats(2);    std::cout << "Отсортировано.\n"; break;
            case 0: break;
            default: std::cout << "Неверный пункт.\n"; break;
        }
        if (ch != 0) pauseScreen();
    } while (ch != 0);
}

// ---------- п.4 ----------
static void searchTypes() {
    int code = inputInt("Код типа (-1 любой): ");
    std::cout << "Часть названия (пусто = любая): ";
    std::string nameFilter; std::getline(std::cin, nameFilter);

    bool found = false;
    for (Type* p = types; p; p = p->next) {
        if (code != -1 && p->code != code) continue;
        if (!nameFilter.empty() && std::string(p->name).find(nameFilter) == std::string::npos) continue;
        std::cout << padRight(std::to_string(p->code), W_CODE) << p->name << "\n";
        found = true;
    }
    if (!found) std::cout << "Не найдено.\n";
}
static void searchComponents() {
    int tc = inputInt("Код типа (-1 любой): ");
    double mn = inputDouble("Мин. цена (-1 любая): ");
    double mx = inputDouble("Макс. цена (-1 любая): ");
    int only = inputInt("Только в наличии? 1-да, 0-нет: ");

    bool found = false;
    for (Component* p = components; p; p = p->next) {
        if (tc != -1 && p->typeCode != tc) continue;
        if (mn >= 0 && p->price < mn) continue;
        if (mx >= 0 && p->price > mx) continue;
        if (only == 1 && p->available <= 0) continue;
        std::cout << padRight(std::to_string(p->code), W_CODE)
                  << padRight(p->manufacturer, W_MAN)
                  << padRight(p->model, W_MODEL)
                  << padLeft(fmtPrice(p->price), W_PRICE) << "\n";
        found = true;
    }
    if (!found) std::cout << "Не найдено.\n";
}
static void searchCompatByComponent() {
    int code = inputInt("Код комплектующего: ");
    bool found = false;
    for (Compat* p = compats; p; p = p->next)
        if (p->compCode == code || p->compatCode == code) {
            std::cout << padRight(std::to_string(p->compCode), 10)
                      << padRight(std::to_string(p->compatCode), 10) << "\n";
            found = true;
        }
    if (!found) std::cout << "Не найдено.\n";
}
static void searchCompatibleByTypeAndComponent() {
    int tc = inputInt("Код типа комплектующего: ");
    int cc = inputInt("Код конкретного комплектующего: ");
    bool found = false;
    for (Component* p = components; p; p = p->next)
        if (p->typeCode == tc && areCompatible(cc, p->code)) {
            std::cout << padRight(std::to_string(p->code), W_CODE)
                      << padRight(p->manufacturer, W_MAN)
                      << padRight(p->model, W_MODEL) << "\n";
            found = true;
        }
    if (!found) std::cout << "Не найдено.\n";
}

void menuSearch() {
    int ch;
    do {
        clearScreen();
        std::cout << "=== Поиск ===\n"
                  << "1. Типы по фильтру\n"
                  << "2. Комплектующие по фильтру\n"
                  << "3. Совместимость по коду комплектующего\n"
                  << "4. Комплектующие заданного типа, совместимые с конкретным\n"
                  << "0. Назад\n";
        ch = inputInt("Выбор: ");
        clearScreen();
        switch (ch) {
            case 1: searchTypes(); break;
            case 2: searchComponents(); break;
            case 3: searchCompatByComponent(); break;
            case 4: searchCompatibleByTypeAndComponent(); break;
            case 0: break;
            default: std::cout << "Неверный пункт.\n"; break;
        }
        if (ch != 0) pauseScreen();
    } while (ch != 0);
}

// ---------- п.5 ----------
static void addTypeInteractive() {
    int code = inputInt("Код типа: ");
    if (typeExists(code)) { std::cout << "Такой код уже есть.\n"; return; }
    char name[64]; inputString("Название: ", name, 64);
    addTypeNode(code, name);
    std::cout << "Тип добавлен.\n";
}
static void addComponentInteractive() {
    int code = inputInt("Код комплектующего: ");
    if (componentExists(code)) { std::cout << "Такой код уже есть.\n"; return; }
    int tc = inputInt("Код типа: ");
    if (!typeExists(tc)) { std::cout << "Тип не найден.\n"; return; }
    char man[64], model[64], par[128];
    inputString("Фирма: ", man, 64);
    inputString("Модель: ", model, 64);
    inputString("Параметры: ", par, 128);
    double price = inputDouble("Цена: ");
    int avail = inputInt("Наличие: ");
    addComponentNode(code, tc, man, model, par, price, avail);
    std::cout << "Комплектующее добавлено.\n";
}
static void addCompatInteractive() {
    int a = inputInt("Код комплектующего: ");
    int b = inputInt("Код совместимого: ");
    if (!componentExists(a) || !componentExists(b)) { std::cout << "Код не найден.\n"; return; }
    addCompatNode(a, b);
    std::cout << "Совместимость добавлена.\n";
}

void menuAdd() {
    int ch;
    do {
        clearScreen();
        std::cout << "=== Добавление ===\n"
                  << "1. Тип комплектующего\n"
                  << "2. Комплектующее\n"
                  << "3. Совместимость\n"
                  << "0. Назад\n";
        ch = inputInt("Выбор: ");
        clearScreen();
        switch (ch) {
            case 1: addTypeInteractive(); break;
            case 2: addComponentInteractive(); break;
            case 3: addCompatInteractive(); break;
            case 0: break;
            default: std::cout << "Неверный пункт.\n"; break;
        }
        if (ch != 0) pauseScreen();
    } while (ch != 0);
}

// ---------- п.6 ----------
static void deleteTypeInteractive() {
    int code = inputInt("Код типа для удаления: ");
    Type* p = types; Type* prev = nullptr;
    while (p) {
        if (p->code == code) {
            if (prev) prev->next = p->next; else types = p->next;
            delete p; std::cout << "Тип удалён.\n"; return;
        }
        prev = p; p = p->next;
    }
    std::cout << "Тип не найден.\n";
}
static void deleteComponentInteractive() {
    int code = inputInt("Код комплектующего для удаления: ");
    Component* p = components; Component* prev = nullptr;
    while (p) {
        if (p->code == code) {
            if (prev) prev->next = p->next; else components = p->next;
            delete p;
            removeCompatByComponentCode(code);
            std::cout << "Комплектующее удалено, связки совместимости очищены.\n";
            return;
        }
        prev = p; p = p->next;
    }
    std::cout << "Комплектующее не найдено.\n";
}
static void deleteCompatInteractive() {
    int a = inputInt("Код 1: ");
    int b = inputInt("Код 2: ");
    while (compats &&
          ((compats->compCode == a && compats->compatCode == b) ||
           (compats->compCode == b && compats->compatCode == a))) {
        Compat* p = compats; compats = compats->next; delete p;
    }
    Compat* p = compats;
    while (p && p->next) {
        bool m = (p->next->compCode == a && p->next->compatCode == b) ||
                 (p->next->compCode == b && p->next->compatCode == a);
        if (m) { Compat* d = p->next; p->next = d->next; delete d; }
        else p = p->next;
    }
    std::cout << "Запись совместимости удалена, если была.\n";
}

void menuDelete() {
    int ch;
    do {
        clearScreen();
        std::cout << "=== Удаление ===\n"
                  << "1. Тип комплектующего\n"
                  << "2. Комплектующее\n"
                  << "3. Совместимость\n"
                  << "0. Назад\n";
        ch = inputInt("Выбор: ");
        clearScreen();
        switch (ch) {
            case 1: deleteTypeInteractive(); break;
            case 2: deleteComponentInteractive(); break;
            case 3: deleteCompatInteractive(); break;
            case 0: break;
            default: std::cout << "Неверный пункт.\n"; break;
        }
        if (ch != 0) pauseScreen();
    } while (ch != 0);
}

// ---------- п.7 ----------
static void editTypeInteractive() {
    int code = inputInt("Код типа для редактирования: ");
    for (Type* p = types; p; p = p->next)
        if (p->code == code) {
            char n[64]; inputString("Новое название: ", n, 64);
            safeCopy(p->name, n, 64);
            std::cout << "Тип отредактирован.\n"; return;
        }
    std::cout << "Тип не найден.\n";
}
static void editComponentInteractive() {
    int code = inputInt("Код комплектующего для редактирования: ");
    for (Component* p = components; p; p = p->next)
        if (p->code == code) {
            int tc = inputInt("Новый код типа: ");
            if (!typeExists(tc)) { std::cout << "Тип не найден.\n"; return; }
            char man[64], model[64], par[128];
            inputString("Новая фирма: ", man, 64);
            inputString("Новая модель: ", model, 64);
            inputString("Новые параметры: ", par, 128);
            double price = inputDouble("Новая цена: ");
            int avail = inputInt("Новое наличие: ");
            p->typeCode = tc;
            safeCopy(p->manufacturer, man, 64);
            safeCopy(p->model, model, 64);
            safeCopy(p->params, par, 128);
            p->price = price; p->available = avail;
            std::cout << "Комплектующее отредактировано.\n"; return;
        }
    std::cout << "Комплектующее не найдено.\n";
}
static void editCompatInteractive() {
    int a = inputInt("Старый код 1: ");
    int b = inputInt("Старый код 2: ");
    for (Compat* p = compats; p; p = p->next) {
        bool m = (p->compCode == a && p->compatCode == b) ||
                 (p->compCode == b && p->compatCode == a);
        if (m) {
            int na = inputInt("Новый код 1: ");
            int nb = inputInt("Новый код 2: ");
            if (!componentExists(na) || !componentExists(nb)) {
                std::cout << "Новые коды не найдены.\n"; return;
            }
            p->compCode = na; p->compatCode = nb;
            std::cout << "Совместимость отредактирована.\n"; return;
        }
    }
    std::cout << "Запись совместимости не найдена.\n";
}

void menuEdit() {
    int ch;
    do {
        clearScreen();
        std::cout << "=== Редактирование ===\n"
                  << "1. Тип комплектующего\n"
                  << "2. Комплектующее\n"
                  << "3. Совместимость\n"
                  << "0. Назад\n";
        ch = inputInt("Выбор: ");
        clearScreen();
        switch (ch) {
            case 1: editTypeInteractive(); break;
            case 2: editComponentInteractive(); break;
            case 3: editCompatInteractive(); break;
            case 0: break;
            default: std::cout << "Неверный пункт.\n"; break;
        }
        if (ch != 0) pauseScreen();
    } while (ch != 0);
}

// ---------- главное меню ----------
void mainMenu() {
    int choice;
    do {
        clearScreen();
        std::cout << "=== Главное меню ===\n"
                  << " 1. Чтение данных из файла\n"
                  << " 2. Просмотр всего списка\n"
                  << " 3. Сортировка данных\n"
                  << " 4. Поиск данных\n"
                  << " 5. Добавление данных\n"
                  << " 6. Удаление данных\n"
                  << " 7. Редактирование данных\n"
                  << " 8. Подбор и оформление заказа\n"
                  << " 9. Выход без сохранения\n"
                  << "10. Выход с сохранением\n";
        choice = inputInt("Выбор: ");
        clearScreen();

        switch (choice) {
            case 1:
                menuRead();
                pauseScreen();
                break;
            case 2:
                menuView();
                break;
            case 3:
                menuSort();
                break;
            case 4:
                menuSearch();
                break;
            case 5:
                menuAdd();
                break;
            case 6:
                menuDelete();
                break;
            case 7:
                menuEdit();
                break;
            case 8:
                menuSpecial();
                break;
            case 9: {
                std::cout << "Внимание! Изменения не будут сохранены.\n"
                        << "Вы действительно хотите выйти? 1-да, 0-отмена: ";
                int confirm = inputInt("");
                if (confirm == 1) {
                    clearAll();
                    std::cout << "Выход без сохранения.\n";
                    return;
                }
                std::cout << "Выход отменён.\n";
                pauseScreen();
                break;
            }
            case 10:
                saveAllFiles();
                clearAll();
                std::cout << "Выход с сохранением.\n";
                return;
            default:
                std::cout << "Неверный пункт меню.\n";
                pauseScreen();
                break;
        }
    } while (true);
}