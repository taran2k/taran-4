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

// Resets 'CURRENT_MENU' to 0 and calls function 'func'
// with arguments 'args'.
template<typename... Args>
int CallAndReset(int (*func)(Args&...), Args&... args) {
    gd::CURRENT_MENU = 0;
    return func(args...);
}


// Reads a char c and navigates the menu accordingly.
// Returns '\0' for successful navigation, otherwise returns c back.
char navigate(char c, menugroup &mg, bool nav = true) {

    // If c is the key for the compact mode, toggle compact mode.
    if (c == mg.groupGetCompactModeKey()) 
    {
        if (gd::VIEW != 1) {
            gd::VIEW = 1;
        } else {
            gd::VIEW = gd::DEFAULT_VIEW;
        }
        mg.print(gd::VIEW);
    } 
    // If c is the key for the navigation mode, toggle navigation mode.
    else if (c == mg.groupGetNavigationModeKey()) 
    {
        if (gd::VIEW != 2) {
            gd::VIEW = 2;
        } else {
            gd::VIEW = gd::DEFAULT_VIEW;
        }
        mg.print(gd::VIEW);

    } 
    // If c is a navigation key and navigation mode is on, navigate.
    else if (gd::VIEW == 2 && nav &&
    (c == mg.groupGetNavLeftKey() || c == mg.groupGetNavRightKey())) 
    {
        if (c == mg.groupGetNavLeftKey() 
        && gd::CURRENT_MENU > 0) {
            mg.print(gd::VIEW, --gd::CURRENT_MENU);
        } 
        else if (c == mg.groupGetNavRightKey()
        && gd::CURRENT_MENU < (mg.groupGetMenuCount() - 1)) {
            mg.print(gd::VIEW, ++gd::CURRENT_MENU);
        } 
    } 
    // If c is not a navigation key, return c.
    else 
    {
        return c;
    }    

    // Otherwise, return '\0'.
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

// Bool function that asks the user if they're happy with their input
// and returns true if they input y/Y, false if they input n/N.
bool IsHappy() {
    char happy;
    std::cout << "Happy? (Y/n) ";
    happy = bsf::readchr();

    if (happy == 'n' || happy == 'N') 
    {
        return false;
    } 
    else if (happy == 'y' || happy == 'Y') 
    {
        return true;
    } 

    // If the input is invalid, call IsHappy() again.
    else 
    {
        std::cout << "Invalid input, try again." << std::endl;
        return IsHappy();
    }

    return false;
}

// Prints the menugroup 'mg' and reads a char option.
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

// Unlocks the quitting option in the 'exit' menu.
void UnlockQuitting(menu &exit) 
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

// Locks the quitting option in the 'exit' menu.
void LockQuitting(menu &exit)
{
    exit.menuSetOptions(4, 
    std::to_string(100 - gd::GAMES_PLAYED) + " games--");

    exit.menuSetCompactOption("[-] Quit Game (unlocks after " 
    + std::to_string(100 - gd::GAMES_PLAYED) + " games)");
}

// Constructs and prints the 'setupgomoku' menugroup.
int SetUpGomoku(int &m, int &n, int &h) {
    menugroup gomoku("menus/setupgomoku.cfg");
    int max = gomoku.groupGetMenuCount(); // 3
    int temp[max] = {0};
    int c = 0; // <- offset for default view

    if (gd::VIEW == 0) {
        c = 1;  // otherwise the first print contains
                // no menus for the default view. (see print() docs)
    }

    // For each menu in the menugroup, print it and read an int input.
    for (int i = 0 + c; i < max + c; i++) {
        menu &currentmenu = gomoku.groupGetMenu(i-c)->m;
        std::string currentco = currentmenu.menuGetCompactOption();

        // Set the title and options of the current menu to indicate
        // that the user should enter an integer.
        currentmenu.menuSetOptions(1, "[..]");
        currentmenu.menuSetCompactOption("[" + currentco + "]");
        currentmenu.menuSetHasTitle(true);

        // Print the menugroup. For default view, print all menus
        // up until the current one. For navigation mode, print only the
        // current menu.
        gomoku.print(gd::VIEW, i, false);

        // Read an int input and check if it's valid. If not, 
        // read again until it is.
        int input = bsf::readint(gd::MIN, gd::MAX);
        while (input == -1) {
            std::cout << "Invalid input, try again: ";
            input = bsf::readint(gd::MIN, gd::MAX);
            std::cout << std::endl;
        }

        // Store the input in the temp array.
        temp[i-c] = input;

        // Set the title and options of the current menu to indicate
        // that the user has entered an integer.
        currentmenu.menuSetOptions(1, std::to_string(temp[i-c]));
        currentmenu.menuSetCompactOption(
            currentco + ": " + std::to_string(temp[i-c])
        );
        currentmenu.menuSetHasTitle(false);

        // If the view is navigation mode, print the menu again
        // to show the changes.
        if (gd::VIEW == 2) {
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

    // Ask the user if they're happy with their input.
    // If they are, set the board dimensions and connect accordingly.
    if (IsHappy()) {
        m = temp[0], n = temp[1], h = temp[2];
    } 
    // If not, call SetUpGomoku() again.
    else 
    {
        SetUpGomoku(m, n, h);
    }

    return 0;
}

void AISettingsSetMenu(menu &currentmenu, int AI)
{
    currentmenu.menuSetTitle("COM" + std::to_string(AI+1) 
    + "     < HARD >");
    currentmenu.menuSetOptions(0, "Use arrows < to change >");
    currentmenu.menuSetOptions(1, "Or press [H] or [R]");
    currentmenu.menuSetOptions(2, "Press [V] to confirm");

    currentmenu.menuSetCompactOption("COM" 
    + std::to_string(AI+1) + " difficulty ([R]andom / [H]ard)");
}

// Constructs and prints the 'aisettings' menugroup.
int AISettings(int &AI1, int &AI2) {
    menugroup aisettings("menus/aisettings.cfg");
    int max = aisettings.groupGetMenuCount(); // 2
    int temp[max] = {0};
    int AI = 0;

    // For each menu in the menugroup, print it and read an int input.
    do {
        bool vPressed = false;
        menu &currentmenu = aisettings.groupGetMenu(AI)->m;
        AISettingsSetMenu(currentmenu, AI);

        do {
            // Initialize the menu and print it.
            gd::CURRENT_MENU =  0;
            currentmenu.menuCalculateTitleAndOptionLengths();
            currentmenu.menuCalculateAmountOfOptions();
            char option = PrintAndInteract(aisettings);

            // If the user pressed v/V, set vPressed to true and
            // continue 
            if (option == 'v' || option == 'V') 
            {
                vPressed = true;
                continue;
            } 

            // If the user pressed < or >, toggle the AI difficulty
            // for the current AI.
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
            
            // If the user pressed h/H or r/R, set the AI difficulty
            // for the current AI accordingly, set vPressed to true
            // and continue.
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
        } while (!vPressed);

        // If the user pressed v/V, set the menu title and options
        if (vPressed)
        {
            std::string diffstring;

            currentmenu.menuSetTitle("COM" + std::to_string(AI+1));

            if (temp[AI] == 1) 
            {
                diffstring = "RANDOM";
            } 
            else 
            {
                diffstring = "HARD";
            }

            // Set the menu title and options accordingly.
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

    // Print the menugroup and ask the user if they're happy with 
    // their input.
    aisettings.print(gd::VIEW, max, false, false);

    if (IsHappy()) 
    {
        AI1 = temp[0], AI2 = temp[1];
    }
    else 
    {
        CallAndReset(AISettings, AI1, AI2);
    }
    
    return 0;

}

// Constructs and prints the 'gamemodes' menugroup.
int Gamemodes(int &gm) {
    menugroup gamemodes("menus/gamemodes.cfg");
    menu &exit = gamemodes.groupGetMenu(3)->m;

    // If the player has played 100 games, unlock quitting.
    if (gd::GAMES_PLAYED >= 100) 
    {   
        UnlockQuitting(exit);
    } 
    else 
    {
        LockQuitting(exit);
    }

    exit.menuCalculateAmountOfOptions();
    exit.menuCalculateTitleAndOptionLengths();

    // Print the gamemodes menu and read a char option.
    char option = PrintAndInteract(gamemodes);

    // Set the gamemode accordingly.
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

    // If no valid option was selected, call Gamemodes() again.
    else {
        Gamemodes(gm);
    }

    return 0;
}

// Constructs and prints the 'quickstart' menugroup.
int Quickstart(int &m, int &n, int &h, int &gm) {
    menugroup quickstart("menus/quickstart.cfg");

    // Print the quickstart menu and read a char option.
    char option = PrintAndInteract(quickstart);

    // Set the board dimensions and connect accordingly.
    if (option == 'T' || option == 't') 
    {
        m = 3, n = 3, h = 3;
    } 
    else if (option == 'C' || option == 'c') 
    {
        m = 6, n = 7, h = 4;
    } 
    else if (option == 'G' || option == 'g') 
    {
        CallAndReset(SetUpGomoku, m, n, h); 
    } 

    // If no valid option was selected, call Quickstart() again.
    else 
    {
        Quickstart(m, n, h, gm);
    }

    return 0;
}

// Constructs and prints the 'aigames' menugroup.
int AIGames() {
    menugroup aigames("menus/aigames.cfg");
    aigames.print(gd::VIEW);

    // Read an int input between 0 and LARGE_NUM_MAX.
    int input = bsf::readint(0, gd::LARGE_NUM_MAX);

    // If the input is over 100, warn the user.
    if (input > 100)
    {
        std::cout << "Be wary that simulating a large amount";
        std::cout << " of games may be computationally intensive";
        std::cout << std::endl;
    }

    // Ask the user if they're happy with their input.
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

int DetailedHistogram(int games_won_per_turn[], int m, int n) {
    for (int i = 0; i < bsf::ceil((m * n) / 10.0); i++)
    {
        std::cout << i << ": ";
        for (int j = 0; j < 10; j++)
        {
            for (int k = 0; k < games_won_per_turn[i*10+j]; k++)
            {
                std::cout << j << " ";
            }
        }
        std::cout << std::endl;
    }

    return 0;
}

int Histogram(int games_won_per_turn[], int m, int n) {
    bool print = false;
    for (int i = 0; i < (m * n); i++)
    {
        if (games_won_per_turn[i] > 0)
        {
            print = true;
        }
        else
        {
            continue;
        }

        if (print)
        {
            if (i < 10)
            {
                std::cout << " ";
            }
            if (i < 100)
            {
                std::cout << " ";
            }
            std::cout << i << "| ";
            for (int j = 0; j < games_won_per_turn[i]; j++)
            {
                std::cout << ":";
            }
            std::cout << std::endl;
        }
    }

    return 0;
}

// Returns the amount of games won by player 1. (Every game ending
// on an even turn is won by player 1.)
int GamesWonByPlayer1(int m, int n, int games_won_per_turn[]) {
    int games_won = 0;

    for (int i = 0; i < m * n; i++)
    {
        if (i % 2 == 0)
        {
            games_won += games_won_per_turn[i];
        }
    }

    return games_won;
}

// Returns the amount of games won by player 2. (Every game ending
// on an odd turn is won by player 2.)
int GamesWonByPlayer2(int m, int n, int games_won_per_turn[]) {
    int games_won = 0;

    for (int i = 0; i < m * n; i++)
    {
        if (i % 2 != 0)
        {
            games_won += games_won_per_turn[i];
        }
    }

    return games_won;
}

int Statistics(int m, int n, int gtp, int games_won_per_turn[])
{
    int p1wins = GamesWonByPlayer1(m, n, games_won_per_turn);
    int p2wins = GamesWonByPlayer2(m, n, games_won_per_turn);
    int ties = gtp - p1wins - p2wins;

    std::cout << "Statistics" << std::endl;
    std::cout << "Games played: " << gtp << std::endl;
    std::cout << "Games won by player 1: " << p1wins;
    std::cout << std::endl;
    std::cout << "Games won by player 2: " << p2wins;
    std::cout << std::endl;
    std::cout << "Games tied: " << ties << std::endl;
    std::cout << "Player 1 winrate: " << p1wins * 100.0 
    / gtp << "%" << std::endl;
    std::cout << "Player 2 winrate: " << p2wins * 100.0 
    / gtp << "%" << std::endl;
    std::cout << "Tie rate: " << ties * 100.0 / gtp 
    << "%" << std::endl;

    return 0;

}

// Constructs and prints the 'itsgotime' menugroup.
int ItsGoTime(int &m, int &n, int &h, int &gm, int &AI1, int &AI2) {
    int gtp = 1;    // <- games to play

    // If the gamemode is 2 (AI vs AI), ask the user how many games
    // they want to simulate.
    if (gm == 2)
    {
        do
        {
            gtp = CallAndReset(AIGames);
        } while (gtp == -1);
    }

    // For each game to play, construct a game and play it.
    int games_won_per_turn[m*n] = {0};
    for (int i = 0; i < gtp; i++)
    {
        Game g(m, n, h, gm, AI1, AI2);
        int endturn = g.play();
        gd::GAMES_PLAYED++;
        games_won_per_turn[endturn]++;
    }

    // Print the statistics menu.
    std::cout << std::endl;
    std::cout << "GAMES WON PER TURN HISTOGRAM" << std::endl;
    Histogram(games_won_per_turn, m, n);
    Statistics(m, n, gtp, games_won_per_turn);

    return 0;
}

// Lets the user quit the game.
int Quit() {
    // If the user hasn't played 100 games yet, 
    // return to the main menu.
    if (gd::GAMES_PLAYED < 100) 
    {
        std::cout << "Enter any key to return to the main menu. ";
        bsf::readchr();
        return 0;
    }

    // Otherwise, let the user know they've unlocked quitting.
    // Ask them if they want to quit.
    std::cout << "You've unlocked quitting.";
    std::cout << " Do you want to quit? (Y/n) ";
    std::cout << std::endl;
    char option = bsf::readchr();

    if (option == 'y' || option == 'Y') 
    {
        return 1;
    }
    else if (option == 'n' || option == 'N') 
    {
        return 0;
    } 
    else 
    {
        return Quit();
    }
}


 /* This function is the entry point of the program. 
 * It initializes variables, calls other functions, 
 * and runs a loop until the user chooses to quit.
 */
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