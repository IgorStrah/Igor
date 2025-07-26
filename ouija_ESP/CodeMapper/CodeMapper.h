#ifndef CODEMAPPER_H
#define CODEMAPPER_H

#include <Arduino.h>

#define MAX_CODES 100     // Максимальное количество кодов
#define CODE_LENGTH 15    // Длина одного кода (14 символов + '\0')

class CodeMapper {
public:
    CodeMapper();                              // Конструктор

    void loadDefaults();                       // Загрузка предустановленных кодов
    const char* getSymbol(const char* code);   // Найти символ по коду
    bool addOrUpdateCode(const char* code, const char* symbol);  // Добавить/обновить код

private:
    char codes[MAX_CODES][CODE_LENGTH];   // Массив кодов
    String symbols[MAX_CODES];            // Массив символов
    int codeCount;                        // Текущее количество кодов

    int findIndex(const char* code);      // Поиск индекса кода
};

#endif
