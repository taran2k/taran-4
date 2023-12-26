// file hoofd.cpp (main)
// if you're reading this, you're a nerd

// Creator: Taran van Ess
// Date: 26/10/2023
// Version: 1.0.0
// Purpose: Game in which two players take turns placing
// pieces on a board. The first player to connect
// a certain amount of pieces wins. You can play alone,
// together, or watch two computers play against eachother.
// Compiler used: g++ (Ubuntu 11.4.0-1ubuntu1~22.04)
// Compile: make 

#include <iostream>
#include <string>
#include "basicfunctions.h"
#include "game.h"
#include "gobord.h"
#include "GlobalData.h"
#include "namespacealiases.h"

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

// Constructs and prints the 'setupgomoku' menugroup.
int SetUpGomoku(int &m, int &n, int &h) {

    int max = 3;
    int temp[max] = {0};
    int i = 0;


    // For each menu in the menugroup, print it and read an int input.
    do {
        std::cout << std::endl;
        std::cout << std::endl;
        std::cout << " SET UP GAME" << std::endl;
        if (i == 0)
        {
            std::cout << "[Height]" << std::endl;
            std::cout << "Width" << std::endl;
            std::cout << "Connect" << std::endl;
        }
        if (i == 1)
        {
            std::cout << "Height: " << temp[0] << std::endl;
            std::cout << "[Width]" << std::endl;
            std::cout << "Connect" << std::endl;
        }
        if (i == 2)
        {
            std::cout << "Height: " << temp[0] << std::endl;
            std::cout << "Width: " << temp[1] << std::endl;
            std::cout << "[Connect]" << std::endl;
        }

        std::cout << std::endl;
        std::cout << std::endl;
        std::cout << "Value: ";

        // Read an int input and check if it's valid. If not, 
        // read again until it is.
        int input = bsf::readint(gd::MIN, gd::MAX);
        while (input == -1) {
            std::cout << "Invalid input, try again: ";
            input = bsf::readint(gd::MIN, gd::MAX);
            std::cout << std::endl;
        }

        // Store the input in the temp array.
        temp[i] = input;
        i++;
    } while (i < max);
    
    // Ask for confirmation of input
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << " SET UP GAME" << std::endl;
    std::cout << "Height: " << temp[0] << std::endl;
    std::cout << "Width: " << temp[1] << std::endl;
    std::cout << "Connect: " << temp[2] << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;

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


void printAISettings(std::string diffstring[], int AI)
{
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << " CHOOSE AI DIFFICULTY" << std::endl;
    std::cout << "COM1 difficulty ";
    if (AI == 0) 
    {
        std::cout << "([R]andom / [H]ard)" << std::endl;
    }
    else
    {
        std::cout << "(" << diffstring[0] << ")" << std::endl;
    }
    if (AI == 1) 
    {
        std::cout << "COM2 difficulty ";
        std::cout << "([R]andom / [H]ard)" << std::endl;
    }
    else if (AI > 1)
    {
        std::cout << "COM2 difficulty ";
        std::cout << "(" << diffstring[1] << ")" << std::endl;
    }
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << "NOTE: Using HARD AI on larger boards";
    std::cout << " (> 15x15) or with higher connect values";
    std::cout << " (> 5) may cause the program to run";
    std::cout << " slowly." << std::endl;

}

// Constructs and prints the 'aisettings' menugroup.
int AISettings(int &AI1, int &AI2) {
    std::string diffstring[2];
    int max = 2;
    int temp[2] = {0};
    int AI = 0;

    // For each menu in the menugroup, print it and read an int input.
    do {
        bool vPressed = false;

        do {
            printAISettings(diffstring, AI);
            std::cout << "Enter an option: ";
            char option = bsf::readchr();
            
            // If the user pressed h/H or r/R, set the AI difficulty
            // for the current AI accordingly, set vPressed to true
            // and continue.
            if ((option == 'h' || option == 'H'))
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

            if (temp[AI] == 1) 
            {
                diffstring[AI] = "RANDOM";
            } 
            else 
            {
                diffstring[AI] = "HARD";
            }

            AI++;
            continue;
        }

    } while (AI < max);

    // Print the menugroup and ask the user if they're happy with 
    // their input.
    printAISettings(diffstring, AI);

    if (IsHappy()) 
    {
        AI1 = temp[0], AI2 = temp[1];
    }
    else 
    {
        AISettings(AI1, AI2);
    }
    
    return 0;

}

// Constructs and prints the 'gamemodes' menugroup.
int Gamemodes(int &gm) {
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << " CHOOSE A GAMEMODE" << std::endl;
    std::cout << "[S]ingleplayer" << std::endl;
    std::cout << "[M]ultiplayer" << std::endl;
    std::cout << "[C]omputer" << std::endl;
    std::cout << std::endl;
    if (gd::GAMES_PLAYED < 100) 
    {
        std::cout << "[-] Quit Game (unlocks after "; 
        std::cout << 100 - gd::GAMES_PLAYED << " games)" << std::endl;
    } 
    else 
    {
        std::cout << "[Q]uit Game" << std::endl;
    }
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << "Enter a gamemode: ";

    char option = bsf::readchr();

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
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << " QUICK START" << std::endl;
    std::cout << "[T]ic-Tac-Toe" << std::endl;
    std::cout << "[C]onnect Four" << std::endl;
    std::cout << "[G]omoku (custom)" << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << "Enter a game: ";
    
    char option = bsf::readchr();

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
        SetUpGomoku(m, n, h); 
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
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << " CHOOSE AMOUNT OF GAMES TO PLAY" << std::endl;
    std::cout << "Enter an amount of games the AI should simulate";
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << "Enter an amount: ";

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
    std::cout << std::endl;
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
            gtp = AIGames();
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

    // Print the statistics menu if more than 1 game was played.
    if (gtp > 1) 
    {
        std::cout << std::endl;
        std::cout << "GAMES WON PER TURN HISTOGRAM" << std::endl;
        Histogram(games_won_per_turn, m, n);
        Statistics(m, n, gtp, games_won_per_turn);
    }

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

    // Infoblokje
    std::cout << "+-------------------------+" << std::endl;
    std::cout << "|     Opg. IV: Gomoku     |" << std::endl;
    std::cout << "+-------------------------+" << std::endl;
    std::cout << "| By: Taran van Ess '23   |" << std::endl;
    std::cout << "| Study: Wis+Informatica  |" << std::endl;
    std::cout << "| Date: 26/12/2023        |" << std::endl;
    std::cout << "| Student: 4125185        |" << std::endl;
    std::cout << "+-------------------------+" << std::endl;
    std::cout << "This program is a game in which two players take";
    std::cout << " turns placing pieces on a board." << std::endl;
    std::cout << "The first player to connect a certain amount of"; 
    std::cout << " pieces wins." << std::endl;
    std::cout << "You can play alone, together, or watch two computers";
    std::cout << " play against eachother." << std::endl;
    std::cout << std::endl;  
    std::cout << "Enter some key to start. ";
    bsf::readchr();

    do {
        int m, n, h, gm;
        int AI1 = 0; int AI2 = 0;

        Gamemodes(gm);
        Quickstart(m, n, h, gm);
        if (gm == 2)
        {
            AISettings(AI1, AI2);
        }

        ItsGoTime(m, n, h, gm, AI1, AI2);

    } while (!Quit());


    return 0;
}