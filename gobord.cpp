// file gobord.cc
#include <iostream>
#include "gobord.h"
#include "GlobalData.h"
#include "basicfunctions.h"
#include "namespacealiases.h"

// Tile constructor
Tile::Tile()
{
  colour = nullptr;
  for (int i = 0; i < 8; i++)
  {
    adjacents[i] = nullptr;
  }
}

// Tile destructor
Tile::~Tile()
{
}

// TileStack constructor
TileStack::TileStack()
{
  top = nullptr;
}

// TileStack destructor
TileStack::~TileStack()
{
  while (top) 
  {
    pop();
  }
}

// Pushes a tile pointer to the stack
void TileStack::push(Tile tile)
{
  Node *node = new Node{tile, top};
  top = node;
}

// Returns true if the stack is empty, false otherwise
bool TileStack::isEmpty()
{
  return top == nullptr;
}

// Pops a tile pointer from the stack
void TileStack::pop()
{
  if (!isEmpty())
  {
    Node *node = top;
    top = top->next;
    delete node;
  }
}

// Returns the tile pointer at the top of the stack
Tile *TileStack::peek()
{
  if (!isEmpty())
  {
    return &(top->tile);
  }

  return nullptr;
}

// Returns a tile pointer to the first adjacent-free tile
// in the stack (meaning: for all i, adjacents[i] == nullptr)
Tile *TileStack::firstfree()
{
  Node *currentNode = top;

  while (currentNode)
  {
    bool isFree = true;
    for (int i = 0; i < 8; i++)
    {
      if (currentNode->tile.adjacents[i])
      {
        isFree = false;
        break;
      }
    }

    if (isFree)
    {
      return &(currentNode->tile);
    }

    currentNode = currentNode->next;
  }

  return nullptr;
}

Tile *TileStack::next(Tile *tile)
{
  Node *currentNode = top;

  while (currentNode)
  {
    if (&(currentNode->tile) == tile)
    {
      return &(currentNode->next->tile);
    }

    currentNode = currentNode->next;
  }

  return nullptr;
}


// Goboard constructor
Goboard::Goboard(int height, int width)
{
  this->height = height;
  this->width = width;
  in = nullptr;

  // Create all the tiles needed for the board.
  for (int i = 0; i < height; i++)
  {
    for (int j = 0; j < width; j++)
    {
      newTile();
    }
  }

  // Make the board
  makeBoard();
}

// Goboard destructor
Goboard::~Goboard()
{

}

// Creates a new tile, adds it to the
// tileStack and returns a pointer to it.
Tile *Goboard::newTile()
{
  Tile tile;
  tileStack.push(tile);
  return tileStack.peek();
}

// Connect two tiles together by setting the pointer
// to tile2 in the adjacent array of tile1 and vice versa.

// Connects tile1 and tile2 horizontally.
// Tile 2 will the right most tile
void Goboard::connectTilesHorizontally(Tile *tile1, Tile *tile2)
{
  if ((tile1 && tile2) && (tile1 != tile2) &&
      (tile1->adjacents[2] == nullptr && tile2->adjacents[6] == nullptr))
  {
    tile1->adjacents[2] = tile2;
    tile2->adjacents[6] = tile1;
  }
}

// Connects tile1 and tile2 vertically.
// Tile 2 will be the bottom most tile.
void Goboard::connectTilesVertically(Tile *tile1, Tile *tile2)
{
  if ((tile1 && tile2) && (tile1 != tile2) &&
      (tile1->adjacents[4] == nullptr && tile2->adjacents[0] == nullptr))
  {
    tile1->adjacents[4] = tile2;
    tile2->adjacents[0] = tile1;
  }
}

// Connects tile1 and tile2 diagonally upwards.
// Tile 2 will be the top right most tile.
void Goboard::connectTilesDiagonallyUpwards(Tile *tile1, Tile *tile2)
{
  if ((tile1 && tile2) && (tile1 != tile2) &&
      (!tile1->adjacents[5] && !tile2->adjacents[1]))
  {
    tile1->adjacents[5] = tile2;
    tile2->adjacents[1] = tile1;
  }
}

// Connects tile1 and tile2 diagonally downwards.
// Tile 2 will be the bottom right most tile.
void Goboard::connectTilesDiagonallyDownwards(Tile *tile1, Tile *tile2)
{
  if ((tile1 && tile2) && (tile1 != tile2) &&
      (!tile1->adjacents[3] && !tile2->adjacents[7]))
  {
    tile1->adjacents[3] = tile2;
    tile2->adjacents[7] = tile1;
  }
}

// Disconnects a tile from any adjacent tiles and add it
// to the tileStack
void Goboard::disconnectTile(Tile *tile)
{
  if (tile)
  {
    for (int i = 0; i < 8; i++)
    {
      if (tile->adjacents[i])
      {
        // 0 <=> 4, 1 <=> 5, 2 <=> 6, 3 <=> 7, 4 <=> 0, 5 <=> 1 etc..
        tile->adjacents[i]->adjacents[(i + 4) % 8] = nullptr;
        tile->adjacents[i] = nullptr;
      }
    }
  }
}

// Makes a row of tiles by connecting them horizontally and
// returns the address of the left most tile.
Tile *Goboard::makeRow(int length)
{
  Tile *currentTile = tileStack.firstfree();
  Tile *row = currentTile;

  for (int i = 0; i < length - 1; i++)
  {
    Tile *newTile = tileStack.next(currentTile); //<-not actually new
    connectTilesHorizontally(currentTile, newTile); // but from stack
    currentTile = newTile;
  }

  return row;
}

// Connects two horizontal rows together. row1 and row2
// are starting addresses of the rows (LTR).
void Goboard::connectRows(Tile *row1, Tile *row2)
{
  if (row1 && row2)
  {
    Tile *currentRow1Tile = row1;
    Tile *currentRow2Tile = row2;

    // Connect row tiles vertically
    do
    {
      connectTilesVertically(currentRow1Tile, currentRow2Tile);
      currentRow1Tile = currentRow1Tile->adjacents[2];
      currentRow2Tile = currentRow2Tile->adjacents[2];
    } while (currentRow1Tile &&
             currentRow2Tile);

    currentRow1Tile = row1;
    currentRow2Tile = row2;

    // Connect row tiles diagonally upwards
    do
    {
      connectTilesDiagonallyUpwards(currentRow1Tile->adjacents[2],
                                    currentRow2Tile);
      currentRow1Tile = currentRow1Tile->adjacents[2];
      currentRow2Tile = currentRow2Tile->adjacents[2];
    } while (currentRow1Tile && currentRow2Tile);

    currentRow1Tile = row1;
    currentRow2Tile = row2;

    // Connect row tiles diagonally downwards
    do
    {
      connectTilesDiagonallyDownwards(currentRow1Tile,
                                      currentRow2Tile->adjacents[2]);
      currentRow1Tile = currentRow1Tile->adjacents[2];
      currentRow2Tile = currentRow2Tile->adjacents[2];
    } while (currentRow1Tile && currentRow2Tile);
  }
  else
  {
    std::cout << "[ERROR] Cannot connect rows." << std::endl;
  }
}

// Makes a board by connecting rows vertically.
Tile *Goboard::makeBoard()
{
  // Use tiles from stack to create rows and connect
  // these rows.
  Tile *currentRow = makeRow(width);
  in = currentRow;
  for (int i = 0; i < height - 1; i++)
  {
    Tile *newRow = makeRow(width);
    connectRows(currentRow, newRow);
    currentRow = newRow;
  }

  return in;
}

// Disbands a row by disconnecting all tiles in the row
void Goboard::disbandRow(Tile *row)
{
  Tile *currentTile = row;

  while (currentTile) 
  {
    Tile *nextTile = currentTile->adjacents[2];
    disconnectTile(currentTile);
    currentTile = nextTile;
  }
}

// Disbands the board by disbanding all rows
void Goboard::disbandBoard()
{
  Tile *currentTile = in;

  while (currentTile)
  {
    Tile *nextTile = currentTile->adjacents[4];
    disbandRow(currentTile);
    currentTile = nextTile;
  } 
}

// GETTERS
Tile *Goboard::getInTile()
{
  return in;
}

int Goboard::getHeight()
{
  return height;
}

int Goboard::getWidth()
{
  return width;
}

// Finds a tile on the board by row and column and
// returns a pointer to it.
Tile *Goboard::findTileOnBoard(int row, int column)
{
  Tile *currentTile = in;

  // Go down one row on the board (by setting the current
  // tile to its adjacents[4] pointer) until the sought for
  // row is reached.
  for (int i = 1; i < row; i++)
  {
    if (currentTile->adjacents[4])
    {
      currentTile = currentTile->adjacents[4];
    }
    // If the row doesn't exist, return nullptr
    else
    {
      return nullptr;
    }
  }

  // Go right one column on the board (by setting the current
  // tile to its adjacents[2] pointer) until the sought for
  // column is reached.
  for (int j = 1; j < column; j++)
  {
    if (currentTile->adjacents[2])
    {
      currentTile = currentTile->adjacents[2];
    }
    // If the column doesn't exist, return nullptr
    else
    {
      return nullptr;
    }
  }

  // Return the sought for tile (currentTile)
  return currentTile;
}

// Get the size of the horizontal connection of a tile
int Goboard::getHorizontalConnection(Tile *tile)
{
  if (tile)
  {
    int connection_size = 0;
    Tile *currentTile = tile;

    if (currentTile->colour)
    {
      // Count the tile itself as a connection (of 1)
      connection_size++;

      // Count right connections
      while (currentTile->adjacents[2] &&
             currentTile->adjacents[2]->colour == tile->colour)
      {
        currentTile = currentTile->adjacents[2];
        connection_size++;
      }

      currentTile = tile;

      // Count left connections
      while (currentTile->adjacents[6] &&
             currentTile->adjacents[6]->colour == tile->colour)
      {
        currentTile = currentTile->adjacents[6];
        connection_size++;
      }

      return connection_size;
    }
    else
    {
      return 0;
    }
  }

  return -1;
}

// Get the size of the vertical connection of a tile
int Goboard::getVerticalConnection(Tile *tile)
{
  if (tile)
  {
    int connection_size = 0;
    Tile *currentTile = tile;

    if (currentTile->colour)
    {
      // Count the tile itself as a connection (of 1)
      connection_size++;

      // Count down connections
      while (currentTile->adjacents[4] &&
             currentTile->adjacents[4]->colour == tile->colour)
      {
        currentTile = currentTile->adjacents[4];
        connection_size++;
      }

      currentTile = tile;

      // Count up connections
      while (currentTile->adjacents[0] &&
             currentTile->adjacents[0]->colour == tile->colour)
      {
        currentTile = currentTile->adjacents[0];
        connection_size++;
      }

      return connection_size;
    }
    else
    {
      return 0;
    }
  }

  return -1;
}

// Get the size of the diagonal up connection of a tile
int Goboard::getDiagonalUpConnection(Tile *tile)
{
  if (tile)
  {
    int connection_size = 0;
    Tile *currentTile = tile;

    if (currentTile->colour)
    {
      // Count the tile itself as a connection (of 1)
      connection_size++;

      // Count right up connections
      while (currentTile->adjacents[1] &&
             currentTile->adjacents[1]->colour == tile->colour)
      {
        currentTile = currentTile->adjacents[1];
        connection_size++;
      }

      currentTile = tile;

      // Count left down connections
      while (currentTile->adjacents[5] &&
             currentTile->adjacents[5]->colour == tile->colour)
      {
        currentTile = currentTile->adjacents[5];
        connection_size++;
      }

      return connection_size;
    }
    else
    {
      return 0;
    }
  }

  return -1;
}

// Get the size of the diagonal down connection of a tile
int Goboard::getDiagonalDownConnection(Tile *tile)
{
  if (tile)
  {
    int connection_size = 0;
    Tile *currentTile = tile;

    if (currentTile->colour)
    {
      // Count the tile itself as a connection (of 1)
      connection_size++;

      // Count right down connections
      while (currentTile->adjacents[3] &&
             currentTile->adjacents[3]->colour == tile->colour)
      {
        currentTile = currentTile->adjacents[3];
        connection_size++;
      }

      currentTile = tile;

      // Count left up connections
      while (currentTile->adjacents[7] &&
             currentTile->adjacents[7]->colour == tile->colour)
      {
        currentTile = currentTile->adjacents[7];
        connection_size++;
      }

      return connection_size;
    }
    else
    {
      return 0;
    }
  }

  return -1;
}

// Get the size of the longest connection of a tile
int Goboard::getLongestConnection(Tile *tile)
{
  int horizontal_connection = getHorizontalConnection(tile);
  int vertical_connection = getVerticalConnection(tile);
  int diagonalup_connection = getDiagonalUpConnection(tile);
  int diagonaldown_connection = getDiagonalDownConnection(tile);

  int connection_size = horizontal_connection;
  if (vertical_connection > connection_size)
  {
    connection_size = vertical_connection;
  }
  if (diagonalup_connection > connection_size)
  {
    connection_size = diagonalup_connection;
  }
  if (diagonaldown_connection > connection_size)
  {
    connection_size = diagonaldown_connection;
  }

  return connection_size;
}

// Get the total size of all connections of a tile
int Goboard::getTotalConnections(Tile *tile)
{
  int horizontal_connection = getHorizontalConnection(tile);
  int vertical_connection = getVerticalConnection(tile);
  int diagonalup_connection = getDiagonalUpConnection(tile);
  int diagonaldown_connection = getDiagonalDownConnection(tile);

  return horizontal_connection + vertical_connection +
         diagonalup_connection + diagonaldown_connection;
}

// Get the row number of a tile by counting the number of
// tiles above it.
int Goboard::getRow(Tile *tile)
{
  int row = 1;
  Tile *currentTile = tile;

  // As long as currentTile is not a nullpointer
  while (currentTile)
  {
    // Set currentTile to be the tile above it
    currentTile = currentTile->adjacents[0];
    // And increment row
    row++;
  }

  return row;
}

// Get the column number of a tile by counting the number of
// tiles to the left of it.
int Goboard::getColumn(Tile *tile)
{
  int column = 1;
  Tile *currentTile = tile;

  // As long as currentTile is not a nullpointer
  while (currentTile)
  {
    // Set currentTile to be the tile to the left of it
    currentTile = currentTile->adjacents[6];
    // And increment column
    column++;
  }

  return column;
}
// END GETTERS

// Places a tile on the board. Returns a pointer to the placed tile
Tile *Goboard::findAndPlace(int column, int row, char *colour)
{
  Tile *foundTile = findTileOnBoard(row, column);

  if (foundTile)
  {
    if (!foundTile->colour)
    {
      foundTile->colour = colour;
      return foundTile;
    }
    else
    {
      std::cout << "There's already a stone in that spot.";
    }
  }
  else
  {
    std::cout << "That coordinate doesn't exist on this board!";
  }

  std::cout << std::endl;

  return nullptr;
}

// Removes a tile from the board. Returns a pointer to the removed tile
Tile *Goboard::remove(int column, int row)
{
  Tile *foundTile = findTileOnBoard(row, column);

  if (foundTile)
  {
    if (foundTile->colour)
    {
      foundTile->colour = nullptr;
      return foundTile;
    }
    else
    {
      std::cout << "There's no stone in that spot.";
    }
  }

  return nullptr;
}

// Prints the letter coordinates on the board
void Goboard::printLetterCoordinates()
{
  // Folds are one iteration of the alphabet
  int folds = bsf::floor(width / alph::size);

  // Iterate over the folds (alph::size letters per fold)
  for (int i = 0; i <= folds; i++)
  {
    // Check if it's the last fold
    if (i == folds)
    {
      // Iterate over the remaining letters in the last fold
      for (int j = 0; j < (width % alph::size); j++)
      {
        // Print a space if it's fold 0, otherwise the fold letter
        if (i == 0)
        {
          std::cout << " ";
        }
        else
        {
          std::cout << alph::upper[i - 1];
        }
        // Print the current letter and a space
        std::cout << alph::upper[j] << " ";
      }
    }
    else // if not the last fold
    {
      // Iterate over the letters in a fold
      for (int j = 0; j < alph::size; j++)
      {
        // Print a space if it's fold 0, otherwise the fold letter
        if (i == 0)
        {
          std::cout << " ";
        }
        else
        {
          std::cout << alph::upper[i - 1];
        }
        // Print the current letter and a space
        std::cout << alph::upper[j] << " ";
      }
    }
  }
}

// Prints the contents of a row
void Goboard::printRow(Tile *row)
{
  Tile *currentTile = row;

  do
  {
    for (int i = 0; i < gd::TILE_SIZE; i++)
    {
      if (currentTile->colour)
      {
        std::cout << currentTile->colour;
      }
      else
      {
        std::cout << ".";
      }
    }
    std::cout << " ";
    if (currentTile->adjacents[2])
    {
      currentTile = currentTile->adjacents[2];
    }
    else
    {
      break;
    }
  } while (currentTile);
}

void Goboard::print()
{
  Tile *currentRow = in;

  // Print the first row with letter coordinates
  std::cout << std::endl;
  std::cout << "   ";
  printLetterCoordinates();
  std::cout << std::endl;

  // Print the rest of the menu
  int i = 1;
  do
  {
    // Print the left row of numbers
    if (i < 10)
    {
      std::cout << " " << i << " ";
    }
    else
    {
      std::cout << i << " ";
    }
    // Print the contents of the board
    printRow(currentRow);
    std::cout << std::endl;
    if (currentRow->adjacents[4])
    {
      currentRow = currentRow->adjacents[4];
    }
    else
    {
      break;
    }
    i++;
  } while (currentRow);
}
