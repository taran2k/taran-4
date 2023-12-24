#include <iostream>
#include <string.h> //pas later aan
#include <string>
#include "menus.h"
#include "basicfunctions.h"
#include "GlobalData.h"
#include "namespacealiases.h"

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
    amountOfOptions = 0;
    hasTitle = true;
    title = "Empty menu!";
    menuTitleLength = 0;
    menuTitleBoxPadding = 0;
    titleBoxHeight = 0;
    for (int i = 0; i < 10; i++) { 
        menuOptions[i] = "\0";
        menuOptionLength[i] = 0;
    }
    compact_option = '\0';
    longestTitleOrOptionLength = 0;
    boxHeight = 0;
    boxWidth = 0;
    for (int i = 0; i < 100; i++) { 
        for (int j = 0; j < 100; j++) {
            menuVector[i][j] = '\0';
        }
    }
}

menu::~menu() {}

// SETTERS:
void menu::menuSetCornerSymbol(char C) {
    cornersymbol = C;
}
void menu::menuSetHorizontalSymbol(char C) {
    horizontalsymbol = C;
}
void menu::menuSetVerticalSymbol(char C) {
    verticalsymbol = C;
}
void menu::menuSetSpacingSymbol(char C) {
    spacingsymbol = C;
    if (spacingsymbol == '\0') {
        spacingsymbol = ' ';
    }
}
void menu::menuSetHasTitle(bool HASTITLE) {
    hasTitle = HASTITLE;
}
void menu::menuSetTitle(std::string TITLE) {
    if (hasTitle) {
        title = TITLE;
    } 
    menuTitleLength = title.length();
}
void menu::menuSetTitleBoxPadding(int PADDING) {
    menuTitleBoxPadding = PADDING;
}

// Set a specific option to a new value. OPTIONNUMBER starts counting
// from 0, so give one value lower than x for the x-th option.
void menu::menuSetOptions(int OPTIONNUMBER, std::string OPTION) {
    if (OPTIONNUMBER < 10) {
        menuOptions[OPTIONNUMBER] = OPTION;
    } else {
        std::cout << "Invalid option number." << std::endl;
    }
    menuOptionLength[OPTIONNUMBER] = OPTION.length();
}
void menu::menuSetOptionsAmount(int AMOUNTOFOPTIONS) {
    amountOfOptions = AMOUNTOFOPTIONS;
}

void menu::menuSetCompactOption(std::string OPTION) {
    compact_option = OPTION;
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
        if (menuOptions[i] != "\0") {
            amountOfOptions++;
        }
    }
}

// FUNCTIONS END

/* MENU CREATION (FOR TESTING):
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
*/

// REAL MENU CREATION:
void menu::menuCreateVector() {
    // empty any existing vector
    for (int i = 0; i < 100; i++) {
        for (int j = 0; j < 100; j++) {
            menuVector[i][j] = '\0';
        }
    }

    if (hasTitle) {
        titleBoxHeight = menuGetTitleBoxHeight();
    } else {
        titleBoxHeight = 0;
    }
    boxHeight = amountOfOptions + titleBoxHeight;
    if (amountOfOptions > 0) {
        boxHeight += 4;
    } else {
        boxHeight += 1;
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
            menuVector[1+menuTitleBoxPadding][bsf::calcind(boxWidth, menuTitleLength) + j] = title[j];
        }
        for (int j = 0; j < boxWidth; j++) {
            menuVector[2+2*menuTitleBoxPadding][j] = horizontalsymbol;
        }
        menuVector[2+2*menuTitleBoxPadding][0] = cornersymbol;
        menuVector[2+2*menuTitleBoxPadding][boxWidth-1] = cornersymbol;
    }
    if (amountOfOptions > 0) {
        for (int i = 0; i < amountOfOptions; i++) {
            for (int j = 0; j < menuOptionLength[i]; j++) {
                menuVector[2 + titleBoxHeight + i]
                [bsf::calcind(boxWidth, menuOptionLength[i]) + j] 
                = menuOptions[i][j];
            }
        }
    }
}
// REAL MENU CREATION END

void menu::menuSetUp(char corner_symbol, char horizontal_symbol,
 char vertical_symbol, char spacing_symbol, const char* title, 
 int title_padding, char options[][MAX], std::string compact_option, 
 int numOptions) {
    menuSetCornerSymbol(corner_symbol);
    menuSetHorizontalSymbol(horizontal_symbol);
    menuSetVerticalSymbol(vertical_symbol);
    menuSetSpacingSymbol(spacing_symbol);
    menuSetHasTitle(title[0] != '\0');
    menuSetTitle(title);
    menuSetTitleBoxPadding(title_padding);

    for (int i = 0; i < numOptions; ++i) {
        std::string optionStr(options[i]);
        menuSetOptions(i, optionStr);
    }
    this->compact_option = compact_option;

    menuCalculateAmountOfOptions();
    menuCalculateTitleAndOptionLengths();
    menuCreateVector();
}

// PRINT MENU:
void menu::menuPrint() {
    for (int i = 0; i < boxHeight; i++) {
        for (int j = 0; j < boxWidth; j++) {
            std::cout << menuVector[i][j];
        }
        std::cout << std::endl;
    }
}
// PRINT MENU END

// GETTERS:
bool menu::menuGetHasTitle() {
    return hasTitle;
}
std::string menu::menuGetTitle() {
    return title;
}
int menu::menuGetTitleBoxHeight() {
    return (2 + 2 * menuTitleBoxPadding);
}
int menu::menuGetTitleSpacing() {
    return menuTitleBoxPadding;
}
int menu::menuGetBoxHeight() {
    return boxHeight;
}
int menu::menuGetBoxWidth() {
    return boxWidth;
}
int menu::menuGetAmountOfOptions() {
    return amountOfOptions;
}
std::string menu::menuGetOption(int i) {
    return menuOptions[i];
}
std::string menu::menuGetCompactOption() {
    return compact_option;
}
char menu::menuGetVector(int X, int Y) {
    return menuVector[X][Y];
}
// GETTERS END
