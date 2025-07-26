#include "CodeMapper.h"

// Твой массив со значениями перенесён сюда
const int uidCount = 45;
const char rfidMap[uidCount][2][15] = {
    { "04BB8F1A237380", "!" }, //yes
    { "04C38F1A237380", "@" }, //ouija
    { "04BF8F1A237380", "#" }, //no
    { "042D8F1A237380", "a" }, 
    { "04318F1A237380", "b" }, 
    { "04358F1A237380", "c" }, 
    { "04398F1A237380", "d" }, 
    { "043D8F1A237380", "e" }, 
    { "04418F1A237380", "f" }, 
    { "04458F1A237380", "g" }, 
    { "04488F1A237380", "h" }, 
    { "044C8F1A237380", "i" }, 
    { "04508F1A237380", "j" }, 
    { "04548F1A237380", "k" }, 
    { "04588F1A237380", "l" }, 
    { "045C8F1A237380", "m" }, 
    { "04608F1A237380", "n" }, 
    { "04648F1A237380", "o" }, 
    { "04688F1A237380", "p" }, 
    { "046C8F1A237380", "q" }, 
    { "04708F1A237380", "r" }, 
    { "04748F1A237380", "s" }, 
    { "04788F1A237380", "t" }, 
    { "047C8F1A237380", "u" }, 
    { "04848F1A237380", "v" }, 
    { "04808F1A237380", "w" }, 
    { "04888F1A237380", "x" }, 
    { "0480C11A237381", "y" }, 
    { "04908F1A237380", "z" }, 
    { "04A08F1A237380", "1" }, 
    { "04A48F1A237380", "2" }, 
    { "04968E1A237380", "3" }, 
    { "049A8E1A237380", "4" }, 
    { "04A28E1A237380", "5" }, 
    { "04A68E1A237380", "6" }, 
    { "04B68F1A237380", "7" }, 
    { "049C8F1A237380", "8" }, 
    { "049C8F1A237380", "9" }, 
    { "04948F1A237380", "0" }, 
    { "04AC8F1A237380", "%" }, 
    { "04A88F1A237380", "%" }, 
    { "04B18F1A237380", "%" }
};
// Конструктор
CodeMapper::CodeMapper() {
    codeCount = 0;
}

// Загрузка дефолтных кодов
void CodeMapper::loadDefaults() {
    for (int i = 0; i < uidCount; i++) {
        addOrUpdateCode(defaultMap[i][0], defaultMap[i][1]);
    }
}

// Найти индекс кода
int CodeMapper::findIndex(const char* code) {
    for (int i = 0; i < codeCount; i++) {
        if (strncmp(codes[i], code, CODE_LENGTH) == 0) {
            return i;
        }
    }
    return -1;
}

// Получить символ по коду
const char* CodeMapper::getSymbol(const char* code) {
    int index = findIndex(code);
    if (index != -1) {
        return symbols[index].c_str();
    }
    return nullptr;
}

// Добавить новый код или обновить существующий
bool CodeMapper::addOrUpdateCode(const char* code, const char* symbol) {
    int index = findIndex(code);
    if (index != -1) {
        symbols[index] = symbol;
        return true;
    }

    if (codeCount >= MAX_CODES) {
        return false;  // Массив заполнен
    }

    strncpy(codes[codeCount], code, CODE_LENGTH - 1);
    codes[codeCount][CODE_LENGTH - 1] = '\0';
    symbols[codeCount] = symbol;
    codeCount++;
    return true;
}
