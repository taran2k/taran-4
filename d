#include <iostream>
#include <string.h> //pas later aan
#include <string>
#include "menus.h"
#include "basicfunctions.h"

#ifdef en
    #include "lang_en.h"
    #elif nl
    #include "lang_nl.h"
    #else
    #warning "No language selected, defaulting to English. \
Choose a language by adding -Den or -Dnl to the compiler flags."
#endif

menu::menu() {
    cornersymbol = '+';
    horizontalsymbol = '-';
    verticalsymbol = '|';
    spacingsymbol = ' ';
    hasTitle = true;
    title = "Empty menu!";
    menuTitleLength = 0;
    for (int i = 0; i < 10; i++) { 
        menuOptions[i] = "0";
    }
    for (int i = 0; i < 10; i++) { 
        menuOptionLength[i] = 0;
    }
    longestTitleOrOptionLength = 0;
}

menu::~menu() {

}

// SETTERS:
void menu::menuSetCornerSymbol(char C) {
    //cornersymbol = readString()[0];
    //std::cout << std::endl;
    cornersymbol = C;
}
void menu::menuSetHorizontalSymbol(char C) {
    //horizontalsymbol = readString()[0];
    //std::cout << std::endl;
    horizontalsymbol = C;
}
void menu::menuSetVerticalSymbol(char C) {
    //verticalsymbol = readString()[0];
    //std::cout << std::endl;
    verticalsymbol = C;
}
void menu::menuSetSpacingSymbol(char C) {
    //spacingsymbol = readString()[0];
    spacingsymbol = C;
    if (spacingsymbol == '\0') {
        spacingsymbol = ' ';
    }
    //std::cout << std::endl;
}
void menu::menuSetHasTitle(bool HASTITLE) {
    hasTitle = HASTITLE;
}
void menu::menuSetTitle(std::string TITLE) {
    if (hasTitle) {
        title = TITLE;
    }
}
void menu::menuSetOptions(int OPTIONNUMBER, std::string OPTION) {
    if (OPTIONNUMBER < 10) {
        menuOptions[OPTIONNUMBER] = OPTION;
    } else {
        std::cout << "Invalid option number." << std::endl;
    }
}
// SETTERS END

// FUNCTIONS:
void menu::menuCalculateTitleAndOptionLengths() {
    menuTitleLength = title.length();
    for (int i = 0; i < 10; i++) {
        menuOptionLength[i] = menuOptions[i].length();
    }
    // Also calculate the longest title/option length:
    longestTitleOrOptionLength = menuTitleLength;
    for (int i = 0; i < 10; i++) {
        if (menuOptionLength[i] > longestTitleOrOptionLength) {
            longestTitleOrOptionLength = menuOptionLength[i];
        }
    }
}

void menu::menuCalculateAmountOfOptions() {
    amountOfOptions = 0;
    for (int i = 0; i < 10; i++) {
        if (menuOptions[i] != "0") {
            amountOfOptions++;
        }
    }
}

int indentationCalculate(int boxWidth, int titleOrOptionLength) {
    int indentation = 0;
    indentation = (boxWidth - titleOrOptionLength) / 2;
    return indentation;
}
// FUNCTIONS END

// MENU CREATION (FOR TESTING):
void menu::menuCreate() { // string type aanpassen 
    std::cout << "Enter the corner symbol: ";
    menuSetCornerSymbol(readCharacter());
    std::cout << "Enter the horizontal symbol: ";
    menuSetHorizontalSymbol(readCharacter()); 
    std::cout << "Enter the vertical symbol: ";
    menuSetVerticalSymbol(readCharacter());
    std::cout << "Enter the spacing symbol (spacebar for a space,\
 which is the default): ";
    menuSetSpacingSymbol(readCharacter()); 
    std::cout << "Do you want to enter a title for the menu? (Y/n): ";
    char a = readCharacter();
    if (a == 'n' || a == 'N') {
        hasTitle = false;
    } else if (a == 'y' || a == 'Y') {
        hasTitle = true;
    } else {
        std::cout << "Invalid input, defaulting to yes." << std::endl;
        hasTitle = true;
    }
    if (hasTitle) {
        std::cout << "Enter the title of the menu: ";
        std::cin >> title;
        std::cout << std::endl;
    }
    std::cout << "Enter the lines/options of the menu (or S/s for \
save and quit): " << std::endl;
    menuSetTitle(readString());
    for (int i = 0; i < 10; i++) {
        std::cout << "Line " << i + 1 << ": ";
        menuOptions[i] = readString();
        if (menuOptions[i] == "S" || menuOptions[i] == "s") {
            menuOptions[i] = "0";
            break;
        }
    }
    menuCalculateTitleAndOptionLengths();
    menuCalculateAmountOfOptions();   
}
// MENU CREATION END

void menu::menuCreateVector() {
    if (hasTitle) {
        boxHeight = amountOfOptions + 8;
    } else {
        boxHeight = amountOfOptions + 4;
    }
    boxWidth = longestTitleOrOptionLength + 8;
    // Create spacing:
    for (int i = 0; i < boxHeight; i++) {
        for (int j = 0; j < boxWidth; j++) {
            menuVector[i][j] = spacingsymbol;
        }
    }
    // Create the top line:
    for (int j = 0; j < boxWidth; j++) {
        menuVector[0][j] = horizontalsymbol;
    }
    // Create the bottom line:
    for (int j = 0; j < boxWidth; j++) {
        menuVector[boxHeight-1][j] = horizontalsymbol;
    }
    // Create the left line:
    for (int i = 0; i < boxHeight; i++) {
        menuVector[i][0] = verticalsymbol;
    }
    // Create the right line:
    for (int i = 0; i < boxHeight; i++) {
        menuVector[i][boxWidth-1] = verticalsymbol;
    }
    // Create the corners:
    menuVector[0][0] = cornersymbol;
    menuVector[0][boxWidth-1] = cornersymbol;
    menuVector[boxHeight-1][0] = cornersymbol;
    menuVector[boxHeight-1][boxWidth-1] = cornersymbol;

    // Create the title and options:
    if (hasTitle) {
        for (int j = 0; j < menuTitleLength; j++) {
            menuVector[2][indentationCalculate(boxWidth, menuTitleLength) + j] = title[j];
        }
        for (int j = 0; j < boxWidth; j++) {
            menuVector[4][j] = horizontalsymbol;
        }
        menuVector[4][0] = cornersymbol;
        menuVector[4][boxWidth-1] = cornersymbol;

        for (int i = 0; i < amountOfOptions; i++) {
            for (int j = 0; j < menuOptionLength[i]; j++) {
                menuVector[6 + i][indentationCalculate(boxWidth, menuOptionLength[i]) + j] = menuOptions[i][j];
            }
        }


    } else { 
        for (int i = 0; i < amountOfOptions; i++) {
            for (int j = 0; j < menuOptionLength[i]; j++) {
                menuVector[2 + i][indentationCalculate(boxWidth, menuOptionLength[i]) + j] = menuOptions[i][j];
            }
        }
    }
}

void menu::menuPrint() {
    for (int i = 0; i < boxHeight; i++) {
        for (int j = 0; j < boxWidth; j++) {
            std::cout << menuVector[i][j];
        }
        std::cout << std::endl;
    }
}


// GETTERS:
int menu::menuGetBoxHeight() {
    return boxHeight;
}
int menu::menuGetBoxWidth() {
    return boxWidth;
}
char menu::menuGetVector(int X, int Y) {
    return menuVector[X][Y];
}
// GETTERS END