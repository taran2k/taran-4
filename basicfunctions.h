#ifndef BASICFUNCTIONS_H
#define BASICFUNCTIONS_H

namespace BasicFunctions {
    extern char readchr();
    extern int readletval(char input = readchr()); 
    extern int readint(int min = 0, int max = 99999, char c = readchr());
    extern bool isnum(char c);
    extern int ltoi(char c); // letter to int
    extern char itol(int i); // int to letter (lowercase)
    extern std::string readstr();
    extern void action(); //debug -> remove
    extern int calcind(int boxWidth, int titleOrOptionLength);
    extern int floor(float x);
    extern int ceil(float x);
    extern int min(int x, int y);
    extern int max(int x, int y);
    extern float abs(float x);
    extern float pow(float x, int n);
    extern float sqrt(float x, float epsilon = 0.0001);
    extern long long perm(int n, int r);
    extern long long fact(int n);
    extern std::string prettylong(long long n);
}

#endif
