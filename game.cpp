#include <iostream>
#include <random>
#include "GlobalData.h"
#include "basicfunctions.h"
#include "namespacealiases.h"
#include "menus.h"
#include "menugroups.h"
#include "gobord.h"
#include "game.h"

// MoveStack constructor
MoveStack::MoveStack()
{
    top = nullptr;
}

// MoveStack destructor
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

// Pop (delete) the top move from the stack
void MoveStack::pop()
{
    if (!isEmpty())
    {
        Node *temp = top;
        top = top->next;
        delete temp;
    }
}

// Return the move at the top of the stack
Move *MoveStack::peek()
{
    if (!isEmpty())
    {
        return &(top->move);
    }

    return nullptr;
}

// Game constructor
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

// Game destructor
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

// Calculate the number of permutations (possible outcomes)
// for the current board. This is done recursively by
// placing a tile on the board, switching the current player,
// and repeating the process until the game is concluded.
void Game::calculatePermutations()
{
    Tile *rowTile = board->getInTile();
    Tile *currentTile = rowTile;

    while (rowTile) // while the row exists
    {
        currentTile = rowTile;
        while (currentTile) // loop through the tiles of the row
        {
            if (!currentTile->colour) // and if a tile has no colour,
            {                         // emulate a turn
                currentTile->colour = gd::PLAYER_COLOURS
                    [current_player - 1];
                switchCurrentPlayer();
                turn++;

                if (doesTileConcludeGame(currentTile)) // if this turn
                {                                      // ends the game
                    perm++;                            // increment
                }                                      // perm
                else                                   // otherwise
                {                                      // calculate the permttns
                    calculatePermutations();           // for the new board
                }

                switchCurrentPlayer();
                turn--;
                currentTile->colour = nullptr; // then undo the move

                // If the amount of permutations is higher than
                // LARGE_NUM_MAX or < 0, set perm to -1 to indicate
                // that the number of permutations is too high to be
                // calculated.
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

// Read the input for the options menu
char Game::readOptionInput(char c)
{
    // the player who initialized the action
    int player = current_player;

    // IF c == '<':
    // Undo moves until the last move of the current player
    // is undone. (for this 2-player game that means undoing 2 moves)
    if (c == '<')
    {
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

    // IF c == '>':
    // Redo moves until the last move of the current player
    // is redone. (for this 2-player game that means redoing 2 moves)
    else if (c == '>')
    {
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

    // IF c == '*':
    // Calculate the number of permutations for the current board
    else if (c == '*')
    {
        perm = 0;
        calculatePermutations();
    }

    // IF c == '~':
    // End the game by setting concluded to true
    else if (c == '~')
    {
        concluded = true;
        return '\0';
    }

    return c;
}

// Print the options menu
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
    std::cout << " Press [~] to surrender." << std::endl;

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

// Print the prompt for the player to place a tile
void Game::printInput()
{
    std::cout << "Enter coordinates: ";
}

// Print the board and the current player
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
    return (board->getLongestConnection(placedTile) >= win_threshold || isTie());
}
// END GETTERS

// Undo the last move
void Game::undoPlay()
{
    // Check if the undo stack is empty
    if (undoStack.isEmpty())
    {
        // If it is, return
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
        // If it is, return
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
// a single turn by the opponent creates at least 2 winning
// possibilities the move thereafter)
// Returns a pointer to the tile that should be placed.
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
                currentTile->colour = gd::PLAYER_COLOURS[2 - current_player];

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

// Calculate if there is a tile that can be placed to prevent
// the opponent from a guaranteed win in 3 turns (that is, if
// a single turn by the opponent creates at least 2 assists
// for winning possibilities two moves thereafter)
// Returns a pointer to the tile that should be placed.
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
                currentTile->colour = gd::PLAYER_COLOURS[2 - current_player];

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

// Calculate if there is a tile that can be placed to prevent
// the opponent from a guaranteed win in 4 turns (that is, if
// a single turn by the opponent creates at least 2 assists
// for winning possibilities three moves thereafter)
// NOTE: unlike depth 0,1,2; depth 3 works within a scope of
// tiles surrounding the last two moves making it less
// accurate. This is to increase performance.
// Returns a pointer to the tile that should be placed.
Tile *Game::AIDefenseDepth3(Tile *skipTile)
{
    // Call AISearchRadius and get an AIScope object with the
    // tiles to check and the number of tiles to check
    std::cout << "AI is searching through "; 
    std::cout << bsf::pow(scope.tilesCount, 8);
    std::cout << " tiles." << std::endl; // debug
    for (int i = 0; i < scope.tilesCount; ++i)
    {
        Tile *currentTile = scope.tilesToCheck[i];
        if (skipTile != currentTile && !currentTile->colour)
        {
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

// Calculate if there is a tile that can be placed to create
// a guaranteed win in 2 turns (that is, if a single turn by
// the AI creates at least 2 winning possibilities the move
// thereafter)
// Returns a pointer to the tile that should be placed.
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
                currentTile->colour = gd::PLAYER_COLOURS[current_player - 1];

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

// Calculate if there is a tile that can be placed to create
// a guaranteed win in 3 turns (that is, if a single turn by
// the AI creates at least 2 assists for winning possibilities
// two moves thereafter)
// Returns a pointer to the tile that should be placed.
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
                currentTile->colour = gd::PLAYER_COLOURS[current_player - 1];

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

// Calculate if there is a tile that can be placed to create
// a guaranteed win in 4 turns (that is, if a single turn by
// the AI creates at least 2 assists for winning possibilities
// three moves thereafter)
// NOTE: unlike depth 0,1,2; depth 3 works within a scope of
// tiles surrounding the last two moves making it less
// accurate. This is to increase performance.
// Returns a pointer to the tile that should be placed.
Tile *Game::AIOffenseDepth3(Tile *skipTile)
{
    // Call AISearchRadius and get an AIScope object with the
    // tiles to check and the number of tiles to check
    std::cout << "AI is searching through ";
    std::cout << bsf::pow(scope.tilesCount, 8) << " tiles.";
    std::cout << std::endl; // debug
    for (int i = 0; i < scope.tilesCount; ++i)
    {
        Tile *currentTile = scope.tilesToCheck[i];
        if (skipTile != currentTile && !currentTile->colour)
        {
            // simulate a opposite turn
            currentTile->colour = gd::PLAYER_COLOURS[current_player - 1];

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

// Calculate an offensive move based on very basic strategy
// (to be used when no other move can be made). Returns a
// pointer to the tile that should be placed.
Tile *Game::AIOffense()
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
                    if (int a =
                            board->getLongestConnection(currentTile) > longestConnection)
                    {
                        preferredTile = currentTile;
                        longestConnection = a;
                    }

                    else if (board->getLongestConnection(currentTile) == longestConnection)
                    {
                        if (int b =
                                board->getTotalConnections(currentTile) > totalConnections)
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

// Calculate if a tile can be placed to win the game. Returns
// a pointer to the tile that should be placed.
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

// Calculate if a tile can be placed to block a win. Returns
// a pointer to the tile that should be placed.
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

// Have the AI find a strategic move by going through a set of
// calculations. Places the tile and returns a pointer to the
// placed tile.
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
        tileToPlace = t;
    }
    // If the AI can block a win, place a tile to block the win
    else if (Tile *t = AIBlockWin())
    {
        tileToPlace = t;
    }

    // IF STARTING FIRST: place a tile in the middle
    else if (turn == 1)
    {
        int row;
        int column;

        if (board->getHeight() % 2 == 0)
        {
            row = board->getHeight() / 2 + rand() % 2;
        }
        else
        {
            row = bsf::ceil(board->getHeight() / 2.0);
        }

        if (board->getWidth() % 2 == 0)
        {
            column = board->getWidth() / 2 + rand() % 2;
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
        // std::cout << "AI will attack at level 1" << std::endl; // debug
        tileToPlace = t;
    }
    else if (Tile *t = AIDefenseDepth1())
    {
        // std::cout << "AI will defend at level 1" << std::endl; // debug
        tileToPlace = t;
    }
    else if (Tile *t = AIOffenseDepth2())
    {
        // std::cout << "AI will attack at level 2" << std::endl; // debug
        tileToPlace = t;
    }
    else if (Tile *t = AIDefenseDepth2())
    {
        // std::cout << "AI will defend at level 2" << std::endl; // debug
        tileToPlace = t;
    }
    else if (Tile *t = AIOffenseDepth3())
    {
        // std::cout << "AI will attack at level 3" << std::endl; // debug
        tileToPlace = t;
    }
    else if (Tile *t = AIDefenseDepth3())
    {
        // std::cout << "AI will defend at level 3" << std::endl; // debug
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
                int distance = 
                bsf::min(
                    bsf::min(board->getHeight() - 
                             board->getRow(currentTile),
                             board->getRow(currentTile) - 1)
                             ,
                    bsf::min(board->getWidth() - 
                             board->getColumn(currentTile),
                             board->getColumn(currentTile) - 1)
                        );
                if (distance < minDistance)
                {
                    minDistance = distance;
                    preferredTile = currentTile;
                }
            }
            tileToPlace = preferredTile;
        }
    }

    // If no move was found, place a tile next to the longest
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
        for (int i = 0; i < 8; i++)
        {
            if (last->adjacents[i] && !last->adjacents[i]->colour)
            {
                if (rand() % 2 == 0)
                {
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
                    if (rand() % 2 == 0)
                    {
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
        std::cout << "AI could not make a move. The board is full.";
        std::cout << std::endl;
        return nullptr;
    }

    // Place the tile
    tileToPlace->colour = gd::PLAYER_COLOURS[current_player - 1];

    return tileToPlace;
}

// Place a tile randomly on an open spot
Tile *Game::AIRandomPlay()
{
    Tile *tileToPlace;

    do
    {
        int row = rand() % board->getHeight() + 1;
        int column = rand() % board->getWidth() + 1;

        tileToPlace = board->findTileOnBoard(row, column);
    } while (tileToPlace->colour);

    // Place the tile
    tileToPlace->colour = gd::PLAYER_COLOURS[current_player - 1];

    return tileToPlace;
}

// Place a tile based on the input of the player
Tile *Game::HumanPlay()
{
    int result = 0;
    int column = 0;
    int row = 0;
    char input;

    printGame();
    printInput();

    // Read the input
    input = bsf::readchr();
    // If the input is a special key, read the option input
    if (isSpecialKey(input))
    {
        readOptionInput(input);
        return nullptr; // <- return nullptr to indicate that no tile
                        //    was placed
    }

    result = bsf::readletval(input);
    if (result >= 0)
    {
        column = result;
    }
    else
    {
        std::cout << "Invalid input, try again" << std::endl;
        return nullptr;
    }

    input = bsf::readint();
    if (input == -1)
    {
        std::cout << "Invalid input, try again" << std::endl;
        return nullptr;
    }
    row = input;

    // Return the tile that was placed
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
            if (AILevel[current_player - 1] == 0)
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
        if (AILevel[current_player - 1] == 0)
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

// Play the game and return the turn on which the game was concluded
int Game::play()
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
                    std::cout << std::endl;
                    std::cout << "Game was won by player ";
                    std::cout << current_player;
                    std::cout << std::endl;
                    break;
                }
            }
            
            while (redoStack.peek()) // <- clear redo stack
            {
                redoStack.pop();
            }

            undoStack.push(
                        Move
                            {
                              board->getRow(lastTile),
                              board->getColumn(lastTile),
                              gd::PLAYER_COLOURS[current_player - 1]
                            }
                        );
        }

        else

        {
            continue;
        }

        switchCurrentPlayer();
        perm = 0;
    }

    return turn;
}
