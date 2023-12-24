#ifndef GLOBALDATA_H
#define GLOBALDATA_H

struct GlobalData {
    static int DEFAULT_VIEW;
    static int VIEW;
    static int CURRENT_MENU;
    static int GAMES_PLAYED;
    static int TILE_SIZE;
    static const int MIN = 1;
    static const int MAX = 99;
    static char* PLAYER_COLOURS[2];
    static const long long LARGE_NUM_MAX = 10000000;
};

// not using standard ASCII values for the alphabet
// to avoid compatability problems with the 'char' type
namespace LatinAlphabet {
    const int size = 26;
    extern const char* lower[26];
    extern const char* upper[26];
};

namespace GreekAlphabet {
    const int size = 24;
    extern const char* lower[24];
    extern const char* upper[24];
};

#endif