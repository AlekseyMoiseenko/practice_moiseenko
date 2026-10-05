#include "list.h"
#include <cstring>

void clearTypes() {
    while (types) { Type* p = types; types = types->next; delete p; }
}
void clearComponents() {
    while (components) { Component* p = components; components = components->next; delete p; }
}
void clearCompats() {
    while (compats) { Compat* p = compats; compats = compats->next; delete p; }
}
void clearAll() { clearTypes(); clearComponents(); clearCompats(); }

void addTypeNode(int code, const char* name) {
    Type* n = new Type;
    n->code = code;
    safeCopy(n->name, name, 64);
    n->next = nullptr;
    if (!types) { types = n; return; }
    Type* p = types;
    while (p->next) p = p->next;
    p->next = n;
}

void addComponentNode(int code, int typeCode, const char* man, const char* model,
                      const char* params, double price, int available) {
    Component* n = new Component;
    n->code = code;
    n->typeCode = typeCode;
    safeCopy(n->manufacturer, man, 64);
    safeCopy(n->model, model, 64);
    safeCopy(n->params, params, 128);
    n->price = price;
    n->available = available;
    n->next = nullptr;
    if (!components) { components = n; return; }
    Component* p = components;
    while (p->next) p = p->next;
    p->next = n;
}

void addCompatNode(int compCode, int compatCode) {
    Compat* n = new Compat;
    n->compCode = compCode;
    n->compatCode = compatCode;
    n->next = nullptr;
    if (!compats) { compats = n; return; }
    Compat* p = compats;
    while (p->next) p = p->next;
    p->next = n;
}

bool typeExists(int code) {
    for (Type* p = types; p; p = p->next) if (p->code == code) return true;
    return false;
}
bool componentExists(int code) {
    for (Component* p = components; p; p = p->next) if (p->code == code) return true;
    return false;
}

bool areCompatible(int a, int b) {
    if (a == b) return true;
    for (Compat* p = compats; p; p = p->next) {
        if ((p->compCode == a && p->compatCode == b) ||
            (p->compCode == b && p->compatCode == a)) return true;
    }
    return false;
}

void removeCompatByComponentCode(int code) {
    while (compats && (compats->compCode == code || compats->compatCode == code)) {
        Compat* p = compats; compats = compats->next; delete p;
    }
    Compat* p = compats;
    while (p && p->next) {
        if (p->next->compCode == code || p->next->compatCode == code) {
            Compat* d = p->next; p->next = d->next; delete d;
        } else p = p->next;
    }
}

// ---------- сортировки ----------
static void swapTypeData(Type* a, Type* b) {
    std::swap(a->code, b->code);
    char t[64]; std::strcpy(t, a->name);
    std::strcpy(a->name, b->name); std::strcpy(b->name, t);
}
static void swapComponentData(Component* a, Component* b) {
    std::swap(a->code, b->code); std::swap(a->typeCode, b->typeCode);
    char t[128];
    std::strcpy(t, a->manufacturer); std::strcpy(a->manufacturer, b->manufacturer); std::strcpy(b->manufacturer, t);
    std::strcpy(t, a->model);        std::strcpy(a->model, b->model);               std::strcpy(b->model, t);
    std::strcpy(t, a->params);       std::strcpy(a->params, b->params);             std::strcpy(b->params, t);
    std::swap(a->price, b->price);   std::swap(a->available, b->available);
}
static void swapCompatData(Compat* a, Compat* b) {
    std::swap(a->compCode, b->compCode);
    std::swap(a->compatCode, b->compatCode);
}

void sortTypes(int criterion) {
    for (Type* p = types; p; p = p->next)
        for (Type* q = p->next; q; q = q->next) {
            bool s = false;
            if (criterion == 1 && p->code > q->code) s = true;
            if (criterion == 2 && std::strcmp(p->name, q->name) > 0) s = true;
            if (s) swapTypeData(p, q);
        }
}
void sortComponents(int criterion) {
    for (Component* p = components; p; p = p->next)
        for (Component* q = p->next; q; q = q->next) {
            bool s = false;
            if (criterion == 1 && p->code > q->code) s = true;
            if (criterion == 2 && p->price > q->price) s = true;
            if (criterion == 3 && p->price < q->price) s = true;
            if (criterion == 4 && p->typeCode > q->typeCode) s = true;
            if (criterion == 5 && std::strcmp(p->manufacturer, q->manufacturer) > 0) s = true;
            if (s) swapComponentData(p, q);
        }
}
void sortCompats(int criterion) {
    for (Compat* p = compats; p; p = p->next)
        for (Compat* q = p->next; q; q = q->next) {
            bool s = false;
            if (criterion == 1 && p->compCode > q->compCode) s = true;
            if (criterion == 2 && p->compatCode > q->compatCode) s = true;
            if (s) swapCompatData(p, q);
        }
}