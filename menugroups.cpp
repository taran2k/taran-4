#include <iostream>
#include <string>
#include <fstream>
#include <cstring>
#include <cmath>
#include "menus.h"
#include "menugroups.h"
#include "basicfunctions.h"
#include "GlobalData.h"
#include "namespacealiases.h"

menugroup::menugroup(std::string path) {
    for (int i = 0; i < MAX_MENUS; ++i) {
        memset(menuConfigs[i].name, 0, sizeof(menuConfigs[i].name));
        memset(menuConfigs[i].title, 0, sizeof(menuConfigs[i].title));
        menuConfigs[i].title_padding = 0;
        for (int j = 0; j < MAX_OPTIONS; ++j) {
            memset(menuConfigs[i].options[j], 0, sizeof(menuConfigs[i].options[j]));
        }
        menuConfigs[i].optionCount = 0;
        menuConfigs[i].corner_symbol = '+';
        menuConfigs[i].horizontal_symbol = '-';
        menuConfigs[i].vertical_symbol = '|';
        menuConfigs[i].spacing_symbol = ' ';
    }
    group_background_symbol = ' ';
    groupHasTitle = false;
    highestOptionAmount = 0;
    menuCount = 0;
    spacing = 1;
    padding_x = 1;
    padding_y = 1;
    extra_prompt_1 = '\0';
    compact_mode_key = '\0';
    extra_prompt_2 = '\0';
    navigation_mode_key = '\0';
    navigation_mode_footer = "Navigate";
    navigation_left_key = '<';
    navigation_right_key = '>';
    navigation_not_available_symbol = 'x';
    input_prompt="Input: ";
    this->path = path;
    readConfigFile();
    namedMenus = new NamedMenu[menuCount];
    createMenusFromConfig();
    refresh();
}

menugroup::~menugroup() {
    delete[] namedMenus;
}

void menugroup::refresh() {
    for (int i = 0; i < menuCount; ++i) {
        namedMenus[i].m.menuCreateVector();
    }
    groupCalculateWidth();
    groupCalculateHeight();
    groupCalculateHighestOptionAmount();
}

/** @brief menugroup::print() USAGE:
 *
 * Will default to gd::DEFAULT_VIEW. Parameters are optional.
 * @param[in]  VIEW  change the view in which the menus are printed.
 * @param[in]  MENUS_TO_PRINT print a specific amount of menus
 * starting from the first for the default view (0), ignored for 
 * compact view (1) and prints a specific menu for the navigation view (2).
 * @param[in]  NAV  whether or not to allow for navigation. 
 * Default is true.
 * @param[in]  INPUT  whether or not to print the input prompt. 
 * Default is true.
 * @return void, prints the menus.
 * 
 * VIEWS: 0 = default, 1 = compact, 2 = navigation
 */
void menugroup::print(int VIEW, int menusToPrint, bool nav, 
bool input) {
    if (VIEW == 0) {
        printGroup(menusToPrint);
    } else if (VIEW == 1) {
        printGroupCompact();
    } else if (VIEW == 2) {
        printSpecificMenu(menusToPrint, nav);
    }
    if (input) {
        std::cout << input_prompt;
    }
}

void menugroup::printSpecificMenu(int targetID, bool nav) {
    refresh();
    int headerLength = 0;
    int indent = 0;
    
    if (targetID < 0) {
        targetID = 0;
    } else if (targetID > menuCount) {
        targetID = menuCount;
    }

    
    for (int k = 0; k < menuCount; k++) {
        namedMenus[k].m.menuCreateVector();
        if (k == targetID) {
            if (groupHasTitle) {
                groupCalculateTitleLength();
                if (groupTitleLength < namedMenus[k].m.menuGetBoxWidth()) {
                    headerLength = namedMenus[k].m.menuGetBoxWidth() + padding_x * 2;
                } else {
                    headerLength = groupTitleLength + 4; // +4:  '=[' + ']='
                    if (headerLength - namedMenus[k].m.menuGetBoxWidth() > padding_x * 2) { 
                        padding_x = (headerLength - namedMenus[k].m.menuGetBoxWidth()) / 2 ;
                    }
                }
                indent = bsf::calcind(headerLength, groupTitleLength);
                
                std::cout << std::endl;
                for (int i = 0; i < indent - 1; i++) {
                    std::cout << "=";
                }
                std::cout << "[" << groupTitle << "]";
                for (int i = 0; i < indent - 1; i++) {
                    std::cout << "=";
                }
                if (headerLength % 2 == 0) {
                    std::cout << "=";
                }
                std::cout << std::endl;
            }
            for (int py = 0; py < padding_y; py++) {
                for (int hw = 0; hw < headerLength; hw++) {
                    std::cout << group_background_symbol;
                }
                std::cout << std::endl;
            }
            if (padding_x == 0) {
                padding_x = 1;
            }
            for (int i = 0; i < namedMenus[k].m.menuGetBoxHeight(); i++) {
                if (nav && i == namedMenus[k].m.menuGetBoxHeight() / 2 && k > 0) {
                    std::cout << navigation_left_key;
                } else {
                    std::cout << group_background_symbol;
                }
                for (int px = 0; px < padding_x-1; px++) {
                    std::cout << group_background_symbol;
                }
                for (int j = 0; j < namedMenus[k].m.menuGetBoxWidth(); j++) {
                    std::cout << namedMenus[k].m.menuGetVector(i,j);
                }
                for (int px = 0; px < padding_x-1; px++) {
                    std::cout << group_background_symbol;
                }
                if (nav && i == namedMenus[k].m.menuGetBoxHeight() / 2 && k < menuCount - 1) {
                    std::cout << navigation_right_key;
                } else {
                    std::cout << group_background_symbol;
                }
                std::cout << std::endl;
            }
            for (int py = 0; py < padding_y; py++) {
                for (int hw = 0; hw < headerLength; hw++) {
                    std::cout << group_background_symbol;
                }
                std::cout << std::endl;
            }
            if (groupHasTitle) {
                if (nav) {
                    indent = bsf::calcind(headerLength, navigation_mode_footer.length());
                    for (int i = 0; i < indent-3; i++) {
                        std::cout << " ";
                    }
                    if (k > 0) {
                        std::cout << "[" << navigation_left_key << "]";
                    } else {
                        for (int i = 0; i < 3; i++) {
                            std::cout << navigation_not_available_symbol;
                        }
                    }
                    std::cout << navigation_mode_footer; 
                    if (k < menuCount - 1) {
                        std::cout << "[" << navigation_right_key << "]";
                    } else {
                        for (int i = 0; i < 3; i++) {
                            std::cout << navigation_not_available_symbol;
                        }
                    }
                    std::cout << std::endl;
                    for (int py = 0; py < padding_y; py++) {
                        for (int hw = 0; hw < headerLength; hw++) {
                            std::cout << group_background_symbol;
                        }
                        std::cout << std::endl;
                    }
                }
                for (int j = 0; j < headerLength; j++) {
                    std::cout << "=";
                }
                std::cout << std::endl;
                if (extra_prompt_1[0] != '\0') {
                    std::cout << extra_prompt_1 << std::endl;
                }
                if (extra_prompt_2[0] != '\0') {
                    std::cout << extra_prompt_2 << std::endl;
                } 
            }
            return;
        }
    }
}

void menugroup::readConfigFile() {
    std::ifstream configFile(path);
    std::string line = "";
    MenuConfig* currentConfig = nullptr;

    while (getline(configFile, line)) {
        if (line.empty() || line[0] == '#') continue;

        if (line[0] == '[') {
            currentConfig = &menuConfigs[menuCount++];
            std::sscanf(line.c_str(), "[%99[^]]", currentConfig->name);
            currentConfig->optionCount = 0;
            continue;
        }

        if (currentConfig) {
            size_t equalsPos = line.find('=');
            if (equalsPos != std::string::npos) {
                std::string key = line.substr(0, equalsPos);
                std::string value = line.substr(equalsPos + 1);

                if (key == "title") {
                    strncpy(currentConfig->title, value.c_str(), sizeof(currentConfig->title));
                    currentConfig->title[sizeof(currentConfig->title) - 1] = '\0';
                } else if (key == "title_padding") {
                    currentConfig->title_padding = std::stoi(value);
                } else if (key == "corner_symbol") {
                    currentConfig->corner_symbol = value[0];
                } else if (key == "horizontal_symbol") {
                    currentConfig->horizontal_symbol = value[0];
                } else if (key == "vertical_symbol") {
                    currentConfig->vertical_symbol = value[0];
                } else if (key == "spacing_symbol") {
                    currentConfig->spacing_symbol = value[0];
                } else if (key.rfind("option", 0) == 0) {
                    strncpy(currentConfig->options[currentConfig->optionCount], value.c_str(), MAX_OPTION_LENGTH);
                    currentConfig->options[currentConfig->optionCount][MAX_OPTION_LENGTH - 1] = '\0';
                    currentConfig->optionCount++;
                } else if (key == "compact_option") {
                    currentConfig->compact_option = value;
                }
            }
        } else {
            size_t equalsPos = line.find('=');
            if (equalsPos != std::string::npos) {
                std::string key = line.substr(0, equalsPos);
                std::string value = line.substr(equalsPos + 1);

                if (key == "group_title") {
                    groupTitle = value;
                    if (value[0] == '\0') {
                        groupHasTitle = false;
                    } else {
                        groupHasTitle = true;
                    }
                } else if (key == "group_corner_symbol") {
                    for (int i = 0; i < MAX_MENUS; ++i) {
                        if (value[0] == '\0') {
                            menuConfigs[i].corner_symbol = ' ';
                        } else
                        menuConfigs[i].corner_symbol = value[0];
                    }
                } else if (key == "group_horizontal_symbol") {
                    for (int i = 0; i < MAX_MENUS; ++i) {
                        if (value[0] == '\0') {
                            menuConfigs[i].horizontal_symbol = ' ';
                        } else
                        menuConfigs[i].horizontal_symbol = value[0];
                    }
                } else if (key == "group_vertical_symbol") {
                    for (int i = 0; i < MAX_MENUS; ++i) {
                        if (value[0] == '\0') {
                            menuConfigs[i].vertical_symbol = ' ';
                        } else
                        menuConfigs[i].vertical_symbol = value[0];
                    }
                } else if (key == "group_spacing_symbol") {
                    for (int i = 0; i < MAX_MENUS; ++i) {
                        if (value[0] == '\0') {
                            menuConfigs[i].spacing_symbol = ' ';
                        } else
                        menuConfigs[i].spacing_symbol = value[0];
                    }
                } else if (key == "group_background_symbol") {
                    if (value[0] == '\0') {
                        group_background_symbol = ' ';
                    } else
                    group_background_symbol = value[0];
                }
                else if (key == "spacing") {
                    spacing = std::stoi(value);
                } else if (key == "padding_x") {
                    padding_x = std::stoi(value);
                } else if (key == "padding_y") {
                    padding_y = std::stoi(value);
                } else if (key == "extra_prompt_1") {
                    if (value[0] == '\0') {
                        extra_prompt_1 = '\0';
                    } else {
                        extra_prompt_1 = value;
                    }
                } else if (key == "compact_mode_key") {
                    compact_mode_key = value[0];
                } else if (key == "extra_prompt_2") {
                    if (value[0] == '\0') {
                        extra_prompt_2 = '\0';
                    } else {
                        extra_prompt_2 = value;
                    }
                } else if (key == "navigation_mode_key") {
                    navigation_mode_key = value[0];
                } else if (key == "navigation_mode_footer") {
                    if (value[0] == '\0') {
                        navigation_mode_footer = '\0';
                    } else {
                        navigation_mode_footer = value;
                    }
                } else if (key == "navigation_left_key") {
                    if (value[0] != '\0') {
                        navigation_left_key = value[0];
                    }
                } else if (key == "navigation_right_key") {
                    if (value[0] != '\0') {
                        navigation_right_key = value[0];
                    }
                } else if (key == "navigation_not_available_symbol") {
                    if (value[0] != '\0') {
                        navigation_not_available_symbol = value[0];
                    }
                } else if (key == "input_prompt") {
                    if (value[0] == '\0') {
                        input_prompt = '\0';
                    } else {
                        input_prompt = value;
                    }
                }
            }
        } 
    }
}

void menugroup::createMenusFromConfig() {
    for (int i = 0; i < menuCount; ++i) {
        namedMenus[i].name = menuConfigs[i].name;
        namedMenus[i].m.menuSetUp(menuConfigs[i].corner_symbol, menuConfigs[i].horizontal_symbol, menuConfigs[i].vertical_symbol, menuConfigs[i].spacing_symbol, menuConfigs[i].title, menuConfigs[i].title_padding, menuConfigs[i].options, menuConfigs[i].compact_option, menuConfigs[i].optionCount);
    }
}

void menugroup::groupCalculateWidth() {
    groupWidth = 0;
    for (int i = 0; i < menuCount; ++i) {
        groupWidth += namedMenus[i].m.menuGetBoxWidth();
    }
    groupWidth += spacing * (menuCount - 1);

}

void menugroup::groupCalculateHeight() {
    groupHeight = 0;
    for (int i = 0; i < menuCount; ++i) {
        if (namedMenus[i].m.menuGetBoxHeight() > groupHeight) {
            groupHeight = namedMenus[i].m.menuGetBoxHeight();
        }
    }
}

int menugroup::groupGetLargestTitleBoxHeight() {
    int largestTitleBoxHeight = 0;
    for (int i = 0; i < menuCount; ++i) {
        if (namedMenus[i].m.menuGetTitleBoxHeight() > largestTitleBoxHeight) {
            largestTitleBoxHeight = namedMenus[i].m.menuGetTitleBoxHeight();
        }
    }
    return largestTitleBoxHeight;
}

int menugroup::groupGetMenuCount() {
    return menuCount;
}

char menugroup::groupGetCompactModeKey() {
    return compact_mode_key;
}

char menugroup::groupGetNavigationModeKey() {
    return navigation_mode_key;
}

char menugroup::groupGetNavLeftKey() {
    return navigation_left_key;
}

char menugroup::groupGetNavRightKey() {
    return navigation_right_key;
}

std::string menugroup::groupGetInputPrompt() {
    return input_prompt;
}

int menugroup::groupGetMenuIDByName(const char* targetName) {
    for (int i = 0; i < menuCount; ++i) {
        if (strcmp(targetName, namedMenus[i].name) == 0) {
            return i;
        }
    }
    return -1;
}

std::string menugroup::groupGetMenuNameByID(int targetID) {
    return namedMenus[targetID].name;
}

NamedMenu* menugroup::groupGetMenu(int i) {
    return &namedMenus[i];
}

NamedMenu* menugroup::groupGetMenuByName(const char* targetName) {
    for (int i = 0; i < menuCount; ++i) {
        if (strcmp(targetName, namedMenus[i].name) == 0) {
            return &namedMenus[i];
        }
    }
    std::cout << "Menu not found." << std::endl;
    return nullptr;
}

void menugroup::groupCalculateHighestOptionAmount() {
    for (int i = 0; i < menuCount; ++i) {
        if (menuConfigs[i].optionCount > highestOptionAmount) {
            highestOptionAmount = namedMenus[i].m.menuGetAmountOfOptions();
        }
    }
}

void menugroup::groupCalculateTitleLength() {
    groupTitleLength = groupTitle.length();
}

void menugroup::printGroup(int menusToPrint) {
    refresh();
    int groupWidthToPrint = groupWidth;
    int groupHeightToPrint = groupHeight;
    if (menusToPrint > menuCount || menusToPrint < 0) {
        menusToPrint = menuCount;
    } else {
        groupWidthToPrint = 0;
        for (int i = 0; i < menusToPrint; i++) {
            groupWidthToPrint += namedMenus[i].m.menuGetBoxWidth();
        }
        groupWidthToPrint += spacing * (menusToPrint - 1);
    }
    bool menuHasTitle[menusToPrint] = {false};
    int amountOfLines[menusToPrint] = {0};
    int printLinesBehind[menusToPrint] = {0};
    int menuTitleBoxHeight[menusToPrint] = {0};
    int highestMenuTitleBoxHeight = 0;
    int headerLength = 0;
    int indent = 0;
    int correction[menusToPrint] = {0};
    bool incr = false;

    for (int i = 0; i < menusToPrint; i++) {
        amountOfLines[i] = namedMenus[i].m.menuGetAmountOfOptions();
    }

    if (groupHasTitle) {
        groupCalculateTitleLength();
        if (groupTitleLength < groupWidthToPrint) {
            headerLength = groupWidthToPrint + padding_x * 2;
        } else {
            headerLength = groupTitleLength + 4; // +4:  '=[' + ']='
            if (headerLength - groupWidthToPrint > padding_x * 2) { 
                padding_x = (headerLength - groupWidthToPrint) / 2 ;
            }
        }
        indent = bsf::calcind(headerLength, groupTitleLength);
        
        std::cout << std::endl;
        for (int i = 0; i < indent - 1; i++) {
            std::cout << "=";
        }
        std::cout << "[" << groupTitle << "]";
        for (int i = 0; i < indent - 1; i++) {
            std::cout << "=";
        }
        if (headerLength % 2 == 0) {
            std::cout << "=";
        }
        std::cout << std::endl;
    }
    for (int py = 0; py < padding_y; py++) {
        for (int hw = 0; hw < headerLength; hw++) {
            std::cout << group_background_symbol;
        }
        std::cout << std::endl;
    }

    for (int i = 0; i < menusToPrint; i++) {
        menuHasTitle[i] = namedMenus[i].m.menuGetHasTitle();
        if (menuHasTitle[i]) {
            menuTitleBoxHeight[i] = namedMenus[i].m.menuGetTitleBoxHeight();
            if (menuTitleBoxHeight[i] > highestMenuTitleBoxHeight) {
                highestMenuTitleBoxHeight = menuTitleBoxHeight[i];
            }
        } 
    }
    for (int i = 0; i < menusToPrint; i++) {
        correction[i] = highestMenuTitleBoxHeight - menuTitleBoxHeight[i];
        if (namedMenus[i].m.menuGetTitleBoxHeight() == highestMenuTitleBoxHeight && !incr) {
            groupHeightToPrint += (highestOptionAmount - amountOfLines[i]);
            incr = true;
            
        }
    }

    // where it all begins
    for (int i = 0; i < groupHeight; i++) {
        for (int px = 0; px < padding_x; px++) {
            std::cout << group_background_symbol;
        }
        for (int k = 0; k < menusToPrint; k++) {
            namedMenus[k].m.menuCreateVector();
            if (i < highestMenuTitleBoxHeight) {
                for (int j = 0; j < namedMenus[k].m.menuGetBoxWidth(); j++) {
                    if (i >= correction[k]) {
                        std::cout << namedMenus[k].m.menuGetVector(i-correction[k],j);
                    } else {
                        std::cout << group_background_symbol;
                    }
                }
                if (k + 1 < menusToPrint) {
                    for (int s = 0; s < spacing; s++) {
                        std::cout << group_background_symbol;
                    }
                }
            } else {
                if (i >= highestMenuTitleBoxHeight + 3 + amountOfLines[k] && amountOfLines[k] < highestOptionAmount) {
                    printLinesBehind[k]++;
                    for (int j = 0; j < namedMenus[k].m.menuGetBoxWidth(); j++) {
                        std::cout << namedMenus[k].m.menuGetVector((i-correction[k])-printLinesBehind[k],j);
                    }
                    amountOfLines[k]++;
                } else if (printLinesBehind[k] > 0 && amountOfLines[k] == highestOptionAmount) {
                    for (int j = 0; j < namedMenus[k].m.menuGetBoxWidth(); j++) {
                        std::cout << namedMenus[k].m.menuGetVector((i-correction[k])-printLinesBehind[k],j);
                    }
                    printLinesBehind[k] = 0;
                } 
                
                else if (i-correction[k] < namedMenus[k].m.menuGetBoxHeight()) {
                    for (int j = 0; j < namedMenus[k].m.menuGetBoxWidth(); j++) {
                        std::cout << namedMenus[k].m.menuGetVector(i-correction[k],j);
                    }
                } else {
                    for (int j = 0; j < namedMenus[k].m.menuGetBoxWidth(); j++) {
                        std::cout << group_background_symbol;
                    }
                }
                if (k + 1 < menusToPrint) {
                    for (int s = 0; s < spacing; s++) {
                        std::cout << group_background_symbol;
                    }
                }
                
            }
        }
        for (int px = 0; px < padding_x; px++) {
            std::cout << group_background_symbol;
        }
        std::cout << std::endl;

    }

    // where it all ends
    for (int py = 0; py < padding_y; py++) {
        std::cout << std::endl;
    }
    if (groupHasTitle) {
        if (extra_prompt_1[0] != '\0') {
            std::cout << extra_prompt_1 << std::endl;
        }
        if (extra_prompt_2[0] != '\0') {
            std::cout << extra_prompt_2 << std::endl;
        }
        for (int i = 0; i < headerLength; i++) {
            std::cout << "=";
        }
        std::cout << std::endl;
    }
}

void menugroup::printGroupCompact() {
    refresh();
    for (int py = 0; py < padding_y; py++) {
        std::cout << std::endl;
    }
    if (groupHasTitle) {
        std::cout << groupTitle << std::endl;
    }
    for (int i = 0; i < menuCount; ++i) {
        if (namedMenus[i].m.menuGetCompactOption()[0] != '\0') {
            std::cout << namedMenus[i].m.menuGetCompactOption() << std::endl;
        } else {
            std::cout << namedMenus[i].m.menuGetOption(0) << std::endl;
        }
    }
    for (int py = 0; py < padding_y; py++) {
        std::cout << std::endl;
    }
    if (extra_prompt_1[0] != '\0') {
        std::cout << extra_prompt_1 << std::endl;
    }
    if (extra_prompt_2[0] != '\0') {
        std::cout << extra_prompt_2 << std::endl;
    }
}

