// file hoofd.cpp (main)
#include <iostream>
#include <string>
#include "basicfunctions.h"
#include "game.h"
#include "gobord.h"
#include "menus.h"
#include "menugroups.h"
#include "GlobalData.h"
#include "namespacealiases.h"

// Resets 'CURRENT_MENU' to 0 and calls function 'func'.
template<typename... Args>
int CallAndReset(int (*func)(Args&...), Args&... args) {
    gd::CURRENT_MENU = 0;
    return func(args...);
}


// Reads a char c and navigates the menu accordingly.
// Returns '\0' for successful navigation, otherwise returns c back.
char navigate(char c, menugroup &mg, bool nav = true) {

    if (c == mg.groupGetCompactModeKey()) {
        if (gd::VIEW != 1) {
            gd::VIEW = 1;
        } else {
            gd::VIEW = gd::DEFAULT_VIEW;
        }
        mg.print(gd::VIEW);

    } else if (c == mg.groupGetNavigationModeKey()) {
        if (gd::VIEW != 2) {
            gd::VIEW = 2;
        } else {
            gd::VIEW = gd::DEFAULT_VIEW;
        }
        mg.print(gd::VIEW);

    } else if (gd::VIEW == 2 && nav &&
    (c == mg.groupGetNavLeftKey() || c == mg.groupGetNavRightKey())) {
        if (c == mg.groupGetNavLeftKey() 
        && gd::CURRENT_MENU > 0) {
            mg.print(gd::VIEW, --gd::CURRENT_MENU);
        } 
        else if (c == mg.groupGetNavRightKey()
        && gd::CURRENT_MENU < (mg.groupGetMenuCount() - 1)) {
            mg.print(gd::VIEW, ++gd::CURRENT_MENU);
        } 
    } else {
        return c;
    }    

    return '\0';
}

// While o is '\0', read a char c and pass it to navigate() to
// navigate within the current menugroup 'mg'.
// 'nav' is an optional parameter that decides whether char c 
// should be used for navigation or not. True by default.
char readoption(menugroup &mg, bool nav = true) {
    char o = '\0';
    do {
        char c = bsf::readchr();
        o = navigate(c, mg, nav);
    } while (o == '\0');

    return o;
}

bool IsHappy() {
    char happy;
    std::cout << "Happy? (Y/n) ";
    happy = bsf::readchr();
    if (happy == 'n' || happy == 'N') {
        return false;
    } else if (happy == 'y' || happy == 'Y') {
        return true;
    } else {
        std::cout << "Invalid input, try again." << std::endl;
        return IsHappy();
    }
}

char PrintAndInteract(menugroup &mg) {
    char option;
    int menus;

    if (gd::VIEW == 2)
    {
        menus = gd::CURRENT_MENU;
    }
    else 
    {
        menus = -1;
    }
    mg.print(gd::VIEW, menus);
    option = readoption(mg);

    return option;
}


int SetUpGomoku(int &m, int &n, int &h) {
    menugroup gomoku("menus/setupgomoku.cfg");
    int max = gomoku.groupGetMenuCount(); // 3
    int temp[max] = {0};
    int c = 0;

    if (gd::VIEW == 0) {
        c = 1;  // otherwise the first print contains
                // no menus for the default view. (see print() docs)
    }
    for (int i = 0 + c; i < max + c; i++) {
        menu &currentmenu = gomoku.groupGetMenu(i-c)->m;
        std::string currentco = currentmenu.menuGetCompactOption();

        currentmenu.menuSetOptions(1, "[..]");
        currentmenu.menuSetCompactOption("[" + currentco + "]");
        currentmenu.menuSetHasTitle(true);
        gomoku.print(gd::VIEW, i, false);
        int input = bsf::readint(gd::MIN, gd::MAX);
        while (input == -1) {
            std::cout << "Invalid input, try again: ";
            input = bsf::readint(gd::MIN, gd::MAX);
            std::cout << std::endl;
        }
        temp[i-c] = input;
        currentmenu.menuSetOptions(1, std::to_string(temp[i-c]));
        currentmenu.menuSetCompactOption(
            currentco + ": " + std::to_string(temp[i-c])
        );
        currentmenu.menuSetHasTitle(false);
        if (gd::VIEW == 2) {
            // If the view is navigation mode, print the menu again
            // to show the changes.
            gomoku.print(gd::VIEW, i, false);
        }
    }
    gomoku.print(gd::VIEW, max + c, false, false);

    // Check if connect is higher than the board dimensions.
    if (temp[2] > temp[0] && temp[2] > temp[1] 
    && temp) { // no need to calculate the max diagonal, 
               // since it always equals highest dimension.
        std::cout << "Note: connect is higher than the board";
        std::cout << " dimensions, meaning the game will be"; 
        std::cout << " unwinnable." << std::endl;
    }
    if (IsHappy()) {
        m = temp[0], n = temp[1], h = temp[2];
    } else {
        SetUpGomoku(m, n, h);
    }

    return 0;
}

int AISettings(int &AI1, int &AI2) {
    menugroup aisettings("menus/aisettings.cfg");
    int max = aisettings.groupGetMenuCount(); // 2
    int temp[max] = {0};
    int AI = 0;

    do {
        bool vPressed = false;
        menu &currentmenu = aisettings.groupGetMenu(AI)->m;
        currentmenu.menuSetTitle("COM" + std::to_string(AI+1) 
        + "     < HARD >");
        currentmenu.menuSetOptions(0, "Use arrows < to change >");
        currentmenu.menuSetOptions(1, "Or press [H] or [R]");
        currentmenu.menuSetOptions(2, "Press [V] to confirm");

        currentmenu.menuSetCompactOption("COM" 
        + std::to_string(AI+1) + " difficulty ([R]andom / [H]ard)");

        do {
            currentmenu.menuCalculateTitleAndOptionLengths();
            currentmenu.menuCalculateAmountOfOptions();
            gd::CURRENT_MENU =  0;
            char option = PrintAndInteract(aisettings);

            if (option == 'v' || option == 'V') 
            {
                vPressed = true;
                continue;
            } 

            else if ((option == '<' || option == '>') && gd::VIEW != 1)
            {
                temp[AI] = 1 - temp[AI];

                if (temp[AI] == 1) 
                {
                    currentmenu.menuSetTitle("COM" 
                    + std::to_string(AI+1) + "     < RANDOM >");
                } 
                else 
                {
                    currentmenu.menuSetTitle("COM" 
                    + std::to_string(AI+1) + "     < HARD >");
                }
            }
            
            else if ((option == 'h' || option == 'H'))
            {
                temp[AI] = 0;
                vPressed = true;
                continue;
            }

            else if ((option == 'r' || option == 'R'))
            {
                temp[AI] = 1;
                vPressed = true;
                continue;
            }

            else 
            {
                continue;
            }
        } while (!vPressed);

        if (vPressed)
        {
            std::string diffstring;

            currentmenu.menuSetTitle("COM" + std::to_string(AI+1));

            if (temp[AI] == 1) 
            {
                diffstring = "RANDOM";
            } else 
            {
                diffstring = "HARD";
            }

            currentmenu.menuSetOptions(0, diffstring);
            currentmenu.menuSetOptions(1, "");
            currentmenu.menuSetOptions(2, "");
            currentmenu.menuSetCompactOption("COM" + std::to_string(AI+1) 
                            + " difficulty (" + diffstring + ")");
            currentmenu.menuCalculateTitleAndOptionLengths();
            currentmenu.menuCalculateAmountOfOptions();

            AI++;
            continue;
        }

    } while (AI < max);

    aisettings.print(gd::VIEW, max, false, false);

    if (IsHappy()) {
        AI1 = temp[0], AI2 = temp[1];
    } else {
        CallAndReset(AISettings, AI1, AI2);
    }
    
    return 0;

}

int Gamemodes(int &gm) {
    menugroup gamemodes("menus/gamemodes.cfg");
    menu &exit = gamemodes.groupGetMenu(3)->m;

    if (gd::GAMES_PLAYED >= 100) 
    {   
        exit.menuSetTitle("EXIT");
        exit.menuSetOptions(0, "[Q]uit Game");
        exit.menuSetOptions(1, " ");
        exit.menuSetOptions(2, " ");
        exit.menuSetOptions(3, "You've unlocked");
        exit.menuSetOptions(4, "this option");
        exit.menuSetCompactOption("[Q]uit");
        exit.menuSetHorizontalSymbol('=');
    } 
    else 
    {
        exit.menuSetOptions(4, 
        std::to_string(100 - gd::GAMES_PLAYED) + " games--");

        exit.menuSetCompactOption("[-] Quit Game (unlocks after " 
        + std::to_string(100 - gd::GAMES_PLAYED) + " games)");
    }
    exit.menuCalculateAmountOfOptions();
    exit.menuCalculateTitleAndOptionLengths();

    char option = PrintAndInteract(gamemodes);

    if (option == 's' || option == 'S') {
        gm = 0;
    } 
    else if (option == 'm' || option == 'M') 
    {
        gm = 1;
    } 
    else if (option == 'c' || option == 'C') 
    {
        gm = 2;
    }
    else if ((option == 'q' || option == 'Q') && gd::GAMES_PLAYED > 99)
    {
        std::cout << "Quitting..." << std::endl;
        std::exit(0);
    }
    else {
        Gamemodes(gm);
    }

    return 0;
}

// Constructs and prints the 'quickstart' menugroup.
int Quickstart(int &m, int &n, int &h, int &gm) {
    menugroup quickstart("menus/quickstart.cfg");
    char option = PrintAndInteract(quickstart);

    if (option == 'T' || option == 't') {
        m = 3, n = 3, h = 3;
    } else if (option == 'C' || option == 'c') {
        m = 6, n = 7, h = 4;
    } else if (option == 'G' || option == 'g') {
        CallAndReset(SetUpGomoku, m, n, h); 
    } else {
        Quickstart(m, n, h, gm);
    }

    return 0;
}

int AIGames() {
    menugroup aigames("menus/aigames.cfg");
    aigames.print(gd::VIEW);
    int input = bsf::readint(0, gd::LARGE_NUM_MAX);

    if (input > 100)
    {
        std::cout << "Be wary that simulating a large amount";
        std::cout << " of games may be computationally intensive";
        std::cout << std::endl;
    }
    std::cout << "The AI will simulate " << input << " games.";
    std::cout << std::endl;

    if (IsHappy())
    {
        return input;
    }
    else
    {
        return -1;
    }

    return 0;
}


int ItsGoTime(int &m, int &n, int &h, int &gm, int &AI1, int &AI2) {
    int gtp = 1;    // <- games to play

    if (gm == 2)
    {
        do
        {
            gtp = CallAndReset(AIGames);
        } while (gtp == -1);
    }

    for (int i = 0; i < gtp; i++)
    {
        Game g(m, n, h, gm, AI1, AI2);
        g.play();
        gd::GAMES_PLAYED++;
    }

    return 0;
}

int Quit() {

    if (gd::GAMES_PLAYED < 100) 
    {
        std::cout << "Enter any key to return to the main menu. ";
        bsf::readchr();
        return 0;
    }

    std::cout << "Do you want to quit? (Y/n) ";
    std::cout << std::endl;
    char option = bsf::readchr();

    if (option == 'y' || option == 'Y') {
        return 1;
    } else if (option == 'n' || option == 'N') {
        return 0;
    } else {
        return Quit();
    }
}

int main() {
    do {
        int m, n, h, gm;
        int AI1 = 0; int AI2 = 0;

        CallAndReset(Gamemodes, gm);
        CallAndReset(Quickstart, m, n, h, gm);
        if (gm == 2)
        {
            CallAndReset(AISettings, AI1, AI2);
        }

        CallAndReset(ItsGoTime, m, n, h, gm, AI1, AI2);

    } while (!CallAndReset(Quit));


    return 0;
}