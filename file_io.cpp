#include "file_io.h"
#include "list.h"
#include <vector>

void readTypesFromFile() {
    std::ifstream in(FILE_TYPES, std::ios::binary);
    if (!in) return;
    TypeRec r;
    while (in.read((char*)&r, sizeof(r))) addTypeNode(r.code, r.name);
}
void readComponentsFromFile() {
    std::ifstream in(FILE_COMPONENTS, std::ios::binary);
    if (!in) return;
    ComponentRec r;
    while (in.read((char*)&r, sizeof(r)))
        addComponentNode(r.code, r.typeCode, r.manufacturer, r.model, r.params, r.price, r.available);
}
void readCompatsFromFile() {
    std::ifstream in(FILE_COMPAT, std::ios::binary);
    if (!in) return;
    CompatRec r;
    while (in.read((char*)&r, sizeof(r))) addCompatNode(r.compCode, r.compatCode);
}

void saveAllFiles() {
    {
        std::ofstream t(FILE_TYPES, std::ios::binary);
        for (Type* p = types; p; p = p->next) {
            TypeRec r{}; r.code = p->code; safeCopy(r.name, p->name, 64);
            t.write((char*)&r, sizeof(r));
        }
    }
    {
        std::ofstream c(FILE_COMPONENTS, std::ios::binary);
        for (Component* p = components; p; p = p->next) {
            ComponentRec r{};
            r.code = p->code; r.typeCode = p->typeCode;
            safeCopy(r.manufacturer, p->manufacturer, 64);
            safeCopy(r.model, p->model, 64);
            safeCopy(r.params, p->params, 128);
            r.price = p->price; r.available = p->available;
            c.write((char*)&r, sizeof(r));
        }
    }
    {
        std::ofstream k(FILE_COMPAT, std::ios::binary);
        for (Compat* p = compats; p; p = p->next) {
            CompatRec r{}; r.compCode = p->compCode; r.compatCode = p->compatCode;
            k.write((char*)&r, sizeof(r));
        }
    }
}

void createSampleFiles() {
    {
        std::ofstream t(FILE_TYPES, std::ios::binary);
        auto wt = [&](int code, const char* name) {
            TypeRec r{}; r.code = code; safeCopy(r.name, name, 64);
            t.write((char*)&r, sizeof(r));
        };
        wt(1, "Процессор");
        wt(2, "Материнская плата");
        wt(3, "Память");
        wt(4, "Видеокарта");
        wt(5, "Блок питания");
        wt(6, "Накопитель");
    }

    std::vector<ComponentRec> comps;
    auto addComp = [&](int code, int tc, const char* m, const char* md,
                       const char* p, double pr, int a) {
        ComponentRec r{};
        r.code = code; r.typeCode = tc;
        safeCopy(r.manufacturer, m, 64);
        safeCopy(r.model, md, 64);
        safeCopy(r.params, p, 128);
        r.price = pr; r.available = a;
        comps.push_back(r);
    };
    addComp(101, 1, "Intel",  "Core i5-12400",   "LGA1700 2.5GHz", 18000, 5);
    addComp(102, 1, "AMD",    "Ryzen 5 5600",    "AM4 3.5GHz",     15000, 4);
    addComp(201, 2, "ASUS",   "Prime B660",      "LGA1700 DDR4",   12000, 3);
    addComp(202, 2, "MSI",    "B550-A Pro",      "AM4 DDR4",       10000, 2);
    addComp(301, 3, "Kingston","Fury Beast 16GB","DDR4 3200",       5000,10);
    addComp(302, 3, "Samsung","DDR4 16GB",       "DDR4 2666",       4500, 8);
    addComp(401, 4, "NVIDIA", "RTX 3060",        "12GB",           30000, 2);
    addComp(402, 4, "AMD",    "Radeon RX 6600",  "8GB",            25000, 3);
    addComp(501, 5, "Corsair","CV650",           "650W",            5500, 5);
    addComp(502, 5, "DeepCool","PF500",          "500W",            3500, 6);
    addComp(601, 6, "WD",     "Blue 1TB",        "HDD 7200rpm",     4000, 7);

    {
        std::ofstream c(FILE_COMPONENTS, std::ios::binary);
        for (auto& r : comps) c.write((char*)&r, sizeof(r));
    }

    {
        std::ofstream k(FILE_COMPAT, std::ios::binary);
        for (size_t i = 0; i < comps.size(); ++i)
            for (size_t j = i + 1; j < comps.size(); ++j) {
                if (comps[i].typeCode == comps[j].typeCode) continue;
                int t1 = comps[i].typeCode, t2 = comps[j].typeCode;
                bool ok = true;
                if ((t1 == 1 && t2 == 2) || (t1 == 2 && t2 == 1)) {
                    int cpu = (t1 == 1) ? comps[i].code : comps[j].code;
                    int mb  = (t1 == 2) ? comps[i].code : comps[j].code;
                    if (!((cpu == 101 && mb == 201) || (cpu == 102 && mb == 202))) ok = false;
                }
                if (ok) {
                    CompatRec r{}; r.compCode = comps[i].code; r.compatCode = comps[j].code;
                    k.write((char*)&r, sizeof(r));
                }
            }
    }
}