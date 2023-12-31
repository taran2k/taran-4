#ifndef GOBORD_H
#define GOBORD_H
#include "GlobalData.h"
#include "namespacealiases.h"

// file gobord.h
class Tile
{
public:
  char *colour;       //     7 0 1
  Tile *adjacents[8]; //     6 X 2
  Tile();             //     5 4 3
  ~Tile();
};

class TileStack
{
public:
  TileStack();
  ~TileStack();
  void push(Tile tile);
  bool isEmpty();
  void pop();
  Tile *peek();
  Tile *firstfree();
  Tile* next(Tile *tile);
private:
  struct Node
  {
    Tile tile;
    Node *next;
  };
  Node *top;
};

// When tiles are not on the board, they are stored in
// the bucket. The bucket is a stack of tile pointers.
class Goboard
{
public:
  Goboard(int height, int width);
  ~Goboard();
  Tile *newTile();
  void connectTilesHorizontally(Tile *tile1, Tile *tile2);
  void connectTilesVertically(Tile *tile1, Tile *tile2);
  void connectTilesDiagonallyUpwards(Tile *tile1, Tile *tile2);
  void connectTilesDiagonallyDownwards(Tile *tile1, Tile *tile2);
  void disconnectTile(Tile *tile);
  Tile *makeRow(int length);
  void connectRows(Tile *row1, Tile *row2);
  Tile *makeBoard();
  void disbandRow(Tile *row);
  void disbandBoard();
  Tile *getInTile();
  int getHeight();
  int getWidth();
  Tile *findTileOnBoard(int row, int column);
  int getHorizontalConnection(Tile *tile);
  int getVerticalConnection(Tile *tile);
  int getDiagonalUpConnection(Tile *tile);
  int getDiagonalDownConnection(Tile *tile);
  int getLongestConnection(Tile *tile);
  int getTotalConnections(Tile *tile);
  int getRow(Tile* tile);
  int getColumn(Tile* tile);
  void printLetterCoordinates();
  void printRow(Tile *row);
  void print();
  Tile *findAndPlace(int column, int row, char *colour);
  Tile *remove(int column, int row);
private:
  Tile *in;
  TileStack tileStack;
  int height, width; 
};

#endif