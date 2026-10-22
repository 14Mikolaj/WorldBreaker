// including libraries and classes
#include <iostream>
#include "Menus.h"
#include "Player.h"
#include "Enemy.h"
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <ctime>

// including the name spaces
using namespace std;
using namespace Menus;

// predefines
void start_menu();
void menu(Player &player);
void show_map();


// global variables
HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);


// start_menu --> select_character --> load_character --> menu,
// menu --> investigate --> explore --> trigger fight --> fight
void fight(Player &player, Enemy &enemy) {
    char choice = '0';
    int skill_choice  = 0;
    bool hold = false;
    // only leave loop when either player or enemy is dead, or player chooses to escape
    while (!enemy.is_dead && !player.is_dead && choice != '4') {
        while (!enemy.next_turn && !player.next_turn) {
            if (!hold) {
                // combat system is on a "timer", where player's agility makes them attack faster
                // this is done by calculating turn timer and subtracting agility, then if the
                // timer reaches 0 that character can perform an action, after, the timer is
                // reset by adding a default value of 50.
                enemy.wait();
                player.wait();
            }
            // if the player hasn't done anything, i.e. checked stats, then the timers shouldn't tick
            else    hold = false;
        }
        if (player.next_turn) {
            enemy.draw_bars();  // visual display
            player.draw_bars();  //""
            // combat options
            choice = simple_menu("1-->Attack\n"
                                 "2-->Skill\n"
                                 "3-->Check Stats\n"
                                 "4-->Escape\n\n-->");
            switch (choice) {
            case '1':  // attack
                player.attack(enemy, player);
                player.reset_turn();  // reset timer (+50)
                break;
            case '2':  // skills
                skill_choice = skill_menu(player);  // open a skill menu for player to select a skill
                if (skill_choice == 99) {  // 99 is default for back
                    hold = true;  //if going back then the player hasn't done anything, so timers should be stopped
                    break;
                }
                else {  // otherwise, actually use the skill
                    player.use_skill(skill_choice, enemy, player);
                    player.reset_turn();  // and reset timer (+50)
                }
                break;
            case '3':
                player.get_stats();  // display player stats
                hold = true;  // once again, stop timer since no action done
                break;
            case '4':  // escape
                enemy.attack(player, enemy);  // enemy always attacks the player before they escape
                player.reset_timer();  // reset the player timer, ready for next battle.
                enemy.~Enemy();  // delete enemy from memory
                player.clear_buffs();  // clear any buffs that are on the player (from skills)
                if (player.is_dead) {
                    start_menu();  // go all the way back to start menu if player died
                }
                break;
            default:
                invalid(); // invalid input
            }
        }
        if (enemy.next_turn) {
            enemy.attack(player, enemy);  // let the enemy attack the enemy on their turn
            enemy.reset_turn();  // then reset their turn.
        }
    }
    if (player.is_dead) {
        start_menu();  // double check if the player is dead after the combat ends.
    }
}

// start_menu --> select_character --> load_character --> menu,
// menu --> investigate --> explore --> trigger fight
void trigger_fight(location_data &location, Player &player) {
    int num_enemies = location.enemies.size();
    Enemy enemy;  // initialize enemy from constructor
    int selected_enemy = rand() %num_enemies;  // select a random enemy
    // initialize random enemy from list in the location
    enemy.initialize_enemy(location.enemies[selected_enemy], location.enemy_levels[selected_enemy]);
    int enemy_counter = 0;
    int num_drops = location.drops.size();
    for (int i = 1; i < num_drops; i++) {
        if (enemy_counter == selected_enemy && location.drops[i] != "NEXT") {
            // sort available item drops for this enemy
            enemy.add_drop(location.drops[i], location.drop_chances[enemy_counter]);
        }
        if (location.drops[i] == "NEXT") {
            enemy_counter += 1;  // cycling through enemies drops until the correct enemy
        }
    }
    //enemy.get_stats();  //debugging purposes
    //enemy.get_drops();
    fight(player, enemy);  // start the fight
    // add maybe some story
}

// start_menu --> select_character --> load_character --> menu,
// menu --> investigate --> explore
void explore(location_data &location, Player &player) {
    // if some random chance occurs, do trigger event else do random fight.
    trigger_fight(location, player);
}

// start_menu --> select_character --> load_character --> menu,
// menu --> investigate --> investigate_self --> view_inventory
void view_inventory(Player &player) {
    int max_pages = player.get_inv_max_pages();  // calculate size of inventory
    int current_page = 0;
    int choice = 0;
    while (choice != 8) {  // 8 is default to go back
        choice = inv_menu(player, current_page);  // show available items in inventory on current page
        switch (choice) {
            case 6:
                if (current_page == 0) current_page = max_pages;  // 6 is to go down a page
                else current_page--;  // loop inventory
                break;
            case 7:
                if (current_page == max_pages) current_page = 0;  // 7 is to go up a page
                else current_page++;  // loop inventory
                break;
            case 8:  // go back to investigate_self
                break;
            default:
                player.inspect_item(choice + current_page*6);  // check the selected item
                break;
        }
    }
}

// start_menu --> select_character --> load_character --> menu,
// menu --> investigate --> investigate_self
void investigate_self(Player &player) {
    char choice;
    while (choice != '5') {
        // selection menu
        choice = simple_menu("SELF\n\n"
                             "1-->View Stats\n"
                             "2-->View Equipment\n"
                             "3-->View Skills\n"
                             "4-->View inventory\n"
                             "5-->Back\n\n-->");
        switch (choice) {
            case '1':  // view player stats
                player.get_stats();
                break;
            case '2':  // view player equipment
                player.show_equipment();
                break;
            case '3':  // view player skills
                player.show_skills();
                break;
            case '4':  // view player inventory
                view_inventory(player);
                break;
            case '5':
                break;  // return to investigate
            default:
                invalid();  // invalid input
        }
    }
}

// start_menu --> select_character --> load_character --> menu,
// menu --> investigate
void investigate(location_data &location, Player &player) {
    char choice;
    while (choice != '3') {
        // selection menu
        choice = simple_menu("INVESTIGATE\n\n"
                             "1-->Self\n"
                             "2-->Explore\n"
                             "3-->Back\n\n-->");
        switch (choice) {
            case '1':  // investigate self
                investigate_self(player);
                break;
            case '2':  // explore
                explore(location, player);
                break;
            case '3':  // return to menu
                break;
            default:
                invalid();  // invalid input
        }
    }
}

// start_menu --> select_character --> load_character --> menu,
// menu --> select_location
void select_location(Player &player, location_data &location) {
    char choice = '0';
    int num_locations = location.location_paths.size();
    int int_choice = 0;
    while (int_choice != num_locations+1) {
        for (int l = 0; l < num_locations; l++) {
            // each available location is read from the current location data, then displayed individually
            cout << l+1 << "-->" << location.location_paths[l] << endl;
        }
        string menu_string = to_string(num_locations+1) + "-->Back\n\n-->";
        choice = simple_menu(menu_string);  // create the selection menu
        int int_choice = choice - '0';
        if (int_choice > 0 && int_choice < num_locations+1) {
            player.change_location(location.location_paths[int_choice-1]);  // after selection of location, update it within the player
            int_choice = num_locations+1;
            menu(player);  // returning to menu will automatically update location_data
            break;
        }
        if (int_choice == num_locations+1) {
            menu(player);  // num_locations+1 is default for returning to menu
        }
        else    invalid(); // invalid input
    }
}

// start_menu --> select_character --> load_character --> menu,
// menu --> talk_npc_select --> talk_npc_diag_select -- > talk_npc_return_diag
void talk_npc_return_diag(location_data &location, int selected_npc, string return_diag) {
    string text = "[" + location.npcs[selected_npc] + "]\n\n" +
            location.npcs[selected_npc] + ":\n\n" + return_diag + "\n\n";
    simple_text(text);  // display the return dialogue from selected dialogue in previous menu (talk_npc_diag_select)
}

// start_menu --> select_character --> load_character --> menu,
// menu --> talk_npc_select --> talk_npc_diag_select
void talk_npc_diag_select(location_data &location, int selected_npc){
    // create a menu for user to select only a valid dialogue, from the selected npc from current location
    string selected_dialogue = talk_npc_diag_select_menu(location, selected_npc);
    // display the selected dialogue
    talk_npc_return_diag(location, selected_npc, selected_dialogue);
    //program returns back to menu
}

// start_menu --> select_character --> load_character --> menu,
// menu --> talk_npc_select
void talk_npc_select(location_data &location) {
    int selected_npc;
    while (selected_npc != 99) {  // it's impossible to return from this function, or to have 99 npcs, so 99 is default invalid value
        selected_npc = talk_npc_select_menu(location);  // create a menu of available npcs in the location and allow selection
        if (selected_npc != 99){
            talk_npc_diag_select(location, selected_npc);  // continue to dialogue selection if the user didn't select anything invalid
        }
    }
}
// start_menu --> select_character --> load_character --> menu
void menu(Player &player) {
    location_data this_location;  // location data is a struct of vectors containing all required data for interactions in current location
    string location_name = player.get_location();  // find current location of the player (DebugTown by default, set to Noobsville as character is created)
    this_location = load_location(location_name);  // Menus::load_location reads required file and saves in location data
    char choice = '0';
    while (choice != '6' && choice != '3'){
        // create a menu for the player to see options, and select them
        choice = simple_menu("CHOICE MENU\n\nYou are in:\n" + this_location.name +
            " [Lv " + this_location.level + "]\n\n"
            "1-->Investigate\n"
            "2-->Talk...\n"
            "3-->Change location\n"
            "4-->See Map\n"
            "5-->Save\n"
            "6-->Back\n\n-->");
        switch (choice) {
            case '1':
                investigate(this_location, player);  //investigate menu
                break;
            case '2':
                talk_npc_select(this_location);  // npc selection
                break;
            case '3':
                select_location(player, this_location);  // location selection
                break;
            case '4':
                show_map();  //  display the map from file
                break;
            case '5':
                player.save_character();  // save the character to current slot (saved in character variables)
                break;
            case '6':
                break;  // return to start_menu
            default:
                invalid(); //invalid input

        }
    }
}

// start_menu --> select_character --> load_character --> new_character
void new_character(string slot, Player &player) {
    string character_name = "";
    while (character_name == "") {
        character_name = new_character_name();  // select a name for the character
    }
    string character_class = "";
    while(character_class == "") {
        character_class = new_character_class();  // class select (Menus::)
    }
    create_character_skills(character_class, slot);  // load in the default skills from file
    player.new_character(character_name, character_class, slot);  // initialize new player variables

}

// start_menu --> select_character --> load_character
void load_character(string slot) {
    Player player;  // initialize player from constructor
    ifstream save_file;
    string file_name = "lib/save/" + slot + "/player.sav";
    save_file.open(file_name);  // open the required save file
    if (!save_file) {
        SetConsoleTextAttribute(hConsole, 207);
        cout << "\n############\n#OPEN ERROR#\n############\n" << endl;  // show an error if file cannot be opened
        SetConsoleTextAttribute(hConsole, 15);
    }
    string name;
    getline(save_file, name);  // read the first line of the save file, which is the player's name
    if (name == "NEW CHARACTER" || name == "") new_character(slot, player);  // check if a new character should be made
    else {
        player.load_character(slot);  // otherwise, load all data from that save slot.
    }
    save_file.close();
    cout << "Loaded character " + slot << endl << endl;
    system("pause");
    system("CLS");
    menu(player);  // go into actual game menu after character has been loaded / created
}

// start_menu --> character_selection
void character_selection() {
    char choice = '0';
    while (choice != '1' && choice != '2' && choice != '3' && choice != '4') {
        choice = character_select("CHARACTER SELECT");  // show the user available choices and let them pick which slot to select
        switch(choice) {
            case '1':
                load_character("1");  // load character in slot 1
                break;
            case '2':
                load_character("2");  // ""
                break;
            case '3':
                load_character("3");  // ""
                break;
            case '4':
                break;  // break from loop, returning to start menu
            default:
                invalid();  // invalid input
        }
    }
}

// start_menu --> options --> delete_char_select
void delete_char_select() {
    char choice = '0';
    while (choice != '1' && choice != '2' && choice != '3' && choice != '4') {
        SetConsoleTextAttribute(hConsole, 207);  // changing the colour to white text on red background - "warning"
        cout << "DELETE CHARACTER";
        SetConsoleTextAttribute(hConsole, 15);
        choice = character_select("");  // Menus::character_select displays names in each 3 slots
        vector<string> names = get_names();  // Menus::get_names reads the names in each slots, for validation
        switch (choice) {
            case '1':
                if (confirm_input(names[0]))    delete_char("1");  // if user confirms to delete character in slot one, then delete it
                break;
            case '2':
                if (confirm_input(names[1]))    delete_char("2");  // ""
                break;
            case '3':
                if (confirm_input(names[2]))    delete_char("3");  // ""
                break;
            case '4':
                break;
            default:
                invalid();
        }
    }
}

// start_menu --> options
void options(){
    char choice;
    while (choice != '2') {
        // create menu
        choice = simple_menu("OPTIONS\n\n"
                             "1-->Delete Character\n"
                             "2-->Back\n\n-->");
        switch (choice) {
            case '1':
                delete_char_select();  // go into character selection for deletion
                break;
            case '2' :
                break;  // break out of while, returning to start_menu.
            default:
                invalid();  // invalid input
        }
    }
}

// read "lib/locations/_WorldMap.txt" and display map
void graveyard() {
    string text = get_grave();  // Menus::get_grave returns contents of file as string
    simple_text(text);  // display the string
}

// read "lib/locations/_WorldMap.txt" and display map
void show_map() {
    string text = get_map();  // Menus::get_map returns contents of file as string
    simple_text(text);  // display the string
}

// first game menu
void start_menu() {
    char choice;
    while (true) {  // program always within this menu, unless exit called (case '4')
        // create menu - returns first input character
        choice = simple_menu("MAIN MENU\n\n"
                             "1-->Start Game\n"
                             "2-->Options\n"
                             "3-->Graveyard\n"
                             "4-->Exit\n\n-->");
        switch(choice) {
            case '1':
                character_selection();  // go to character select
                break;
            case '2':
                options();  // go to options
                break;
            case '3':
                graveyard();  // view graveyard
                break;
            case '4':
                exit(EXIT_SUCCESS);  // close game
            default:
                invalid();  // invalid input
        }
    }
}

// begin code
int main() {
    srand(time(NULL));  // random seed based on time
    SetConsoleTextAttribute(hConsole, 15);  // white text on black background
    start_menu();  // start the game
    return 0;
}
