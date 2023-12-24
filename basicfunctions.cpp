#include <iostream>
#include <string>
#include "basicfunctions.h"
#include "GlobalData.h"
#include "namespacealiases.h"

// Reads and returns a character from the input stream,
// ignoring newlines.
char BasicFunctions::readchr() {
    char c = std::cin.get();
    while ( c == '\n' ) {
        c = std::cin.get();
    }
    return c;
}

// Reads and returns a positive integer from the input stream
int BasicFunctions::readint(int min, int max) {
    int num = 0;
    bool overflow = false;
    char c = readchr();

    if (c < '0' || c > '9') {
        return -1;
    }
    
    while ( c != '\n') {
        if (c >= '0' && c <= '9' && !overflow) {
            if ((max - (c - '0')) / 10 >= num) {    //prevent overflow
                num = num * 10 + (c - '0');         //which would
            } else {                                //result in num
                num = max;                          //becoming negative
                overflow = true;
            }
        }
        c = std::cin.get();
    }

    if (num > max) {
        num = max;
    } else if (num < min) {
        num = min;
    }

    return num;
}

// Returns true if the character is a number, false otherwise
bool BasicFunctions::isnum(char c) {
    return c >= '0' && c <= '9';
}

// Converts a letter to an integer based on its pos in the alphabet,
// starting from 1. Returns -1 if the character is not a letter.
int BasicFunctions::ltoi(char c) {
    for (int i = 0; i < alph::size; i++) {
        if (c == *alph::lower[i] || c == *alph::upper[i]) {
            return i + 1;
        }
    }
    return -1;
}

char BasicFunctions::itol(int i) {
    if (*alph::lower[i]) {
        return *alph::lower[i];
    }

    return '\0';
}

void BasicFunctions::action() {
    std::cout << "Press actions not yet defined." << std::endl;
    return;
}

int BasicFunctions::calcind(int boxWidth, int titleOrOptionLength) {
    int indentation = 0;
    indentation = (boxWidth - titleOrOptionLength) / 2;
    return indentation;
}

int BasicFunctions::floor(float x) {
    int y = int(x);
    if (x < y) {
        return y - 1;
    } else {
        return y;
    }
}

int BasicFunctions::ceil(float x) {
    int y = int(x);
    if (x > y) {
        return y + 1;
    } else {
        return y;
    }
}

int BasicFunctions::min(int x, int y) {
    if (x < y) {
        return x;
    } else {
        return y;
    }
}

int BasicFunctions::max(int x, int y) {
    if (x > y) {
        return x;
    } else {
        return y;
    }
}

float BasicFunctions::abs(float x) {
    if (x < 0) {
        return -x;
    } else {
        return x;
    }
}

float BasicFunctions::pow(float x, int n) {
    float result = 1;
    for (int i = 0; i < n; i++) {
        result *= x;
    }
    return result;
}

float BasicFunctions::sqrt(float x, float epsilon) {
    if (x < 0) {
        std::cout << "Error: cannot calculate square root of negative\
 number.";
        return 0;
    }
    if (x == 0 || x == 1) {
        return x;   // trivial cases
    }

    float guess = x / 2.0;
    float newGuess;
    do {
        newGuess = (guess + x / guess) / 2.0;
        if (abs(guess - newGuess) < epsilon) {
            break;
        }
        guess = newGuess;
    } while (true);

    return newGuess;
}

long long BasicFunctions::perm(int n, int r) {
    long long result = 1; // for trivial cases

    if (n < r || n < 0 || r < 0) {
        std::cout << "Error: please enter n >= r >= 0.";
        return -1;
    }
    if (n > gd::LARGE_NUM_MAX || r > gd::LARGE_NUM_MAX) {
        return -1;
    }

    // Check for potential overflow
    for (int i = n; i > n - r; i--) {
        if (result > gd::LARGE_NUM_MAX / i) {
            return -1;
        }
        result *= i;
    }
    
    return result;
}

long long BasicFunctions::fact(int n) {
    long long result = 1; // for trivial cases

    if (n < 0) {
        std::cout << "Error: please enter n >= 0.";
        return 0;
    }

    for (int i = n; i > 0; i--) {
        if (result > gd::LARGE_NUM_MAX / i) {
            return -1;
        }
        result *= i;
    }
    
    return result;
}

// Stylise a long long with dots to make it more readable
std::string BasicFunctions::prettylong(long long num) {
    std::string result = std::to_string(num);
    int length = result.length();
    int i = length - 3;

    while (i > 0) {
        result.insert(i, ".");
        i -= 3;
    }

    return result;
}
