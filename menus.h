#ifndef MENUS_H
#define MENUS_H

const int MAX = 30; //tijdelijk
class menu {
    public:
        menu();
        ~menu();
        void menuSetCornerSymbol(char CORNERSYMBOL);
        void menuSetHorizontalSymbol(char HORSYMBOL);
        void menuSetVerticalSymbol(char VERTSYMBOL);
        void menuSetSpacingSymbol(char SPACINGSYMBOL);
        void menuSetHasTitle(bool HASTITLE);
        void menuSetTitle(std::string TITLE);
        void menuSetTitleBoxPadding(int PADDING);
        void menuSetOptions(int OPTIONNUMBER, std::string OPTION);
        void menuSetOptionsAmount(int AMOUNTOFOPTIONS);
        void menuSetCompactOption(std::string OPTION);
        void menuCalculateAmountOfOptions();
        void menuCalculateTitleAndOptionLengths();
        void menuCreateVector();
        void menuSetUp(char corner_symbol, char horizontal_symbol, 
        char vertical_symbol, char spacing_symbol, const char* title, 
        int title_padding, char options[][MAX], 
        std::string compact_option, int numOptions);
        void menuPrint();
        bool menuGetHasTitle();
        std::string menuGetTitle();
        int menuGetTitleBoxHeight();
        int menuGetTitleSpacing();
        int menuGetBoxHeight();
        int menuGetBoxWidth();
        int menuGetAmountOfOptions();
        std::string menuGetOption(int i);
        std::string menuGetCompactOption();
        char menuGetVector(int X, int Y);
    private:
        char cornersymbol;
        char horizontalsymbol;
        char verticalsymbol;
        char spacingsymbol;
        int amountOfOptions;
        bool hasTitle;
        std::string title;
        int menuTitleLength;
        int menuTitleBoxPadding;
        int titleBoxHeight;
        std::string menuOptions[10];
        std::string compact_option;
        int menuOptionLength[10];
        int longestTitleOrOptionLength;
        int boxHeight;
        int boxWidth;
        char menuVector[100][100];
};


#endif