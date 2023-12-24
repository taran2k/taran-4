#ifndef GAME_H
#define GAME_H

#include "gobord.h"

// A Move struct that contains the row and column of a move and the
// colour of the player who made the move.
struct Move
{
    int row, column;
    char *colour;
};

// A Move Stack that consists of nodes that contain a move and,
// completely in style, a pointer to the next node in the stack. The
// Move Stack takes one 'in' Node pointer called 'top'.
class MoveStack
{
public:
    MoveStack();
    ~MoveStack();
    void push(Move move);
    bool isEmpty();
    void pop();
    Move *peek();

private:
    struct Node
    {
        Move move;
        Node *next;
    };
    Node *top;
};

struct AIScope
{
    Tile *tilesToCheck[gd::MAX * gd::MAX];
    int tilesCount;
};

// The Game class
class Game
{
public:
    Game();
    Game(int m, int n, int h, int gm, int AI1 = 0, int AI2 = 0);
    ~Game();
    Goboard &getGoboard();
    void switchCurrentPlayer();
    void calculatePermutations();
    char readOptionInput(char c);
    void printMenu();
    void printInput();
    void printGame();
    int getGamemode();
    int getTurn();
    int getCurrentPlayer();
    int getWinThreshold();
    bool isSpecialKey(char c);
    bool doesTileConcludeGame(Tile *placedTile);
    bool isTie();
    void undoPlay();
    void redoPlay();
    Tile *playturn();
    void AISetSearchRadius();
    Tile *AIDefenseDepth1(Tile *skipTile = nullptr);
    Tile *AIDefenseDepth2(Tile *skipTile = nullptr);
    Tile *AIDefenseDepth3(Tile *skipTile = nullptr);
    Tile *AIOffenseDepth1(Tile *skipTile = nullptr);
    Tile *AIOffenseDepth2(Tile *skipTile = nullptr);
    Tile *AIOffenseDepth3(Tile *skipTile = nullptr);
    Tile *AIOffense();
    Tile *AIWin(Tile *skipTile = nullptr);
    Tile *AIBlockWin(Tile *skipTile = nullptr);
    Tile *AISmartPlay();
    Tile *AIRandomPlay();
    Tile *HumanPlay();
    void play();
    void quit();

private:
    Goboard *board;
    MoveStack undoStack;
    MoveStack redoStack;
    AIScope scope;
    bool concluded;
    int AILevel[2]; // AI level of the current player
    int gamemode;
    int turn;
    int current_player;
    int win_threshold;
    long long perm; // last calculated permutations
};

#endif