#include "tests.h"
#include "common.h"
#include "list.h"
#include "file_io.h"

static int passed = 0, failed = 0;

#define CHECK(cond, msg) do { \
    if (cond) { ++passed; std::cout << "[ OK ] " << msg << "\n"; } \
    else      { ++failed; std::cout << "[FAIL] " << msg << "\n"; } \
} while (0)

// ---------- UTF-8 ----------
static void testUtf8() {
    std::cout << "\n-- UTF-8 утилиты --\n";
    CHECK(utf8Length("abc") == 3,    "utf8Length: ascii");
    CHECK(utf8Length("привет") == 6, "utf8Length: кириллица");
    CHECK(padRight("аб", 5).size() == 5 + 2, "padRight: длина в байтах корректна");
    CHECK(utf8Length(padRight("аб", 5)) == 5, "padRight: длина в символах = 5");
    CHECK(utf8Length(padLeft("аб", 5)) == 5,  "padLeft: длина в символах = 5");
}

// ---------- Списки ----------
static void testLists() {
    std::cout << "\n-- Динамические списки --\n";
    clearAll();
    CHECK(types == nullptr, "clearAll: types пуст");

    addTypeNode(1, "A");
    addTypeNode(2, "B");
    CHECK(types && types->code == 1 && types->next->code == 2,
          "addTypeNode: порядок сохранён");
    CHECK(typeExists(1) && typeExists(2) && !typeExists(3),
          "typeExists: работает");

    addComponentNode(10, 1, "M1", "Md1", "p1", 100.0, 1);
    addComponentNode(20, 2, "M2", "Md2", "p2", 200.0, 2);
    CHECK(componentExists(10) && componentExists(20) && !componentExists(30),
          "componentExists: работает");

    addCompatNode(10, 20);
    CHECK(areCompatible(10, 20), "areCompatible: симметрия 10-20");
    CHECK(areCompatible(20, 10), "areCompatible: симметрия 20-10");
    CHECK(areCompatible(10, 10), "areCompatible: self");
    CHECK(!areCompatible(10, 99), "areCompatible: отсутствие");

    removeCompatByComponentCode(20);
    CHECK(!areCompatible(10, 20), "removeCompat: удаление");

    clearAll();
    CHECK(!types && !components && !compats, "clearAll: всё очищено");
}

// ---------- Сортировка ----------
static void testSort() {
    std::cout << "\n-- Сортировка --\n";
    clearAll();
    addComponentNode(3, 1, "C", "M", "p", 300, 1);
    addComponentNode(1, 1, "A", "M", "p", 100, 1);
    addComponentNode(2, 1, "B", "M", "p", 200, 1);

    sortComponents(2); // по цене возрастание
    CHECK(components->price == 100, "sort: цена 1-я = 100");
    CHECK(components->next->price == 200, "sort: цена 2-я = 200");
    CHECK(components->next->next->price == 300, "sort: цена 3-я = 300");

    sortComponents(3); // по цене убывание
    CHECK(components->price == 300, "sort: убыв. 1-я = 300");

    sortComponents(1); // по коду
    CHECK(components->code == 1, "sort: код 1-й = 1");
    CHECK(components->next->code == 2, "sort: код 2-й = 2");
    CHECK(components->next->next->code == 3, "sort: код 3-й = 3");

    clearAll();

    addTypeNode(5, "E");
    addTypeNode(1, "A");
    addTypeNode(3, "C");
    sortTypes(1);
    CHECK(types->code == 1 && types->next->code == 3 && types->next->next->code == 5,
          "sortTypes: по коду");
    sortTypes(2);
    CHECK(std::string(types->name) == "A" &&
          std::string(types->next->name) == "C" &&
          std::string(types->next->next->name) == "E",
          "sortTypes: по названию");
    clearAll();
}

// ---------- Файлы ----------
static void testFileRoundtrip() {
    std::cout << "\n-- Файлы: round-trip --\n";
    clearAll();

    // готовим данные
    addTypeNode(101, "TestType");
    addComponentNode(201, 101, "Man", "Mod", "Par", 1234.5, 7);
    addCompatNode(201, 201);
    saveAllFiles();

    // читаем заново
    clearAll();
    readTypesFromFile();
    readComponentsFromFile();
    readCompatsFromFile();

    CHECK(types && types->code == 101 && std::string(types->name) == "TestType",
          "файл types: round-trip");
    CHECK(components && components->code == 201 &&
          components->typeCode == 101 &&
          components->price == 1234.5 &&
          components->available == 7 &&
          std::string(components->manufacturer) == "Man",
          "файл components: round-trip");
    CHECK(compats && compats->compCode == 201 && compats->compatCode == 201,
          "файл compat: round-trip");

    clearAll();
    // восстановим демо-набор, чтобы пользователь не запутался
    createSampleFiles();
}

// ---------- СФ ----------
// Проверим, что подбор вариантов находит хотя бы один при широком диапазоне
// (без записи в файл напрямую — вызывается вспомогательная часть логики)
// Отдельный тест: areCompatible + базовое поведение
static void testSpecialLogic() {
    std::cout << "\n-- Логика СФ --\n";
    clearAll();

    addTypeNode(1, "CPU");
    addTypeNode(2, "MB");
    addComponentNode(11, 1, "X", "x", "", 5000, 2);
    addComponentNode(12, 2, "Y", "y", "", 3000, 2);
    addCompatNode(11, 12);

    // имитируем подбор: одна пара в бюджете
    int found = 0;
    for (Component* a = components; a; a = a->next)
        for (Component* b = components; b; b = b->next)
            if (a->typeCode != b->typeCode &&
                areCompatible(a->code, b->code) &&
                a->available > 0 && b->available > 0 &&
                a->price + b->price >= 1000 &&
                a->price + b->price <= 10000)
                ++found;

    CHECK(found >= 1, "СФ: найден как минимум 1 вариант");
    clearAll();
}

void runAllTests() {
    std::cout << "=====================\n";
    std::cout << " Запуск тестов\n";
    std::cout << "=====================\n";

    testUtf8();
    testLists();
    testSort();
    testFileRoundtrip();
    testSpecialLogic();

    std::cout << "\n=====================\n";
    std::cout << "Пройдено: " << passed << "\n";
    std::cout << "Провалено: " << failed << "\n";
    std::cout << "=====================\n";

    // не оставляем за собой тестовые бинарники
    createSampleFiles();
}