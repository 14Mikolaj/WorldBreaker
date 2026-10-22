#include "Menus.h"
#include <cctype>

char get_input() {
    string choice;
    choice = Console::read_token();
    Console::clear();
    return choice[0];
}

void Menus::invalid() {
    Console::color(207);
    cout << "Invalid input\n\n";
    Console::color(15);
    simple_text("");
}

string Menus::truncate(string str, size_t width, bool show_ellipsis)
{
    if (str.length() > width){
        if (show_ellipsis)
            return str.substr(0, width-3) + "...";
        else
            return str.substr(0, width  );
    }
    return str;
}

char Menus::simple_menu(string options) {
    while (true) {
        cout << options;
        return get_input();
    }
}

bool Menus::confirm_input(string input) {
    while (true) {
        cout << "You have chosen:\n\n" << input << "\n\nIs this correct?\n\n1-->Yes\n2-->No\n\n-->";
        string confirm;
        confirm = Console::read_token();
        Console::clear();
        switch (confirm[0]) {
            case '1':
                return true;
            case '2':
                return false;
            default:
                invalid();
        }
    }
}

bool Menus::confirm_class(char class_choice) {
    string confirm;
    while (true) {
        switch(class_choice) {
            case '1':
                cout << "Bruisers specialize in full frontal combat and tanking enemy hits.\n"
                "For choosing a bruiser, you will receive bonus stats:\n\n"
                "+3 CON\n+2 STR\n+1 AGI\n\n And 1 extra skill point for your allocation\n\n"
                "Does this feel right for you?\n\n"
                "1-->Yes\n2-->No\n\n-->";
                confirm = Console::read_token();
                Console::clear();
                break;
            case '2':
                cout << "Rogues specialize fast and deadly strikes, but they cannot take much themselves.\n"
                "For choosing a rogue, you will receive bonus stats:\n\n"
                "\n+3 AGI\n+2 WIS\n+1 STR\n\n And 1 extra skill point for your allocation\n\n"
                "Does this feel right for you?\n\n"
                "1-->Yes\n2-->No\n\n-->";
                confirm = Console::read_token();
                Console::clear();
                break;
            case '3':
                cout << "Scholars are well rounded characters.\n"
                "For choosing a scholar, you will receive bonus stats:\n\n"
                "+2 WIS\n+2 AGI\n+2 CON\n\n And 1 extra skill point for your allocation\n\n"
                "Does this feel right for you?\n\n"
                "1-->Yes\n2-->No\n\n-->";
                confirm = Console::read_token();
                Console::clear();
                break;
            case '4':
                cout << "Mages specialize using powerful spells to aid them in battle.\n"
                "For choosing a mage, you will receive bonus stats:\n\n"
                "+4 WIS\n+1 CON\n+1 AGI\n\n And 1 extra skill point for your allocation\n\n"
                "Does this feel right for you?\n\n"
                "1-->Yes\n2-->No\n\n-->";
                confirm = Console::read_token();
                Console::clear();
                break;
        }
        switch (confirm[0]) {
        case '1':
            return true;
        case '2':
            return false;
        default:
            invalid();
        }
    }
}

void Menus::simple_text(string text) {
    cout << text;
    Console::pause();
    Console::clear();
}

char Menus::character_select(string title) {
    vector<string> names = get_names();
    while (true) {
        printf("%s\n\n1-->%s\n2-->%s\n3-->%s\n4-->Back\n\n-->",
               title.c_str(), names[0].c_str(), names[1].c_str(), names[2].c_str());
        return get_input();
    }
}

vector<string> Menus::get_names(){
    ifstream save_file;
    vector<string> names;
    for (int file_num = 1 ; file_num < 4; file_num++) {
        string filename = "lib/save/" + to_string(file_num) + "/player.sav";
        save_file.open(filename);
        if (!save_file)  {
            Console::color(207);
            cout << "\n############\n#OPEN ERROR#\n############\n" << endl;
            Console::color(15);
        }
        string line;
        getline(save_file, line);
        names.push_back(line);
        save_file.close();
    }
    return names;
}

void Menus::delete_char(string slot) {
    ofstream save_file;
    string file_name = "lib/save/" + slot + "/player.sav";
    save_file.open(file_name);
    save_file << "NEW CHARACTER";
    save_file.close();
    simple_text("Deleted Character in slot " + slot + "\n\n");
}

string Menus::get_grave() {
    ifstream grave;
    grave.open("lib/save/graveyard.txt");
    string line;
    string return_string = "";
    while (grave) {
        getline(grave, line);
        line += "\n";
        return_string += line;
    }
    grave.close();
    return return_string;
}

string Menus::new_character_name() {
    cout << "Looks like you need a name:\n\n-->";
    string name;
    name = Console::read_line();
    Console::clear();
    if (confirm_input(name)) return name;
    else return "";
}

string Menus::new_character_class() {
    cout << "What kind of play style suits you the best? (select class)\n\n"
    "1-->Bruiser\n2-->Rogue\n3-->Scholar\n4-->Mage\n\n-->";
    string choice;
    choice = Console::read_token();
    Console::clear();
    switch(choice[0]) {
        case '1':
            if (confirm_class('1')) return "Bruiser";
            break;
        case '2':
            if (confirm_class('2')) return "Rogue";
            break;
        case '3':
            if (confirm_class('3')) return "Scholar";
            break;
        case '4':
            if (confirm_class('4')) return "Mage";
            break;
        default:
            invalid();
            return "";
    }
    return "";
}

bool Menus::check_num(char input){
    if (! isdigit(input)){
        return false;
    }
    else {
    return true;
    }
}

bool Menus::check_valid_choice(int input,int  num_choices){
    if (input > num_choices) return false;
    else if (input <= 0) return false;
    else return true;
}

bool Menus::check_valid_string(char input[], int max_length) {
    int i = 0;
    while (input[i]) {
        if (!isalpha(input[i]) || i > max_length) return false;
        i++;
    }
    return true;
}

Menus::location_data Menus::load_location(string location_name) {
    ifstream location;
    string file_name = "lib/locations/" + location_name + ".loc";
    location.open(file_name);
    if (!location) {
        Console::color(207);
        cout << "\n############\n#OPEN ERROR#\n############\n" << endl;
        Console::color(15);
        location.close();
        location.open("lib/debug/debug_location.txt");
        location_name = "debug_location";
    }
    string r_level;
    vector<string> r_npcs;
    vector<string> r_dialogues;
    vector<string> r_enemies;
    vector<int> r_enemy_levels;
    vector<string> r_location_paths;
    vector<string> r_drops;
    vector<double> r_drop_chances;
    string current_line;
    string next_line;
    getline(location, r_level);
    getline(location, current_line);
    while (location) {
        if (current_line == "NPC") {
            getline(location, next_line);
            r_npcs.push_back(next_line);
            r_dialogues.push_back("NEXT");
        }
        if (current_line == "DIALOGUE") {
            getline(location, next_line);
            r_dialogues.push_back(next_line);
            getline(location, next_line);
            r_dialogues.push_back(next_line);
        }
        if (current_line == "ENEMY") {
            getline(location, next_line);
            r_enemies.push_back(next_line);
            getline(location, next_line);
            r_enemy_levels.push_back(stoi(next_line));
            r_drops.push_back("NEXT");
        }
        if (current_line == "DROP") {
            getline(location, next_line);
            r_drops.push_back(next_line);
            getline(location, next_line);
            r_drop_chances.push_back(stod(next_line));

        }
        if (current_line == "LOCATION") {
            getline(location, next_line);
            r_location_paths.push_back(next_line);
        }
        getline(location, current_line);
    }
    /*
    for (int i = 0; i<r_drops.size(); i++) {
        cout << "Drop[" << i << "]:\t" << r_drops[i] << endl <<
        "Chance : " << r_drop_chances[i] << endl;
    }/**/
    location_data return_struct;
    return_struct.name = location_name;
    return_struct.level = r_level;
    return_struct.npcs = r_npcs;
    return_struct.dialogues = r_dialogues;
    return_struct.enemies = r_enemies;
    return_struct.enemy_levels = r_enemy_levels;
    return_struct.location_paths = r_location_paths;
    return_struct.drops = r_drops;
    return_struct.drop_chances = r_drop_chances;
    return return_struct;
}

char Menus::choice_menu(Menus::location_data &location){
    cout << "CHOICE MENU\n\nCurrent location:" << location.name <<
            "\n1-->Investigate\n2-->Talk...\n3-->Change location\n"
            "4-->Save\n5-->Exit\n\n-->";
    return get_input();
}

int Menus::talk_npc_select_menu(Menus::location_data &location) {
    int num_npcs = location.npcs.size();
    int choice;
    do {
        cout << "Available NPCs in " << location.name << ":\n\n";
        for (int npc = 0; npc < num_npcs; npc++) {
            cout << npc + 1 << "-->" << location.npcs[npc] << endl;
            if (npc == num_npcs - 1)    cout << npc + 2 << "-->Back" << endl;
        }
        cout << "\n-->";
        choice = get_input() - '0';
        if (choice < 1 || choice > num_npcs+1) invalid();
    } while (choice < 1 || choice > num_npcs+1);
    if (choice == num_npcs + 1) return 99;
    return choice - 1;
}

string Menus::talk_npc_diag_select_menu(Menus::location_data &location, int npc_num) {
    vector<string> current_diags;
    vector<string> next_diags;
    int npc_counter = 0;
    int max_diags = location.dialogues.size();
    for (int i = 1; i < max_diags ; i+=2) {
        //cout << "dialogue = " <<  location.dialogues[i] << ", i = " << i <<  endl;
        if (npc_counter == npc_num && location.dialogues[i] != "NEXT") {
            current_diags.push_back(location.dialogues[i]);
            next_diags.push_back(location.dialogues[i+1]);
            }
        if (location.dialogues[i] == "NEXT") {
                npc_counter += 1;
                i-=1;
        }
    }
    int num_diags = current_diags.size();
    int choice;
    do {
        cout << "[" << location.npcs[npc_num] << "]\n\nYou:\n";
        for (int diag = 0; diag < num_diags; diag++) {
            cout << diag + 1 << "-->" << current_diags[diag] << endl;
        }
        cout << "\n-->";
        choice = get_input() - '0';
        if (choice < 1 || choice > num_diags) invalid();
    } while (choice < 1 || choice > num_diags);
    return next_diags[choice - 1];
}

int Menus::skill_menu(Player &player) {
    int choice;
    int num_skills = player.skills.size();
    do {
        player.show_select_skill();
        choice = get_input() - '0';
        if (choice < 1 || choice > num_skills+1) invalid();
    } while (choice < 1 || choice > num_skills+1);
    if (choice == num_skills + 1) return 99;
    if (player.get_mp() < player.skills[choice-1].cost) {
        cout << "You don't have enough MP to use that!\n" << endl;
        Console::pause();
        Console::clear();
        return 99;
    }
    return choice - 1;
}

void Menus::create_character_skills(string class_name, string slot) {
    for (char &letter : class_name) {
        letter = static_cast<char>(tolower(static_cast<unsigned char>(letter)));
    }
    ifstream default_skills;
    ofstream current_skills;
    vector<string> skill_contents;
    string line;
    string target_file = "lib/save/default/skills_" + class_name + ".skill";
    default_skills.open(target_file);
    while (default_skills) {
        getline(default_skills, line);
        skill_contents.push_back(line);
    }
    default_skills.close();
    target_file = "lib/save/" + slot + "/skills.skill";
    current_skills.open(target_file);
    int num_lines = skill_contents.size();
    for (int i = 0; i < num_lines-1; i++) {
        current_skills << skill_contents[i];
        if (i != num_lines-2) {
            current_skills << endl;
        }
    }
}

string Menus::get_map() {
    ifstream mapf;
    mapf.open("lib/locations/_WorldMap.txt");
    string line;
    string return_string = "";
    while (mapf) {
        getline(mapf, line);
        line += "\n";
        return_string += line;
    }
    mapf.close();
    return return_string;
}

int Menus::inv_menu(Player &player, int current_page) {
    int choice;
    int inv_size = player.inventory.size();
    do {
        cout << "INVENTORY\n\nPage [" << current_page+1 << "/" <<
        player.get_inv_max_pages()+1 << "]\n\n";
        player.show_inv(current_page);
        choice = get_input() - '0';
        if (choice == 9 || choice == 8 || choice == 7) break;
        if (choice < 1 || (choice + current_page * 6) > inv_size || choice > 9) {
            invalid();
        }
    } while ((choice < 1 || (choice + current_page * 6) > inv_size || choice > 9));
    return choice-1;
}

