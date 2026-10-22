#ifndef MENUS_H
#define MENUS_H
#include <iostream>
#include <fstream>
#include <cstring>
#include <array>
#include "Player.h"
#include <vector>
#include <algorithm>
#include <dirent.h>
#include <stdio.h>

class Player;

using namespace std;

namespace Menus {

    void invalid();
    char simple_menu(string options);
    void simple_text(string text);
    string truncate(string str, size_t width, bool show_ellipsis=true);
    bool confirm_input(string input);
    bool confirm_class(char class_choice);
    char character_select(string title);
    string new_character_name();
    string new_character_class();
    bool check_num(char input);
    bool check_valid_choice(int input,int  num_choices);
    bool check_valid_string(char input[], int max_length = 40);
    void delete_char(string slot);
    vector<string> get_names();
    string get_grave();
    int skill_menu(Player &player);
    void create_character_skills(string class_name, string slot);
    string get_map();
    int inv_menu(Player &player, int current_page);

    struct location_data {
        string name;
        string level;
        vector<string> npcs;
        vector<string> dialogues;
        vector<string> enemies;
        vector<int> enemy_levels;
        vector<string> location_paths;
        vector<string> drops;
        vector<double> drop_chances;
        //vector<string> events;
        //vector<bool> events_reoccurring;
        //vector<float> event_chance;
    };

    location_data load_location(string location_name);
    char choice_menu(location_data &location);
    int talk_npc_select_menu(location_data &location);
    string talk_npc_diag_select_menu(location_data &location, int npc_num);
}

#endif // MENUS_H
