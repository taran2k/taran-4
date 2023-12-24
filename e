int boxHeight = 0;
  int boxWidth = 0;
  if (singleplayer.menuGetBoxHeight() > boxHeight) {
    boxHeight = singleplayer.menuGetBoxHeight();
  }
  if (multiplayer.menuGetBoxHeight() > boxHeight) {
    boxHeight = multiplayer.menuGetBoxHeight();
  }
  if (options.menuGetBoxHeight() > boxHeight) {
    boxHeight = options.menuGetBoxHeight();
  }
  if (exit.menuGetBoxHeight() > boxHeight) {
    boxHeight = exit.menuGetBoxHeight();
  }
  if (singleplayer.menuGetBoxWidth() > boxWidth) {
    boxWidth = singleplayer.menuGetBoxWidth();
  }
  if (multiplayer.menuGetBoxWidth() > boxWidth) {
    boxWidth = multiplayer.menuGetBoxWidth();
  }
  if (options.menuGetBoxWidth() > boxWidth) {
    boxWidth = options.menuGetBoxWidth();
  }
  if (exit.menuGetBoxWidth() > boxWidth) {
    boxWidth = exit.menuGetBoxWidth();
  }

  for (int i = 0; i < boxHeight; i++) {
    for (int j = 0; j < boxWidth; j++) {
      std::cout << singleplayer.menuGetVector(i, j);
    }
    std::cout << "   ";
    for (int j = 0; j < boxWidth; j++) {
      std::cout << multiplayer.menuGetVector(i, j);
    }
    std::cout << "   ";
    for (int j = 0; j < boxWidth; j++) {
      std::cout << options.menuGetVector(i, j);
    }
    std::cout << "   ";
    for (int j = 0; j < boxWidth; j++) {
      std::cout << exit.menuGetVector(i, j);
    }
    std::cout << std::endl;
  }






int menusLoad() {
  menu singleplayer;
  singleplayer.menuSetCornerSymbol('+');
  singleplayer.menuSetHorizontalSymbol('-');
  singleplayer.menuSetVerticalSymbol('|');
  singleplayer.menuSetSpacingSymbol(' ');
  singleplayer.menuSetHasTitle(true);
  singleplayer.menuSetTitle("Testing");
  singleplayer.menuSetOptions(0, "[S]ingleplayer");
  singleplayer.menuSetOptions(1, "S1 vs COM");
  singleplayer.menuCalculateAmountOfOptions();
  singleplayer.menuCalculateTitleAndOptionLengths();

  singleplayer.menuCreateVector();
  singleplayer.menuPrint();
  return 0;
}

void printMultipleMenusNextToEachOther() {
  menu singleplayer;
  singleplayer.menuSetCornerSymbol('+');
  singleplayer.menuSetHorizontalSymbol('-');
  singleplayer.menuSetVerticalSymbol('|');
  singleplayer.menuSetSpacingSymbol(' ');
  singleplayer.menuSetHasTitle(false);
  singleplayer.menuSetTitle("");
  singleplayer.menuSetOptions(0, "[S]ingleplayer");
  singleplayer.menuSetOptions(1, "S1 vs COM");
  singleplayer.menuCalculateAmountOfOptions();
  singleplayer.menuCalculateTitleAndOptionLengths();

  menu multiplayer;
  multiplayer.menuSetCornerSymbol('+');
  multiplayer.menuSetHorizontalSymbol('-');
  multiplayer.menuSetVerticalSymbol('|');
  multiplayer.menuSetSpacingSymbol(' ');
  multiplayer.menuSetHasTitle(false);
  multiplayer.menuSetTitle("");
  multiplayer.menuSetOptions(0, "[M]ultiplayer");
  multiplayer.menuSetOptions(1, "S1 vs S2");
  multiplayer.menuCalculateAmountOfOptions();
  multiplayer.menuCalculateTitleAndOptionLengths();

  menu options;
  options.menuSetCornerSymbol('+');
  options.menuSetHorizontalSymbol('-');
  options.menuSetVerticalSymbol('|');
  options.menuSetSpacingSymbol(' ');
  options.menuSetHasTitle(false);
  options.menuSetTitle("");
  options.menuSetOptions(0, "[O]ptions");
  options.menuSetOptions(1, "Change settings");
  options.menuSetOptions(2, "Change language");
  options.menuSetOptions(3, "Change theme");
  options.menuCalculateAmountOfOptions();
  options.menuCalculateTitleAndOptionLengths();

  menu exit;
  exit.menuSetCornerSymbol('+');
  exit.menuSetHorizontalSymbol('-');
  exit.menuSetVerticalSymbol('|');
  exit.menuSetSpacingSymbol(' ');
  exit.menuSetHasTitle(false);
  exit.menuSetTitle("");
  exit.menuSetOptions(0, "[E]xit");
  exit.menuSetOptions(1, "Exit the game");
  exit.menuCalculateAmountOfOptions();
  exit.menuCalculateTitleAndOptionLengths();

  singleplayer.menuCreateVector();
  multiplayer.menuCreateVector();
  options.menuCreateVector();
  exit.menuCreateVector();

  int longestBoxWidth = 0;
  int longestBoxHeight = 0;

  // Pointers to each of your menu class instances
  menu* menus[] = {&singleplayer, &multiplayer, &options, &exit};
  int numMenus = sizeof(menus) / sizeof(menus[0]);

  for (int i = 0; i < numMenus; i++) {
      if (menus[i]->menuGetBoxWidth() > longestBoxWidth) {
          longestBoxWidth = menus[i]->menuGetBoxWidth();
      }
      if (menus[i]->menuGetBoxHeight() > longestBoxHeight) {
          longestBoxHeight = menus[i]->menuGetBoxHeight();
      }
  }

  for (int i = 0; i < longestBoxHeight; i++) {
      for (int j = 0; j < numMenus; j++) {
          for (int k = 0; k < longestBoxWidth; k++) {
              std::cout << menus[j]->menuGetVector(i, k);
          }
          std::cout << "   ";
      }
      std::cout << std::endl;
  }



}

int main ( ) {
  menu singeplayer;
  singeplayer.menuCreate();
  singeplayer.menuCreateVector();
  singeplayer.menuPrint();
  menu options;
  options.menuCreate();
  options.menuCreateVector();
  options.menuPrint();
  return 0;
}