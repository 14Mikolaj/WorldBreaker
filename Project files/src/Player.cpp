#include "Player.h"

Player::Player()
{
    class_ = "ClassType=NOCLASS";
    exp = 0;
    sp = 1;
    exp_next = 10;
    location = "DebugTown";
    save_slot = "0";
    money = 0;
    /*
    equipment.head.name = "NONE";
    equipment.chest.name = "NONE";
    equipment.gloves.name = "NONE";
    equipment.legs.name = "NONE";
    equipment.lhand.name = "NONE";
    equipment.rhand.name = "NONE";
    equipment.acc1.name = "NONE";
    equipment.acc2.name = "NONE";
    equipment.acc3.name = "NONE";
    */
}

Player::~Player()
{
    //dtor
}

void Player::new_character(string input_name, string  input_class_, string input_slot) {
    name = input_name;
    class_ = input_class_;
    if (class_ == "Bruiser"){
            con += 3;
            agi += 1;
            str += 2;
    }
    if (class_ == "Rogue"){
            agi += 3;
            wis += 2;
            str += 1;
    }
    if (class_ == "Scholar"){
            wis += 2;
            agi += 2;
            con += 2;
    }
    if (class_ == "Mage"){
            wis += 4;
            con += 1;
            agi += 1;
    }
    distribute_sp("");
    hp = hp_max = calc_hp();
    mp = mp_max = calc_mp();
    location = "Noobsville";
    save_slot = input_slot;
    load_skills();
    save_character();
}

void Player::load_character(string slot){
    ifstream save_file;
    string file_name = "lib/save/" + slot + "/player.sav";
    save_file.open(file_name);
    if (!save_file) {
        cout << "\n############\n#OPEN ERROR#\n############\n" << endl;
    }
    getline(save_file, name);
    //save_file >> name;
    save_file >> class_;
    save_file >> exp;
    save_file >> level;
    save_file >> hp;
    save_file >> hp_max;
    save_file >> mp;
    save_file >> mp_max;
    save_file >> str;
    save_file >> con;
    save_file >> agi;
    save_file >> wis;
    save_file >> sp;
    save_file >> exp_next;
    getline(save_file, location);
    getline(save_file, location);
    save_slot = slot;
    load_skills();
    load_inventory();
    getline(save_file, load_equipment.head.name);
    getline(save_file, load_equipment.chest.name);
    getline(save_file, load_equipment.gloves.name);
    getline(save_file, load_equipment.legs.name);
    getline(save_file, load_equipment.lhand.name);
    getline(save_file, load_equipment.rhand.name);
    getline(save_file, load_equipment.acc1.name);
    getline(save_file, load_equipment.acc2.name);
    getline(save_file, load_equipment.acc3.name);/*
    cout << "names=" <<  load_equipment.head.name << endl <<
    load_equipment.chest.name << endl <<
    load_equipment.legs.name << endl <<
    load_equipment.gloves.name << endl <<
    load_equipment.lhand.name << endl <<
    load_equipment.rhand.name << endl <<
    load_equipment.acc1.name << endl <<
    load_equipment.acc2.name << endl <<
    load_equipment.acc3.name << endl << endl;/**/
    if (load_equipment.head.name != "None") {
        //load_equip(load_equipment.head.name, "Head");
        Item head = find_item(load_equipment.head.name);
        equipment.head = head;
        format_equipment(head);
        equipment.head.equipped = true;
    }
    if (load_equipment.chest.name != "None") {
        //load_equip(load_equipment.chest.name, "Chest");
        Item chest = find_item(load_equipment.chest.name);
        equipment.chest = chest;
        format_equipment(chest);
        equipment.chest.equipped = true;
    }
    if (load_equipment.gloves.name != "None") {
        //load_equip(load_equipment.gloves.name, "Gloves");
        Item gloves = find_item(load_equipment.gloves.name);
        equipment.gloves = gloves;
        format_equipment(gloves);
        equipment.gloves.equipped = true;
    }
    if (load_equipment.legs.name != "None") {
        //load_equip(load_equipment.legs.name, "Legs");
        Item legs = find_item(load_equipment.legs.name);
        equipment.legs = legs;
        format_equipment(legs);
        equipment.legs.equipped = true;
    }
    if (load_equipment.lhand.name != "None") {
        //load_equip(load_equipment.lhand.name, "LHand");
        Item lhand = find_item(load_equipment.lhand.name);
        equipment.lhand = lhand;
        format_equipment(lhand);
        equipment.lhand.equipped = true;
    }
    if (load_equipment.rhand.name != "None") {
        //load_equip(load_equipment.rhand.name, "RHand");
        Item rhand = find_item(load_equipment.rhand.name);
        equipment.rhand = rhand;
        format_equipment(rhand);
        equipment.rhand.equipped = true;
    }
    if (load_equipment.acc1.name != "None") {
        //load_equip(load_equipment.acc1.name, "Acc1");
        Item acc1 = find_item(load_equipment.acc1.name);
        equipment.acc1 = acc1;
        format_equipment(acc1);
        equipment.acc1.equipped = true;
    }
    if (load_equipment.acc2.name != "None") {
        //load_equip(load_equipment.acc2.name, "Acc2");
        Item acc2 = find_item(load_equipment.acc2.name);
        equipment.acc2 = acc2;
        format_equipment(acc2);
        equipment.acc2.equipped = true;
    }
    if (load_equipment.acc3.name != "None") {
        //load_equip(load_equipment.acc3.name, "Acc3");
        //cout << "acc3 equipped\n";
        Item acc3 = find_item(load_equipment.acc3.name);
        equipment.acc3 = acc3;
        format_equipment(acc3);
        equipment.acc3.equipped = true;
        //cout << "acc3 added\n";
    }/**/
    save_file.close();
}

void Player::load_skills(){
    ifstream save_file;
    string file_name = "lib/save/" + save_slot + "/skills.skill";
    save_file.open(file_name);
    skill this_skill;
    string cost;
    string amount;
    while (getline(save_file, this_skill.name)) {
        getline(save_file, cost);
        getline(save_file, this_skill.effect);
        getline(save_file, this_skill.amount);
        this_skill.cost = stoi(cost);
        skills.push_back(this_skill);
    }
    save_file.close();
}

void Player::load_inventory() {
    ifstream save_file;
    string file_name = "lib/save/" + save_slot + "/inv.inv";
    save_file.open(file_name);
    string item_name;
    while (getline(save_file, item_name)) {
        if (item_name != "") {
            inventory.push_back(find_item(item_name));
        }
    }
    save_file.close();
}

void Player::format_equipment(Item loaded_item, bool add) {
    int inv_size = inventory.size();
    for (int i = 0; i < inv_size; i++) {
        if (inventory[i].name == loaded_item.name) {
            if (add) {
                inventory[i].equipped = true;
            }
            if (!add) {
                inventory[i].equipped = false;
            }
        }
    }
}

/*
void Player::load_equip(string name, string slot) {
        Item selected = find_item(name);
        cout << "selected item=" << endl <<
        selected.name << endl <<
        selected.slot << endl <<
        selected.stats.str << endl <<
        selected.stats.agi << endl <<
        selected.stats.con << endl <<
        selected.stats.wis << endl <<
        selected.value << endl << endl;
        bool equipped = false;
        int str_diff;
        int agi_diff;
        int con_diff;
        int wis_diff;
        if (slot == "Head") {
            str_diff = selected.stats.str - equipment.head.stats.str;
            agi_diff = selected.stats.agi - equipment.head.stats.agi;
            con_diff = selected.stats.con - equipment.head.stats.con;
            wis_diff = selected.stats.wis - equipment.head.stats.wis;
            equipment.head = selected;
            equipped = true;
        }
        if (slot == "Chest") {
            str_diff = selected.stats.str - equipment.chest.stats.str;
            agi_diff = selected.stats.agi - equipment.chest.stats.agi;
            con_diff = selected.stats.con - equipment.chest.stats.con;
            wis_diff = selected.stats.wis - equipment.chest.stats.wis;
            equipment.chest = selected;
            equipped = true;
        }
        if (slot == "Gloves") {
            str_diff = selected.stats.str - equipment.gloves.stats.str;
            agi_diff = selected.stats.agi - equipment.gloves.stats.agi;
            con_diff = selected.stats.con - equipment.gloves.stats.con;
            wis_diff = selected.stats.wis - equipment.gloves.stats.wis;
            equipment.gloves = selected;
            equipped = true;
        }
        if (slot == "Legs") {
            str_diff = selected.stats.str - equipment.legs.stats.str;
            agi_diff = selected.stats.agi - equipment.legs.stats.agi;
            con_diff = selected.stats.con - equipment.legs.stats.con;
            wis_diff = selected.stats.wis - equipment.legs.stats.wis;
            equipment.legs = selected;
            equipped = true;
        }
        if (slot == "LHand") {
            str_diff = selected.stats.str - equipment.lhand.stats.str;
            agi_diff = selected.stats.agi - equipment.lhand.stats.agi;
            con_diff = selected.stats.con - equipment.lhand.stats.con;
            wis_diff = selected.stats.wis - equipment.lhand.stats.wis;
            equipment.lhand = selected;
            equipped = true;
        }
        if (slot == "LHand") {
            str_diff = selected.stats.str - equipment.rhand.stats.str;
            agi_diff = selected.stats.agi - equipment.rhand.stats.agi;
            con_diff = selected.stats.con - equipment.rhand.stats.con;
            wis_diff = selected.stats.wis - equipment.rhand.stats.wis;
            equipment.rhand = selected;
            equipped = true;
        }
        if (slot == "Acc1") {
            str_diff = selected.stats.str - equipment.acc1.stats.str;
            agi_diff = selected.stats.agi - equipment.acc1.stats.agi;
            con_diff = selected.stats.con - equipment.acc1.stats.con;
            wis_diff = selected.stats.wis - equipment.acc1.stats.wis;
            equipment.acc1 = selected;
            equipped = true;
        }
        if (slot == "Acc2") {
            str_diff = selected.stats.str - equipment.acc2.stats.str;
            agi_diff = selected.stats.agi - equipment.acc2.stats.agi;
            con_diff = selected.stats.con - equipment.acc2.stats.con;
            wis_diff = selected.stats.wis - equipment.acc2.stats.wis;
            equipment.acc2 = selected;
            equipped = true;
        }
        if (slot == "Acc3") {
            str_diff = selected.stats.str - equipment.acc3.stats.str;
            agi_diff = selected.stats.agi - equipment.acc3.stats.agi;
            con_diff = selected.stats.con - equipment.acc3.stats.con;
            wis_diff = selected.stats.wis - equipment.acc3.stats.wis;
            equipment.acc3 = selected;
            equipped = true;
        }
        if (equipped) {
            str += str_diff;
            equip.str += str_diff;
            agi += agi_diff;
            equip.agi += agi_diff;
            con += con_diff;
            equip.con += con_diff;
            wis += wis_diff;
            equip.wis += wis_diff;
            cout << "stats changed:\n" <<
            str_diff << endl <<
            agi_diff << endl <<
            con_diff << endl <<
            wis_diff << endl << endl;
    }
}/**/

void Player::save_character() {
    ofstream save_file;
    string file_name = "lib/save/" + save_slot + "/player.sav";
    save_file.open(file_name);

    save_file << name << endl << class_ << endl << exp << endl << level <<
                endl << hp << endl << hp_max << endl << mp << endl << mp_max <<
                endl << str << endl << con << endl << agi << endl << wis <<
                endl << sp << endl << exp_next << endl << location << endl <<
                equipment.head.name << endl << equipment.chest.name << endl <<
                equipment.gloves.name << endl << equipment.legs.name << endl <<
                equipment.lhand.name << endl << equipment.rhand.name << endl <<
                equipment.acc1.name << endl << equipment.acc2.name << endl <<
                equipment.acc3.name << endl;

    save_file.close();
    file_name = "lib/save/" + save_slot + "/inv.inv";
    save_file.open(file_name);
    int inv_size = inventory.size();
    for (int i = 0; i < inv_size; i++) {
        save_file << inventory[i].name << endl;
    }
    save_file.close();
    Menus::simple_text("Player saved to slot " + save_slot + "\n\n");
}



void Player::get_stats() {
    printf("%s\n"
           "Level %i %s\n"
           "Exp : %i / %i\n\n"
           "HP  : %i / %i\n"
           "MP  : %i / %i\n"
           "STR : %i\n"
           "CON : %i\n"
           "AGI : %i\n"
           "WIS : %i\n\n"
           "Location: %s\n\n\n",
           name.c_str(), level, class_.c_str(), exp, exp_next,
           hp, hp_max, mp, mp_max, str, con, agi, wis, location.c_str());
           Console::pause();
           Console::clear();
}

string Player::get_location() {
    return location;
}

void Player::distribute_sp(string level_text) {
    while (sp > 0) {
    Console::color(14);
    cout << level_text;
    Console::color(15);
        cout << "You have " << sp << " remaining to spend:\n\n"
                "1-->STR +1\n"
                "2-->CON +1\n"
                "3-->AGI +1\n"
                "4-->WIS +1\n\n-->";
        string input;
        input = Console::read_token();
        Console::clear();
        char choice = input[0];
        switch (choice) {
            case '1':
                str++;
                sp--;
                break;
            case '2':
                con++;
                sp--;
                break;
            case '3':
                agi++;
                sp--;
                break;
            case '4':
                wis++;
                sp--;
                break;
            default:
                Menus::invalid();
        }
    }
}

void Player::change_location(string new_location) {
    cout << "You have travelled to " << new_location << "." << endl;
    location = new_location;
    save_character();
}

void Player::attack(Enemy &enemy, Player &player) {
    int critical = rand() %101;
    bool critical_hit = false;
    if (critical < agi) critical_hit = true;
    int dmg = rand() %(int)ceil(str/2) + (int)ceil(str/2);
    if (critical_hit)   dmg = (int)dmg * 1.5;
    if (critical_hit)   cout << "CRITICAL HIT!\n\n";
    int random_word_num = rand() %attack_words.size();
    string attack_word = attack_words[random_word_num];
    string text =  "You have " + attack_word + " the " + enemy.get_name() + " for " + to_string(dmg) + " damage.\n\n";
    Menus::simple_text(text);
    enemy.take_damage(dmg, player);
}

void Player::take_damage(int damage, Enemy &enemy) {
    hp -= damage;
    if (hp <= 0) {
        is_dead = true;
        death(enemy);
    }
}

void Player::death(Enemy &enemy) {
    ofstream save_file;
    string file_name = "lib/save/" + save_slot + "/player.sav";
    save_file.open(file_name);
    save_file << "NEW CHARACTER";
    save_file.close();
    file_name = "lib/save/graveyard.txt";
    save_file.open(file_name, ios::app);
    save_file << name << ", a level " << level << " " << class_ <<
    ". Died to a level " << enemy.level << " " << enemy.name << "." << endl;
    save_file.close();
    file_name = "lib/save/" + save_slot + "/skill.skill";
    save_file.open(file_name);
    save_file << "";
    save_file.close();
    Menus::simple_text("OOF\n\nYour character has died and has been deleted.\n"
    "You can still visit them in the graveyard.\n\n\tF\n\n");
}

void Player::add_exp(int amount) {

    exp += amount;
    if (exp >= exp_next){
        cout << "You have gained " << to_string(amount) << " experience points.\n\n";
        level_up();
    }
    else cout << "You have gained " << to_string(amount) <<
                            " experience points.\nNeeded for next level: " <<
                             to_string(exp_next - exp) << "\n\n";
}

void Player::level_up() {
    int before = level;
    while (exp > exp_next) {
        exp -= exp_next;
        level++;
        sp++;
        exp_next += 2 + (int)ceil(0.05 * exp_next);
    }
    int after = level;
    string text ="You levelled up!\n\nLv." + to_string(before) +"\t-->\tLv." + to_string(after) + "\n\n";
    distribute_sp(text);
    hp = hp_max = calc_hp();
    mp = mp_max = calc_mp();
}

void Player::show_skills() {
    int num_skills = skills.size();
    cout << "Name\t\t\tCost\tEffect\tPower" << endl;
    for (int i=0; i<num_skills; i++) {
        skill this_skill = skills[i];
        string nametext = this_skill.name;
        while (nametext.length() < 24) {
            nametext += " ";
        }
        cout << nametext << this_skill.cost << "\t" << this_skill.effect << "\t" << this_skill.amount << endl;
    }
    Console::pause();
    Console::clear();
}

int Player::get_mp() {
    return mp;
}

void Player::show_select_skill() {
    int num_skills = skills.size();
    cout << "CAST SKILL\n\nCurrent Mana: " << mp << "\n\nName\t\t\tCost\tEffect\tPower" << endl;
    int counter = 0;
    for (int i=0; i<num_skills; i++) {
        counter = i+1;
        skill this_skill = skills[i];
        string nametext;
        nametext += (string)to_string(counter);
        nametext += "-->";
        nametext += this_skill.name;
        while (nametext.length() < 24) {
            nametext += " ";
        }
        cout << nametext << this_skill.cost << "\t" << this_skill.effect << "\t" << this_skill.amount << endl;
    }
    cout << counter+1 << "-->Back\n\n-->";
}

void Player::use_skill(int skill_num, Enemy &enemy, Player &player) {
    skill selected = skills[skill_num];
    string stat;
    stat += selected.amount[0];
    stat += selected.amount[1];
    stat += selected.amount[2];
    string str_amount;
    str_amount += selected.amount[4];
    if (selected.amount.length() > 4) {
        str_amount += selected.amount[5];
    }
    int amount = stoi(str_amount);
    //cout << selected.amount << "\n";
    //cout << "stat = " << stat << "\namount = " << amount << endl;
    cout << "You cast : " << selected.name;
    mp -= selected.cost;
    if (selected.effect == "DAMAGE") {
        use_skill_damage(stat, amount, enemy, player);
    }
    if (selected.effect == "HEAL") {
        use_skill_heal(stat, amount);
    }
    if (selected.effect == "BUFF") {
        use_skill_buff(stat, amount);
    }
}

void Player::use_skill_damage(string stat, int amount, Enemy &enemy, Player &player) {
    int damage = 0;
    if (stat == "STR") {
        damage = str * amount;
    }
    if (stat == "WIS") {
        damage = wis * amount;
    }
    if (stat == "CON") {
        damage = con * amount;
    }
    if (stat == "AGI") {
        damage = agi * amount;
    }
    cout << " on the " << enemy.name << ".\n\nIt deals " << damage << " damage!\n\n";
    Console::pause();
    Console::clear();
    enemy.take_damage(damage, player);
}

void Player::use_skill_heal(string stat, int amount) {
    int heal = 0;
    if (stat == "STR") {
        heal = str * amount;
    }
    if (stat == "WIS") {
        heal = wis * amount;
    }
    if (stat == "CON") {
        heal = con * amount;
    }
    if (stat == "AGI") {
        heal = agi * amount;
    }
    if ((hp + heal)> hp_max) {
        heal = hp_max - hp;
    }
    hp += heal;
    cout << " on yourself.\n\nIt heals you for " << heal << "HP!\n\n";
    Console::pause();
    Console::clear();
}

void Player::use_skill_buff(string stat, int amount) {
    if (stat == "STR") {
        str += amount;
        buffs.str += amount;
        cout << " on yourself.\n\nSTR increased by " << amount << "!\n\n";
    }
    if (stat == "WIS") {
        wis += amount;
        buffs.wis += amount;
        mp += 5 * amount;
        cout << " on yourself.\n\nWIS increased by " << amount << "!\n\n" <<
        "Also recovered " << 5 * amount << "MP!\n\n";
    }
    if (stat == "CON") {
        con += amount;
        buffs.con += amount;
        cout << " on yourself.\n\nCON increased by " << amount << "!\n\n";
    }
    if (stat == "AGI") {
        agi += amount;
        buffs.agi += amount;
        cout << " on yourself.\n\nAGI increased by " << amount << "!\n\n";
    }
    update_stats();
    Console::pause();
    Console::clear();
}

void Player::show_equipment() {
    cout << "EQUIPMENT\n\n"
    "Head: " << equipment.head.name << endl <<
    "Chest: " << equipment.chest.name << endl <<
    "Gloves: " << equipment.gloves.name << endl <<
    "Legs: " << equipment.legs.name << endl <<
    "LHand: " << equipment.lhand.name << endl <<
    "RHand: " << equipment.rhand.name << endl <<
    "Accessory 1: " << equipment.acc1.name << endl <<
    "Accessory 2: " << equipment.acc2.name << endl <<
    "Accessory 3: " << equipment.acc3.name << endl << endl;
    Console::pause();
    Console::clear();
}

void Player::recieve_item(string item_name) {
    Console::color(10);
    cout << "Also received [" << item_name + "] !\n\n";
    Console::color(15);
    Item found_item = find_item(item_name);
    inventory.push_back(found_item);
    format_equipment(found_item, false);
}

Item Player::find_item(string item_name) {
    //cout << "attempting to find item \"" << item_name << "\"\n";
    Item lookup;
    ifstream itemfile;
    string filename = "lib/items/item_list.il";
    itemfile.open(filename);
    string line;
    while (getline(itemfile, line) && line != item_name) {}
    if (!itemfile) {
        throw runtime_error("Item not found in lib/items/item_list.il: " + item_name);
    }
    lookup.name = line;
    itemfile >> lookup.level;
    itemfile >> lookup.slot;
    itemfile >> lookup.stats.str;
    itemfile >> lookup.stats.agi;
    itemfile >> lookup.stats.con;
    itemfile >> lookup.stats.wis;
    itemfile >> lookup.value;
    itemfile.close();
    return lookup;
}

int Player::get_inv_max_pages() {
    return (int)ceil(inventory.size() / 6);
}

void Player::show_inv(int page) {
    int start_index = 6 * page;
    int inv_size = inventory.size();
    for (int i = 0; i < 6; i++) {
        if ((i + start_index) > -1 && (i + start_index) < inv_size) {
            if (inventory[i+start_index].equipped) {
                cout << i+1 << "-->" << inventory[i+start_index].name << " ";
                Console::color(158);
                cout  << "[E]" << endl;
                Console::color(15);
            }
            else cout << i+1 << "-->" << inventory[i+start_index].name << endl;
        }
    }
    cout << "\n7-->Down Page\n8-->Up Page\n9-->Back\n\n-->";
}

void Player::inspect_item(int index) {
    Item selected = inventory[index];
    int choice = 0;
    while (choice != 1 && choice != 2 && choice != 3) {
        cout << "INSPECT\n\n" <<
        selected.name << endl <<
        "Level: " << selected.level << endl <<
        "Slot: " << selected.slot << endl <<
        "STR: " << selected.stats.str << endl <<
        "AGI: " << selected.stats.agi << endl <<
        "CON: " << selected.stats.con << endl <<
        "WIS: " << selected.stats.wis << endl <<
        "Value: " << selected.value << endl << endl;
        choice = Menus::simple_menu("1-->Equip/Unequip\n2-->Sell\n3-->Back\n\n-->") - '0';
        if (choice != 1 && choice != 2 && choice != 3) Menus::invalid();
    }
    if (choice == 1) {
        equip_item(index);
    }
    if (choice ==2) {
        sell_item(index);
    }
}

void Player::equip_item(int index) {
    Item selected = inventory[index];
    Item old_item;
    bool equipped = false;
    if (selected.level <= level) {
        int str_diff;
        int agi_diff;
        int con_diff;
        int wis_diff;
        if (selected.slot == "Head") {
            str_diff = selected.stats.str - equipment.head.stats.str;
            agi_diff = selected.stats.agi - equipment.head.stats.agi;
            con_diff = selected.stats.con - equipment.head.stats.con;
            wis_diff = selected.stats.wis - equipment.head.stats.wis;
            old_item = equipment.head;
            if (selected.name != old_item.name) {
                equipment.head = selected;
            }
            else {
                equipment.head = no_item;
            }
            equipped = true;
        }
        if (selected.slot == "Chest") {
            str_diff = selected.stats.str - equipment.chest.stats.str;
            agi_diff = selected.stats.agi - equipment.chest.stats.agi;
            con_diff = selected.stats.con - equipment.chest.stats.con;
            wis_diff = selected.stats.wis - equipment.chest.stats.wis;
            old_item = equipment.chest;
            if (selected.name != old_item.name) {
                equipment.chest = selected;
            }
            else {
                equipment.chest = no_item;
            }
            equipped = true;
        }
        if (selected.slot == "Gloves") {
            str_diff = selected.stats.str - equipment.gloves.stats.str;
            agi_diff = selected.stats.agi - equipment.gloves.stats.agi;
            con_diff = selected.stats.con - equipment.gloves.stats.con;
            wis_diff = selected.stats.wis - equipment.gloves.stats.wis;
            old_item = equipment.gloves;
            if (selected.name != old_item.name) {
                equipment.gloves = selected;
            }
            else {
                equipment.gloves = no_item;
            }
            equipped = true;
        }
        if (selected.slot == "Legs") {
            str_diff = selected.stats.str - equipment.legs.stats.str;
            agi_diff = selected.stats.agi - equipment.legs.stats.agi;
            con_diff = selected.stats.con - equipment.legs.stats.con;
            wis_diff = selected.stats.wis - equipment.legs.stats.wis;
            old_item = equipment.legs;
            if (selected.name != old_item.name) {
                equipment.legs = selected;
            }
            else {
                equipment.legs = no_item;
            }
            equipped = true;
        }
        if (selected.slot == "Hand") {
            int choice = 0;
            while (choice != 1 && choice != 2) {
                choice = Menus::simple_menu("Which slot?\n\n1-->LHand ["+
                                            equipment.lhand.name+"]\n2-->RHand ["+
                                            equipment.rhand.name+"]\n\n-->") - '0';
                if (choice != 1 && choice != 2) {
                    Menus::invalid();
                }
            }
            switch (choice) {
                case 1:
                    if (equipment.rhand.name != selected.name) {
                        str_diff = selected.stats.str - equipment.lhand.stats.str;
                        agi_diff = selected.stats.agi - equipment.lhand.stats.agi;
                        con_diff = selected.stats.con - equipment.lhand.stats.con;
                        wis_diff = selected.stats.wis - equipment.lhand.stats.wis;
                        old_item = equipment.lhand;
                        if (selected.name != old_item.name) {
                            equipment.lhand = selected;
                        }
                        else {
                            equipment.lhand = no_item;
                        }
                        equipped = true;
                    }
                    else Menus::simple_text("You can't equip identical items!\n\n");
                    break;
                case 2:
                    if (equipment.lhand.name != selected.name) {
                        str_diff = selected.stats.str - equipment.rhand.stats.str;
                        agi_diff = selected.stats.agi - equipment.rhand.stats.agi;
                        con_diff = selected.stats.con - equipment.rhand.stats.con;
                        wis_diff = selected.stats.wis - equipment.rhand.stats.wis;
                        old_item = equipment.rhand;
                        if (selected.name != old_item.name) {
                            equipment.rhand = selected;
                        }
                        else {
                            equipment.rhand = no_item;
                        }
                        equipped = true;
                    }
                    else Menus::simple_text("You can't equip identical items!\n\n");
                    break;
            }
        }
        if (selected.slot == "Accessory") {
            int choice = 0;
            while (choice != 1 && choice != 2 && choice != 3) {
                choice = Menus::simple_menu("Which slot?\n\n1-->Accessory 1 ["+
                                            equipment.acc1.name+"]\n2-->Accessory 2 ["+
                                            equipment.acc2.name+"]\n3-->Accessory 3 ["+
                                            equipment.acc3.name+"]\n\n-->") - '0';
                if (choice != 1 && choice != 2 && choice != 3) {
                    Menus::invalid();
                }
            }
            switch (choice) {
                case 1:
                    if (equipment.acc2.name != selected.name && equipment.acc3.name != selected.name) {
                        str_diff = selected.stats.str - equipment.acc1.stats.str;
                        agi_diff = selected.stats.agi - equipment.acc1.stats.agi;
                        con_diff = selected.stats.con - equipment.acc1.stats.con;
                        wis_diff = selected.stats.wis - equipment.acc1.stats.wis;
                        old_item = equipment.acc1;
                        if (selected.name != old_item.name) {
                            equipment.acc1 = selected;
                        }
                        else {
                            equipment.acc1 = no_item;
                        }
                        equipped = true;
                    }
                    else Menus::simple_text("You can't equip identical items!\n\n");
                    break;
                case 2:
                    if (equipment.acc1.name != selected.name && equipment.acc3.name != selected.name) {
                        str_diff = selected.stats.str - equipment.acc2.stats.str;
                        agi_diff = selected.stats.agi - equipment.acc2.stats.agi;
                        con_diff = selected.stats.con - equipment.acc2.stats.con;
                        wis_diff = selected.stats.wis - equipment.acc2.stats.wis;
                        old_item = equipment.acc2;
                        if (selected.name != old_item.name) {
                            equipment.acc2 = selected;
                        }
                        else {
                            equipment.acc2 = no_item;
                        }
                        equipped = true;
                    }
                    else Menus::simple_text("You can't equip identical items!\n\n");
                    break;
                case 3:
                    if (equipment.acc2.name != selected.name && equipment.acc1.name != selected.name) {
                        str_diff = selected.stats.str - equipment.acc3.stats.str;
                        agi_diff = selected.stats.agi - equipment.acc3.stats.agi;
                        con_diff = selected.stats.con - equipment.acc3.stats.con;
                        wis_diff = selected.stats.wis - equipment.acc3.stats.wis;
                        old_item = equipment.acc3;
                        if (selected.name != old_item.name) {
                            equipment.acc3 = selected;
                        }
                        else {
                            equipment.acc3 = no_item;
                        }
                        equipped = true;
                    }
                    else Menus::simple_text("You can't equip identical items!\n\n");
                    break;
            }
        }
        if (equipped) {
            if (selected.name != old_item.name) {
                str += str_diff;
                equip.str += str_diff;
                agi += agi_diff;
                equip.agi += agi_diff;
                con += con_diff;
                equip.con += con_diff;
                wis += wis_diff;
                equip.wis += wis_diff;
                inventory[index].equipped = true;
                format_equipment(selected);
                format_equipment(old_item, false);
                Menus::simple_text("You have equipped " + selected.name+".\n\n");
            }
            else {
                str_diff = -selected.stats.str;
                agi_diff = -selected.stats.agi;
                wis_diff = -selected.stats.wis;
                con_diff = -selected.stats.con;
                str += str_diff;
                equip.str += str_diff;
                agi += agi_diff;
                equip.agi += agi_diff;
                con += con_diff;
                equip.con += con_diff;
                wis += wis_diff;
                equip.wis += wis_diff;
                format_equipment(old_item, false);
                Menus::simple_text("You have unequipped " + selected.name+".\n\n");
            }
    }
    }
    else {
        Menus::simple_text("You are too low level to use that.\n\n");
    }
}

void Player::sell_item(int index) {
    Item selected = inventory[index];
    if (equipment.head.name == selected.name) {
        equipment.head = no_item;
    }
    if (equipment.chest.name == selected.name) {
        equipment.chest = no_item;
    }
    if (equipment.gloves.name == selected.name) {
        equipment.gloves = no_item;
    }
    if (equipment.legs.name == selected.name) {
        equipment.legs = no_item;
    }
    if (equipment.lhand.name == selected.name) {
        equipment.rhand = no_item;
    }
    if (equipment.rhand.name == selected.name) {
        equipment.rhand = no_item;
    }
    if (equipment.acc1.name == selected.name) {
        equipment.acc1 = no_item;
    }
    if (equipment.acc2.name == selected.name) {
        equipment.acc2 = no_item;
    }
    if (equipment.acc3.name == selected.name) {
        equipment.acc3 = no_item;
    }
    inventory.erase(inventory.begin() + index);
    money += selected.value;
}
