#pragma once
#include "common.h"

void clearTypes();
void clearComponents();
void clearCompats();
void clearAll();

void addTypeNode(int code, const char* name);
void addComponentNode(int code, int typeCode, const char* man, const char* model,
                      const char* params, double price, int available);
void addCompatNode(int compCode, int compatCode);

bool typeExists(int code);
bool componentExists(int code);
bool areCompatible(int a, int b);

void removeCompatByComponentCode(int code);

// критерии сортировки — целые числа, см. соответствующие меню
void sortTypes(int criterion);
void sortComponents(int criterion);
void sortCompats(int criterion);