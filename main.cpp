#include <iostream>
#include <cctype>
#include <cstdio>
#include <fstream>

typedef enum TSymbolType {
    intconst, text, semicolon, period, quotas, othersy
} TSymbol;

char Char;
TSymbol Symbol;
char Spelling[9];
const int MAXLENGTH = 8;
int Constant;
const int MAXINTEGER = 1000000;
std::ifstream inputFile("/Users/moldovexc/CLionProjects/parserv2/input.txt");

//const char* Input = R"(1234;631;"Hello".5678;"course1"."end".)";
//int inputIndex = 0;

void GetNextChar() {
//    Char = Input[inputIndex];
//    inputIndex++;
    if (!inputFile.is_open()) {
        fprintf(stderr, "Error: Unable to open input file\n");
        return;
    }
    Char = inputFile.get();
}

void error(const char* message) {
    fprintf(stderr, "Error: %s\n", message);
}

void GetNextSymbol() {
    int digit;
    int k = 0;
//
//    while (Char != '\0' && !isdigit(Char) && Char != '\"' && Char != ';' && Char != '.' && !isalpha(Char)) {
//        GetNextChar();
//    }
//
//    if (Char == '\0') {
//        Symbol = othersy;
//        std::cout << "End of Input" << std::endl;
//        return;
//    }
    while (!inputFile.eof() && !isdigit(Char) && Char != '\"' && Char != ';' && Char != '.' && !isalpha(Char)) {
        GetNextChar();
    }

    if (inputFile.eof()) {
        Char = '\0';
    }

    while (Char == ' ') {
        GetNextChar();
    }

    switch (toupper(Char)) {
        case 'A' ... 'Z': {
            do {
                if (k < MAXLENGTH) {
                    Spelling[k] = Char;
                    k++;
                }
                GetNextChar();
            } while ((Char >= 'A' && Char <= 'Z') || (Char >= '0' && Char <= '9'));

            Spelling[k] = '\0';

            if (k > 8) {
                error("error: String is too long");
            }

            Symbol = text;
            std::cout << "Text: " << Spelling << std::endl;
        } break;
        case '0' ... '9': {
            Constant = 0;
            do {
                digit = Char - '0';
                if ((Constant < (MAXINTEGER / 10)) || ((Constant == (MAXINTEGER / 10)) && (digit <= (MAXINTEGER % 10)))) {
                    Constant = (10 * Constant) + digit;
                } else {
                    error("error: Int constant too large");
                    Constant = 0;
                }
                GetNextChar();
            } while (Char >= '0' && Char <= '9');
            Symbol = intconst;
            std::cout << "Integer: " << Constant << std::endl;
        } break;
        case '.' : {
            Symbol = period;
            GetNextChar();
            std::cout << "Period" << std::endl;
            return;
        }
        case ';' : {
            Symbol = semicolon;
            GetNextChar();
            std::cout << "Semicolon" << std::endl;
        } break;
        case '\0': {
            Symbol = othersy;
            std::cout << "End of Input" << std::endl;
            break;
        }
        case '\"': {
            Symbol = quotas;
            GetNextChar();
            int insideQuote = 0;
            while (insideQuote < 8 && Char != '\"') {
                Spelling[insideQuote] = Char;
                insideQuote++;
                GetNextChar();
            }
            if (Char == '\"') {
                Spelling[insideQuote] = '\0';
                std::cout << "String: " << Spelling << std::endl;
                GetNextChar();
            } else {
                error("error: String is too long or missing closing double quote");
                Spelling[insideQuote] = '\0';
                std::cout << "String: " << Spelling << std::endl;
            }
        } break;

        default: {
            Symbol = othersy;
            std::cout << "Unknown Symbol with ASCII value: " << static_cast<int>(Char) << std::endl;
            GetNextChar();
            break;
        }
    }
}

int accept(TSymbol symbol) {
    if (Symbol == symbol) {
        GetNextSymbol();
        return 1;
    }
    return 0;
}

int expect(TSymbol symbol) {
    if (accept(symbol)) {
        return 1;
    }
    error("expect: unexpected symbol");
    return 0;
}

void Field() {
    if (accept(intconst) || accept(quotas) || accept(text)) {
        return;
    } else if (accept(period)) {
        accept(quotas);
        if (Symbol == text) {
            Field();
        } else {
            error("field: expects text after period");
        }
    } else {
        error("field: expects intconst, string, text, or period");
    }
}



void Record() {
    Field();
    while (Symbol == period) {
        accept(period);
        Field();
    }
    expect(semicolon);
}

void DataFile() {
    Record();
    while (Symbol != othersy) {
        Record();
    }
    inputFile.close();
}

int main() {

    GetNextChar();
    GetNextSymbol();
    DataFile();
    return 0;
}
