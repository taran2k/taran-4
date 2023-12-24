#include <iostream>
#include <random>
#include "GlobalData.h"
#include "basicfunctions.h"
#include "namespacealiases.h"
#include "menus.h"
#include "menugroups.h"
#include "gobord.h"
#include "game.h"

MoveStack::MoveStack()
{
    top = nullptr;
}

MoveStack::~MoveStack()
{
    while (top)
    {
        pop();
    }
}

// Push a move onto the stack
void MoveStack::push(Move move)
{
    Node *node = new Node{move, top};
    top = node;
}

// Check if the stack is empty
bool MoveStack::isEmpty()
{
    return top == nullptr;
}

// Pop (delete) a move from the stack and return
// the move that was popped
void MoveStack::pop()
{
    if (isEmpty())
    {
        return;
    }

    Node *temp = top;
    top = top->next;
    delete temp;
}

// Return the move at the top of the stack
Move *MoveStack::peek()
{
    if (isEmpty())
    {
        return nullptr; // return bogus move
    }

    return &(top->move);
}

Game::Game()
{
    // board = new Goboard;
}

Game::Game(int m, int n, int h, int gm, int AI1, int AI2)
{
    board = new Goboard(m, n);
    concluded = false;
    gamemode = gm;
    AILevel[0] = AI1;
    AILevel[1] = AI2; 
    win_threshold = h;
    turn = 1;
    current_player = 1;
    perm = 0;
}

Game::~Game()
{
    delete board;
}

// Switch the current player from 1 to 2 and vice versa
// in the most badass way possible
void Game::switchCurrentPlayer()
{
    current_player = 3 - current_player; // 1 -> 2, 2 -> 1
}

void Game::calculatePermutations()
{
    Tile *rowTile = board->getInTile();
    Tile *currentTile = rowTile;

    while (rowTile) // while the row exists
    {
        currentTile = rowTile;
        while (currentTile) // loop through the tiles of the row
        {
            if (!currentTile->colour) // and if the tile has no colour
            {                         // colour it
                currentTile->colour = gd::PLAYER_COLOURS
                    [current_player - 1];
                switchCurrentPlayer();
                turn++;
                if (doesTileConcludeGame(currentTile)) // if
                {                                      // this concludes the game
                    perm++;                            // increment permutations
                }
                else                         // otherwise
                {                            // calculate the permttns
                    calculatePermutations(); // for the new board
                }
                switchCurrentPlayer();
                turn--;
                currentTile->colour = nullptr; // then undo the move
                if (perm > gd::LARGE_NUM_MAX || perm < 0)
                {
                    perm = -1;
                    return;
                }
            }
            currentTile = currentTile->adjacents[2]; // next column
        }
        rowTile = rowTile->adjacents[4]; // next row
    }
}

char Game::readOptionInput(char c)
{
    // the player who initialized the action
    int player = current_player;

    if (c == '<')
    {
        // Undo moves until the last move of the current player
        // is undone.
        if (undoStack.isEmpty() || turn <= 2)
        {
            std::cout << "There are no moves to undo." << std::endl;
            return '\0';
        }

        do
        {
            undoPlay();
        } while (!undoStack.isEmpty() && (undoStack.peek()->colour ==
                                          gd::PLAYER_COLOURS
                                              [player - 1]));

        return '\0';
    }
    else if (c == '>')
    {
        // Redo moves until the last move of the current player
        // is redone.
        if (redoStack.isEmpty())
        {
            std::cout << "There are no moves to redo." << std::endl;
            return '\0';
        }

        do
        {
            redoPlay();
        } while (!redoStack.isEmpty() && (redoStack.peek()->colour !=
                                          gd::PLAYER_COLOURS
                                              [player - 1]));

        return '\0';
    }
    else if (c == '*')
    {
        perm = 0;
        calculatePermutations();
    }
    else if (c == '~')
    {
        concluded = true;
        return '\0';
    }

    return c;
}

void Game::printMenu()
{

    std::cout << "  It's your turn.";
    if (undoStack.peek() && turn > 2)
    {
        std::cout << " Press [<] to undo your last move.";
    }
    if (redoStack.peek())
    {
        std::cout << " Press [>] to redo your last move.";
    }
    std::cout << " Press [~] to end the game." << std::endl;

    std::cout << " Possible oucomes: ";
    if (perm > 0)
    {
        std::cout << bsf::prettylong(perm);
    }
    else if (perm < 0)
    {
        std::cout << "over " << bsf::prettylong(gd::LARGE_NUM_MAX);
    }
    else
    {
        std::cout << "[*]";
    }
}

void Game::printInput()
{
    // Print the prompt for the player to place a tile
    std::cout << "Enter coordinates: ";
}

void Game::printGame()
{
    menugroup player("menus/player.cfg");
    // Print the board and the current player
    board->print();
    player.print(2, current_player - 1, false, false);
    // Print the options menu
    printMenu();

    std::cout << std::endl;
    std::cout << std::endl;
}

// GETTERS
Goboard &Game::getGoboard()
{
    return *board;
}

int Game::getGamemode()
{
    return gamemode;
}

int Game::getTurn()
{
    return turn;
}

int Game::getCurrentPlayer()
{
    return current_player;
}

int Game::getWinThreshold()
{
    return win_threshold;
}

bool Game::isSpecialKey(char c)
{
    return c == '<' || c == '>' || c == '~' || c == '*';
}

bool Game::isTie()
{
    return turn == board->getHeight() * board->getWidth() + 1;
}

bool Game::doesTileConcludeGame(Tile *placedTile)
{
    return (board->getLongestConnection(placedTile) >=
                win_threshold ||
            isTie());
}
// END GETTERS

// Undo the last move
void Game::undoPlay()
{
    // Check if the undo stack is empty
    if (undoStack.isEmpty())
    {
        return;
    }

    // Pop the move from the undo stack and push it onto the redo stack
    Move move = *undoStack.peek();
    undoStack.pop();
    redoStack.push(move);

    // Remove the tile from the board
    board->remove(move.column, move.row);
    turn--;
    switchCurrentPlayer();
    perm = 0;
}

void Game::redoPlay()
{
    // Check if the redo stack is empty
    if (redoStack.isEmpty())
    {
        return;
    }

    // Pop the move from the redo stack and push it onto the undo stack
    Move move = *redoStack.peek();
    redoStack.pop();
    undoStack.push(move);

    // Place the tile on the board
    board->findAndPlace(move.column, move.row, move.colour);
    turn++;
    switchCurrentPlayer();
    perm = 0;
}

// Define a search radius for the AI to avoid unnecessary checking
// of tiles that are too far away from the last two moves.
void Game::AISetSearchRadius()
{
    // Define an array to keep track of the checked tiles
    for (int i = 0; i < gd::MAX * gd::MAX; ++i)
    {
        scope.tilesToCheck[i] = nullptr;
    }
    scope.tilesCount = 0;

    int searchRadius = win_threshold - 1; 

    // Get the last and second to last moves
    Move lastMove = *undoStack.peek();
    undoStack.pop();                                                  
    Move secondLastMove = undoStack.isEmpty() ? lastMove : *undoStack.peek(); 
    undoStack.push(lastMove);                                        

    // add tiles within the search radius to the tilesToCheck array
    auto addTilesWithinRadius = [&](Move move)
    {
        int startRow = bsf::max(1, move.row - searchRadius);
        int endRow = bsf::min(board->getHeight(), move.row + searchRadius);
        int startColumn = bsf::max(1, move.column - searchRadius);
        int endColumn = bsf::min(board->getWidth(), move.column + searchRadius);

        for (int row = startRow; row <= endRow; ++row)
        {
            for (int column = startColumn; column <= endColumn; ++column)
            {
                Tile *currentTile = board->findTileOnBoard(row, column);
                // Check if we've already added this tile to the array
                bool alreadyAdded = false;
                for (int i = 0; i < scope.tilesCount; ++i)
                {
                    if (scope.tilesToCheck[i] == currentTile)
                    {
                        alreadyAdded = true;
                        break;
                    }
                }
                // If it's a new tile, add it to the array
                if (!alreadyAdded)
                {
                    scope.tilesToCheck[scope.tilesCount++] =
                        currentTile;
                }
            }
        }
    };

    // Add tiles around the last and second to last moves
    addTilesWithinRadius(lastMove);
    addTilesWithinRadius(secondLastMove);

}

// Calculate if there is a tile that can be placed to prevent
// the opponent from a guaranteed win in 2 turns (that is, if
// a single turn of the opponent creates at least 2 winning
// possibilities the move thereafter)
Tile *Game::AIDefenseDepth1(Tile *skipTile)
{
    int row = 1;
    int column = 1;
    Tile *currentTile = board->findTileOnBoard(row, column);

    while (currentTile)
    {
        while (currentTile)
        {
            if (skipTile != currentTile && !currentTile->colour)
            {
                // simulate a opposite turn
                currentTile->colour = gd::PLAYER_COLOURS[2-current_player];

                // see if a win can be blocked with the simulated move
                Tile *t = AIBlockWin();
                // see if another win could be blocked
                Tile *s = AIBlockWin(t);

                // undo simulation
                currentTile->colour = nullptr;

                // if there are indeed two possible winning scenarios
                // as a result of one move, return the move
                if (t && s)
                {
                    return currentTile;
                }   
            }
            currentTile = board->findTileOnBoard(row, ++column);
        }
        column = 1;
        currentTile = board->findTileOnBoard(++row, column);
    }
    return nullptr;
}

Tile *Game::AIDefenseDepth2(Tile *skipTile)
{
    int row = 1;
    int column = 1;
    Tile *currentTile = board->findTileOnBoard(row, column);

    while (currentTile)
    {
        while (currentTile)
        {
            if (skipTile != currentTile && !currentTile->colour)
            {
                // simulate a opposite turn
                currentTile->colour = gd::PLAYER_COLOURS[2-current_player];

                // see if a win can be blocked with the simulated move
                Tile *t = AIDefenseDepth1();
                // see if another win could be blocked
                Tile *s = AIDefenseDepth1(t);

                // undo simulation
                currentTile->colour = nullptr;

                // if there are indeed two possible winning scenarios
                // as a result of one move, return the move
                if (t && s)
                {
                    return currentTile;
                }   
            }
            currentTile = board->findTileOnBoard(row, ++column);
        }
        column = 1;
        currentTile = board->findTileOnBoard(++row, column);
    }
    return nullptr;
}

Tile *Game::AIDefenseDepth3(Tile *skipTile)
{
    // Call AISearchRadius and get an AIScope object with the
    // tiles to check and the number of tiles to check
    std::cout << "Searching " << bsf::pow(scope.tilesCount, 8) << " tiles." << std::endl; // debug
    for (int i = 0; i < scope.tilesCount; ++i) {
        Tile* currentTile = scope.tilesToCheck[i];
        if (skipTile != currentTile && !currentTile->colour) {
            // simulate an opposite turn
            currentTile->colour = gd::PLAYER_COLOURS[2 - current_player];

            // see if a win can be blocked with the simulated move
            Tile *t = AIDefenseDepth2();
            // see if another win could be blocked
            Tile *s = AIDefenseDepth2(t);

            // undo simulation
            currentTile->colour = nullptr;

            // if there are indeed two possible winning scenarios
            // as a result of one move, return the move
            if (t && s)
            {
                return currentTile;
            }
        }
    }

    return nullptr;
}

Tile *Game::AIOffenseDepth1(Tile *skipTile)
{
    int row = 1;
    int column = 1;
    Tile *currentTile = board->findTileOnBoard(row, column);

    while (currentTile)
    {
        while (currentTile)
        {
            if (skipTile != currentTile && !currentTile->colour)
            {
                // simulate a turn
                currentTile->colour = gd::PLAYER_COLOURS[current_player-1];

                // see if a win can be made 
                Tile *t = AIWin();
                // see if another win can be made 
                Tile *s = AIWin(t);

                // undo simulation
                currentTile->colour = nullptr;

                // if there are indeed two possible winning scenarios
                // as a result of one move, return the move
                if (t && s)
                {
                    return currentTile;
                }   
            }
            currentTile = board->findTileOnBoard(row, ++column);
        }
        column = 1;
        currentTile = board->findTileOnBoard(++row, column);
    }
    return nullptr;
}

Tile *Game::AIOffenseDepth2(Tile *skipTile)
{
    int row = 1;
    int column = 1;
    Tile *currentTile = board->findTileOnBoard(row, column);

    while (currentTile)
    {
        while (currentTile)
        {
            if (skipTile != currentTile && !currentTile->colour)
            {
                // simulate a opposite turn
                currentTile->colour = gd::PLAYER_COLOURS[current_player-1];

                // see if a win can be blocked with the simulated move
                Tile *t = AIOffenseDepth1();
                // see if another win could be blocked
                Tile *s = AIOffenseDepth1(t);

                // undo simulation
                currentTile->colour = nullptr;

                // if there are indeed two possible winning scenarios
                // as a result of one move, return the move
                if (t && s)
                {
                    return currentTile;
                }   
            }
            currentTile = board->findTileOnBoard(row, ++column);
        }
        column = 1;
        currentTile = board->findTileOnBoard(++row, column);
    }
    return nullptr;
}

Tile *Game::AIOffenseDepth3(Tile *skipTile)
{
    // Call AISearchRadius and get an AIScope object with the
    // tiles to check and the number of tiles to check
    std::cout << "Searching " << bsf::pow(scope.tilesCount, 8) << " tiles." << std::endl; // debug
    for (int i = 0; i < scope.tilesCount; ++i) {
        Tile* currentTile = scope.tilesToCheck[i];
        if (skipTile != currentTile && !currentTile->colour) {
            // simulate a opposite turn
            currentTile->colour = gd::PLAYER_COLOURS[current_player-1];

            // see if a win can be blocked with the simulated move
            Tile *t = AIOffenseDepth2();
            // see if another win could be blocked
            Tile *s = AIOffenseDepth2(t);

            // undo simulation
            currentTile->colour = nullptr;

            // if there are indeed two possible winning scenarios
            // as a result of one move, return the move
            if (t && s)
            {
                return currentTile;
            }
        }
    }

    return nullptr;
}

Tile *Game::AIOffense() // todo : prevent placing on unwinnable spots
{
    int row = 1;
    int column = 1;
    Tile *currentTile = board->findTileOnBoard(row, column);
    Tile *preferredTile = nullptr;
    int longestConnection = 0;
    int totalConnections = 0;

    while (currentTile)
    {
        while (currentTile)
        {
            if (!currentTile->colour)
            {
                currentTile->colour = gd::PLAYER_COLOURS
                    [current_player - 1];
                if (doesTileConcludeGame(currentTile))
                {
                    currentTile->colour = nullptr;
                    return currentTile;
                }
                if (preferredTile)
                {
                    if (int a = board->getLongestConnection(currentTile) > longestConnection)
                    {
                        preferredTile = currentTile;
                        longestConnection = a;
                    }

                    else if (board->getLongestConnection(currentTile) == longestConnection)
                    {
                        if (int b = board->getTotalConnections(currentTile) > totalConnections)
                        {
                            preferredTile = currentTile;
                            totalConnections = b;
                        }
                    }
                }
                else
                {
                    preferredTile = currentTile;
                    longestConnection =
                        board->getLongestConnection(preferredTile);
                    totalConnections =
                        board->getTotalConnections(preferredTile);
                }
                currentTile->colour = nullptr;
            }
            currentTile = board->findTileOnBoard(row, ++column);
        }
        column = 1;
        currentTile = board->findTileOnBoard(++row, column);
    }

    return preferredTile;
}

Tile *Game::AIWin(Tile *skipTile)
{
    int row = 1;
    int column = 1;
    Tile *currentTile = board->findTileOnBoard(row, column);

    while (currentTile)
    {
        while (currentTile)
        {
            if (skipTile != currentTile && !currentTile->colour)
            {
                currentTile->colour = gd::PLAYER_COLOURS
                    [current_player - 1];
                if (doesTileConcludeGame(currentTile))
                {
                    currentTile->colour = nullptr;
                    return currentTile;
                }
                currentTile->colour = nullptr;
            }
            currentTile = board->findTileOnBoard(row, ++column);
        }
        column = 1;
        currentTile = board->findTileOnBoard(++row, column);
    }
    return nullptr;
}

Tile *Game::AIBlockWin(Tile *skipTile)
{
    int row = 1;
    int column = 1;
    Tile *currentTile = board->findTileOnBoard(row, column);

    while (currentTile)
    {
        while (currentTile)
        {
            if (skipTile != currentTile && !currentTile->colour)
            {
                currentTile->colour = gd::PLAYER_COLOURS
                    [2 - current_player];
                if (doesTileConcludeGame(currentTile))
                {
                    currentTile->colour = nullptr;
                    return currentTile;
                }
                currentTile->colour = nullptr;
            }
            currentTile = board->findTileOnBoard(row, ++column);
        }
        column = 1;
        currentTile = board->findTileOnBoard(++row, column);
    }
    return nullptr;
}

Tile *Game::AISmartPlay()
{
    Tile *tileToPlace = nullptr;
    Tile *last = nullptr;

    if (turn > 1)
    {
        AISetSearchRadius();
        last = board->findTileOnBoard(undoStack.peek()->row,
                                    undoStack.peek()->column);
    }
    // If the AI can win, place a tile to win
    if (Tile *t = AIWin())
    {
        std::cout << "AI can win" << std::endl; // debug
        std::cout << "AI placed a tile at ";
        std::cout << board->getRow(t) << ", ";
        std::cout << board->getColumn(t) << std::endl;
        tileToPlace = t;
    }
    else if (Tile *t = AIBlockWin())
    {
        std::cout << "AI can block win" << std::endl; // debug
        std::cout << "AI placed a tile at ";
        std::cout << board->getRow(t) << ", ";
        std::cout << board->getColumn(t) << std::endl;
        tileToPlace = t;
    }
    // IF STARTING FIRST: place a tile in the middle
    else if (turn == 1)
    {
        int row;
        int column;

        if (board->getHeight() % 2 == 0)
        {
            row = board->getHeight() / 2 + rand()%2;
        }
        else
        {
            row = bsf::ceil(board->getHeight() / 2.0);
        }

        if (board->getWidth() % 2 == 0)
        {
            column = board->getWidth() / 2 + rand()%2;
        }
        else
        {
            column = bsf::ceil(board->getWidth() / 2.0);
        }
        tileToPlace = board->findTileOnBoard(row, column);
    }

    // IF IT'S NOT TURN 1...
    else if (Tile *t = AIOffenseDepth1())
    {
        std::cout << "AI will attack at level 1" << std::endl; // debug
        tileToPlace = t;
    }
    else if (Tile *t = AIDefenseDepth1())
    {
        std::cout << "AI will defend at level 1" << std::endl; // debug
        tileToPlace = t;
    }
    else if (Tile *t = AIOffenseDepth2())
    {
        std::cout << "AI will attack at level 2" << std::endl; // debug
        tileToPlace = t;
    }
    else if (Tile *t = AIDefenseDepth2())
    {
        std::cout << "AI will defend at level 2" << std::endl; // debug
        tileToPlace = t;
    }
    else if (Tile *t = AIOffenseDepth3())
    {
        std::cout << "AI will attack at level 3" << std::endl; // debug
        tileToPlace = t;
    }
    else if (Tile *t = AIDefenseDepth3())
    {
        std::cout << "AI will defend at level 3" << std::endl; // debug
        tileToPlace = t;
    }

    // If starting second: if the previous tile was placed against
    // an edge, place a tile in the middle. Otherwise, for all spots
    // that are not against the edge of the board and are adjacent
    // to the previous tile, find the spot with the
    // highest total distance from the edges or the next tile
    // (that is smallest horizontal distance + smallest vertical
    // distance) and place a tile here.
    else if (turn == 2 && last)
    {
        if (last->adjacents[1] == nullptr || last->adjacents[3] == nullptr ||
            last->adjacents[5] == nullptr || last->adjacents[7] == nullptr)
        {
            tileToPlace = board->findTileOnBoard(
                bsf::ceil(board->getHeight() / 2.0),
                bsf::ceil(board->getWidth() / 2.0));
        }
        else
        {
            int minDistance = gd::MAX * gd::MAX;
            Tile *preferredTile = nullptr;
            for (int i = 0; i < 4; ++i)
            {
                Tile *currentTile = last->adjacents[i];
                if (currentTile->adjacents[0] == nullptr ||
                    currentTile->adjacents[2] == nullptr ||
                    currentTile->adjacents[4] == nullptr ||
                    currentTile->adjacents[6] == nullptr)
                {
                    continue;
                }
                int distance = bsf::min(
                    bsf::min(board->getHeight() - board->getRow(currentTile),
                             board->getRow(currentTile) - 1),
                    bsf::min(board->getWidth() - board->getColumn(currentTile),
                             board->getColumn(currentTile) - 1));
                if (distance < minDistance)
                {
                    minDistance = distance;
                    preferredTile = currentTile;
                }
            }
            tileToPlace = preferredTile;
        }
    }

    // If no win can be blocked, place a tile next to the longest
    // existing AI connection on the board, with a preference for
    // as many free spaces as possible around the tile.
    else if (Tile *t = AIOffense())
    {
        std::cout << "AI will attack" << std::endl;
        tileToPlace = t;
    }

    
    // If after all this, no tile has been found, place a tile
    // randomly next to the last tile that was placed.
    if ((!tileToPlace || tileToPlace->colour) && last)
    {
        for (int i = 0; i < 8; i++) {
            if (last->adjacents[i] && !last->adjacents[i]->colour) {
                if (rand() % 2 == 0) {
                    tileToPlace = last->adjacents[i];
                    break;
                }
            }
        }
    }
    // If even that doesn't do anything, place a tile randomly on one
    // an open spot
    if (!tileToPlace || tileToPlace->colour)
        {
            int row = 1;
            int column = 1;
            Tile *currentTile = board->findTileOnBoard(row, column);
            bool found = false;

            while (currentTile && !found)
            {
                while (currentTile)
                {
                    if (!currentTile->colour)
                    {
                        if (rand() % 2 == 0) {
                            tileToPlace = currentTile;
                            found = true;
                            break;
                        }
                    }
                    currentTile = board->findTileOnBoard(row, ++column);
                }
                column = 1;
                currentTile = board->findTileOnBoard(++row, column);
            }
        }

    // If even that doesn't do anything, the board must be full
    if (!tileToPlace || tileToPlace->colour)
    {
        std::cout << "AI could not make a move. The board is full."; //debug
        std::cout << std::endl;
        return nullptr;
    }

    // Place the tile
    tileToPlace->colour = gd::PLAYER_COLOURS[current_player - 1];

    return tileToPlace;
}

// Place a tile randomly, but within a radius of h-1 
// (where h is the win threshold)
Tile *Game::AIRandomPlay() 
{
    Tile *tileToPlace;

    if (turn == 1)
    {
        do {
            int row = rand() % board->getHeight();
            int column = rand() % board->getWidth();

            tileToPlace = board->findTileOnBoard(row, column);
        } while (tileToPlace->colour);
    }
    else
    {
        AISetSearchRadius();
        do {
            int p = rand() % scope.tilesCount;
            tileToPlace = scope.tilesToCheck[p];
        } while (tileToPlace->colour);
    }

    // Place the tile
    tileToPlace->colour = gd::PLAYER_COLOURS[current_player - 1];

    return tileToPlace;
}

Tile *Game::HumanPlay()
{
    printGame();
    printInput();
    char input = bsf::readchr();
    std::cout << std::endl;
    if (isSpecialKey(input))
    {
        readOptionInput(input);
        return nullptr;
    }
    input = bsf::ltoi(input);
    if (input == -1)
    {
        std::cout << "Invalid input, try again" << std::endl;
        return nullptr;
    }
    int column = input;
    input = bsf::readint();
    if (input == -1)
    {
        std::cout << "Invalid input, try again" << std::endl;
        return nullptr;
    }
    int row = input;

    // Check if the tile was placed at (column, row) and if it was
    // increment the turn counter. If it wasn't, continue the loop
    return board->findAndPlace(column, row,
                               gd::PLAYER_COLOURS[current_player - 1]);
}

// Play a turn. If the current player is the AI, play the AI's turn.
Tile *Game::playturn()
{
    if (gamemode == 0)
    {
        if (current_player == 1)
        {
            if (AILevel[current_player-1] == 0) 
            {
                return AISmartPlay();
            }
            else
            {
                return AIRandomPlay();
            }
        }
        else
        {
            return HumanPlay();
        }
    }

    if (gamemode == 1)
    {
        return HumanPlay();
    }

    if (gamemode == 2)
    {
        if (AILevel[current_player-1] == 0) 
        {
            return AISmartPlay();
        }
        else
        {
            return AIRandomPlay();
        }
    }

    return nullptr;
}

// Play the game
void Game::play()
{
    while (!concluded)
    {
        if (Tile *lastTile = playturn())
        {
            turn++;
            if (doesTileConcludeGame(lastTile))
            {
                if (isTie())
                {
                    concluded = true;
                    board->print();
                    std::cout << "Game has tied." << std::endl;
                    break;
                }
                else
                {
                    concluded = true;
                    board->print();
                    std::cout << "Game was won by player ";
                    std::cout << current_player << ". Congrats!";
                    std::cout << std::endl;
                    break;
                }
            }
            while (redoStack.peek()) // <- clear redo stack
            {
                redoStack.pop();
            }
            undoStack.push(Move{board->getRow(lastTile),
                                board->getColumn(lastTile),
                                gd::PLAYER_COLOURS[current_player - 1]});
        }
        else
        {
            continue;
        }


        switchCurrentPlayer();
        perm = 0;
    }
}

// Quit the game
void Game::quit()
{
    std::cout << "Quitting game..." << std::endl;
    exit(0);
}