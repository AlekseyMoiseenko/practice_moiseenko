#include "common.h"

const char* FILE_TYPES      = "types.bin";
const char* FILE_COMPONENTS = "components.bin";
const char* FILE_COMPAT     = "compat.bin";

Type*      types      = nullptr;
Component* components = nullptr;
Compat*    compats    = nullptr;

void safeCopy(char* dst, const char* src, int size) {
    if (size <= 0) return;
    std::strncpy(dst, src, size - 1);
    dst[size - 1] = '\0';
}

void safeCopy(char* dst, const std::string& src, int size) {
    safeCopy(dst, src.c_str(), size);
}

bool fileExists(const char* name) {
    std::ifstream f(name, std::ios::binary);
    return f.good();
}

int inputInt(const std::string& prompt) {
    std::cout << prompt;
    int x;
    while (!(std::cin >> x)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Ошибка ввода. " << prompt;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return x;
}

double inputDouble(const std::string& prompt) {
    std::cout << prompt;
    double x;
    while (!(std::cin >> x)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Ошибка ввода. " << prompt;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return x;
}

void inputString(const std::string& prompt, char* dest, int size) {
    std::cout << prompt;
    std::string s;
    std::getline(std::cin, s);
    safeCopy(dest, s, size);
}

// --- UTF-8 осознанная длина: кириллица в UTF-8 = 2 байта, но 1 символ ---
size_t utf8Length(const std::string& s) {
    size_t len = 0;
    for (unsigned char c : s) {
        if ((c & 0xC0) != 0x80) ++len; // считаем только ведущие байты
    }
    return len;
}

std::string padRight(const std::string& s, int width) {
    size_t len = utf8Length(s);
    if (len >= (size_t)width) return s;
    return s + std::string(width - len, ' ');
}

std::string padLeft(const std::string& s, int width) {
    size_t len = utf8Length(s);
    if (len >= (size_t)width) return s;
    return std::string(width - len, ' ') + s;
}

std::string fmtPrice(double v) {
    std::ostringstream os;
    os << std::fixed << std::setprecision(2) << v;
    return os.str();
}

void clearScreen() {
#ifdef _WIN32
    std::system("cls");
#else
    std::system("clear");
#endif
}

void pauseScreen() {
    std::cout << "\nНажмите Enter для продолжения...";
    std::string dummy;
    std::getline(std::cin, dummy);
}