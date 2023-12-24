#ifndef MENUGROUPS_H
#define MENUGROUPS_H

const int MAX_OPTIONS = 10;
const int MAX_OPTION_LENGTH = 30;
const int MAX_MENUS = 5;

struct MenuConfig {
    char name[100];
    char title[MAX_OPTION_LENGTH];
    int title_padding;
    char options[MAX_OPTIONS][MAX_OPTION_LENGTH];
    std::string compact_option;
    int optionCount;
    char corner_symbol;
    char horizontal_symbol;
    char vertical_symbol;
    char spacing_symbol;
};

struct NamedMenu {
    menu m;
    const char* name;
};

class menugroup {
    public:
        menugroup(std::string path);
        ~menugroup();
        void refresh();
        void print(int VIEW = 0, int menusToPrint = -1, 
        bool nav = true, bool input = true);
        void printSpecificMenu(int targetID, bool nav = false);
        void readConfigFile();
        void createMenusFromConfig();
        void groupCalculateWidth();
        void groupCalculateHeight();
        int groupGetLargestTitleBoxHeight();
        int groupGetMenuCount();
        char groupGetCompactModeKey();
        char groupGetNavigationModeKey();
        char groupGetNavLeftKey();
        char groupGetNavRightKey();
        std::string groupGetInputPrompt();
        int groupGetMenuIDByName(const char* targetName);
        std::string groupGetMenuNameByID(int targetID);
        NamedMenu* groupGetMenu(int i);
        NamedMenu* groupGetMenuByName(const char* targetName);
        void groupCalculateHighestOptionAmount();
        void groupCalculateTitleLength();
        void printGroup(int menusToPrint = 0);
        void printGroupCompact();
    private:
        MenuConfig menuConfigs[MAX_MENUS];
        NamedMenu* namedMenus;
        int menuCount;
        std::string path;
        int highestOptionAmount;
        bool groupHasTitle;
        std::string groupTitle;
        int groupTitleLength;
        int groupWidth;
        int groupHeight;
        char group_corner_symbol;
        char group_horizontal_symbol;
        char group_vertical_symbol;
        char group_spacing_symbol;
        char group_background_symbol;
        int spacing;
        int padding_x;
        int padding_y;
        std::string extra_prompt_1;
        char compact_mode_key;
        std::string extra_prompt_2;
        char navigation_mode_key;
        std::string navigation_mode_footer;
        std::string input_prompt;
        char navigation_left_key;
        char navigation_right_key;
        char navigation_not_available_symbol;
};

#endif
