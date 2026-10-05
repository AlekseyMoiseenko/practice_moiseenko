#pragma once
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <cstring>
#include <limits>
#include <vector>
#include <algorithm>
#include <string>
#include <functional>

// ---------- имена файлов ----------
extern const char* FILE_TYPES;
extern const char* FILE_COMPONENTS;
extern const char* FILE_COMPAT;

// ---------- POD-структуры для файлов ----------
struct TypeRec      { int code; char name[64]; };
struct ComponentRec { int code; int typeCode; char manufacturer[64]; char model[64];
                      char params[128]; double price; int available; };
struct CompatRec    { int compCode; int compatCode; };

// ---------- динамические списки ----------
struct Type      { int code; char name[64]; Type* next; };
struct Component { int code; int typeCode; char manufacturer[64]; char model[64];
                   char params[128]; double price; int available; Component* next; };
struct Compat    { int compCode; int compatCode; Compat* next; };

// ---------- глобальные списки ----------
extern Type*      types;
extern Component* components;
extern Compat*    compats;

// ---------- утилиты ----------
void safeCopy(char* dst, const char* src, int size);
void safeCopy(char* dst, const std::string& src, int size);
bool fileExists(const char* name);

int    inputInt(const std::string& prompt);
double inputDouble(const std::string& prompt);
void   inputString(const std::string& prompt, char* dest, int size);

size_t      utf8Length(const std::string& s);
std::string padRight(const std::string& s, int width);
std::string padLeft (const std::string& s, int width);
std::string fmtPrice(double v);

void clearScreen();
void pauseScreen();