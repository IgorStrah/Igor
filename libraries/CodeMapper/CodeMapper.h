#ifndef CODEMAPPER_H
#define CODEMAPPER_H

#include <Arduino.h>

class CodeMapper {
public:
    CodeMapper();

    const char* getCodeBySymbol(char symbol);
    const char* getSymbolByCode(const char* code);  // Если нужно оставить

private:
    static const char* findCodeInDefault(char symbol);
};

#endif