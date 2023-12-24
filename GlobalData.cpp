#include "GlobalData.h"

int GlobalData::DEFAULT_VIEW = 0;
int GlobalData::VIEW = DEFAULT_VIEW;
int GlobalData::CURRENT_MENU = 0;
int GlobalData::GAMES_PLAYED = 0;
int GlobalData::TILE_SIZE = 2;
char* GlobalData::PLAYER_COLOURS[2] = {"X","O"};

const char* LatinAlphabet::lower[26] = {
    "a", "b", "c", "d", "e", "f", "g", "h", "i", "j",
    "k", "l", "m" ,"n", "o", "p", "q", "r", "s", "t",
    "u", "v", "w" ,"x", "y", "z"
};
const char* LatinAlphabet::upper[26] = {
    "A", "B", "C", "D", "E", "F", "G", "H", "I", "J",
    "K", "L", "M" ,"N", "O", "P", "Q", "R", "S", "T",
    "U", "V", "W" ,"X", "Y", "Z"
};

// For when you're greek 
const char* GreekAlphabet::lower[24] = {
    "α", "β", "γ", "δ", "ε", "ζ", "η", "θ", "ι", "κ", 
    "λ", "μ", "ν", "ξ", "ο", "π", "ρ", "σ", "τ", "υ", 
    "φ", "χ", "ψ", "ω"
};
const char* GreekAlphabet::upper[24] = {
    "Α", "Β", "Γ", "Δ", "Ε", "Ζ", "Η", "Θ", "Ι", "Κ", 
    "Λ", "Μ", "Ν", "Ξ", "Ο", "Π", "Ρ", "Σ", "Τ", "Υ", 
    "Φ", "Χ", "Ψ", "Ω"
};