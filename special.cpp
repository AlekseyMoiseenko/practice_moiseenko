#include "special.h"
#include "list.h"

struct Build { std::vector<Component*> comps; double total; };

static std::vector<Build> lastBuilds;

void resetLastBuilds() { lastBuilds.clear(); }

static void printBuild(std::ostream& os, const Build& b, int num) {
    os << "Вариант " << num << ", итого: " << fmtPrice(b.total) << " руб.\n";
    for (Component* c : b.comps) {
        os << "  " << padRight(std::to_string(c->code), 6)
           << padRight(std::to_string(c->typeCode), 6)
           << padRight(c->manufacturer, 16)
           << padRight(c->model, 20)
           << padRight(c->params, 26)
           << padLeft(fmtPrice(c->price), 12) << " руб."
           << "  наличие: " << c->available << "\n";
    }
    os << "----------------------------------------\n";
}

static void buildVariants() {
    if (!types || !components) { std::cout << "Сначала загрузите данные (п.1).\n"; return; }

    double mn = inputDouble("Мин. стоимость: ");
    double mx = inputDouble("Макс. стоимость: ");
    if (mn > mx) { std::cout << "Минимум больше максимума.\n"; return; }

    std::vector<int> typeCodes;
    for (Type* t = types; t; t = t->next) {
        for (Component* c = components; c; c = c->next)
            if (c->typeCode == t->code && c->available > 0) {
                typeCodes.push_back(t->code); break;
            }
    }
    if (typeCodes.empty()) { std::cout << "Нет доступных комплектующих.\n"; return; }

    std::vector<std::vector<Component*>> options;
    for (int tc : typeCodes) {
        std::vector<Component*> v;
        for (Component* c = components; c; c = c->next)
            if (c->typeCode == tc && c->available > 0) v.push_back(c);
        options.push_back(v);
    }

    std::vector<Component*> cur;
    std::vector<Build> builds;

    std::function<void(int,double)> dfs = [&](int idx, double total) {
        if (total > mx + 1e-9) return;
        if (idx == (int)options.size()) {
            if (total >= mn - 1e-9) builds.push_back({cur, total});
            return;
        }
        for (Component* c : options[idx]) {
            if (total + c->price > mx + 1e-9) continue;
            bool ok = true;
            for (Component* prev : cur)
                if (!areCompatible(prev->code, c->code)) { ok = false; break; }
            if (!ok) continue;
            cur.push_back(c);
            dfs(idx + 1, total + c->price);
            cur.pop_back();
        }
    };
    dfs(0, 0);

    std::sort(builds.begin(), builds.end(),
              [](const Build& a, const Build& b) { return a.total < b.total; });
    lastBuilds = builds;

    std::ofstream f("sf_builds.txt");
    if (builds.empty()) {
        std::cout << "Варианты не найдены.\n";
        f << "Варианты не найдены.\n";
    } else {
        std::cout << "Найдено вариантов: " << builds.size() << "\n";
        f << "Найдено вариантов: " << builds.size() << "\n";
        for (size_t i = 0; i < builds.size(); ++i) {
            printBuild(std::cout, builds[i], (int)i + 1);
            printBuild(f,         builds[i], (int)i + 1);
        }
    }
    std::cout << "\nРезультат записан в sf_builds.txt\n";
}

static void placeOrder() {
    if (lastBuilds.empty()) { std::cout << "Сначала выполните подбор (п.1).\n"; return; }
    int num = inputInt("Номер варианта: ");
    if (num < 1 || num > (int)lastBuilds.size()) { std::cout << "Неверный номер.\n"; return; }
    std::string customer;
    std::cout << "ФИО заказчика: ";
    std::getline(std::cin, customer);
    int qty = inputInt("Количество комплектов: ");
    if (qty <= 0) { std::cout << "Неверное количество.\n"; return; }

    Build& b = lastBuilds[num - 1];
    for (Component* c : b.comps)
        if (c->available < qty) {
            std::cout << "Недостаточно комплектующего " << c->code
                      << " (в наличии " << c->available << ").\n";
            return;
        }

    std::ofstream f("sf_order.txt", std::ios::app);
    auto writeOrder = [&](std::ostream& os) {
        os << "Заказ: " << customer
           << ", количество: " << qty
           << ", сумма за комплект: " << fmtPrice(b.total)
           << ", общая сумма: " << fmtPrice(b.total * qty) << "\n";
        for (Component* c : b.comps)
            os << "  " << c->code << " | " << c->manufacturer << " " << c->model
               << " x" << qty << "\n";
        os << "----------------------------------------\n";
    };
    writeOrder(std::cout);
    writeOrder(f);
    f.close();

    for (Component* c : b.comps) c->available -= qty;
    std::cout << "Заказ оформлен и записан в sf_order.txt\n";
}

void menuSpecial() {
    int ch;
    do {
        clearScreen();
        std::cout << "=== Подбор и оформление заказа ===\n"
                  << "1. Подобрать варианты комплектации в ценовом диапазоне\n"
                  << "2. Оформить заказ выбранного варианта\n"
                  << "0. Назад\n";
        ch = inputInt("Выбор: ");
        clearScreen();
        switch (ch) {
            case 1: buildVariants(); break;
            case 2: placeOrder();    break;
            case 0: break;
            default: std::cout << "Неверный пункт.\n"; break;
        }
        if (ch != 0) pauseScreen();
    } while (ch != 0);
}