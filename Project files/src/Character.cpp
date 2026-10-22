#include "Character.h"

Character::Character(){
    name = "<Character Name>NONAME";
    level = 1;
    str = con = agi = wis = 5;
    turn_timer = turn_length = 50;
    set_hmp(1);
    next_turn = false;
    is_dead = false;
    attack_words = {"offended", "attacked", "poked", "pinched", "slapped", "tickled", "eviscerated", "disintegrated", "body-slammed", "booped", "Teemo Q'd", "imploded", "exploded",
    "poofed", "zapped", "popped", "deleted", "obliterated", "toasted", "squished"};
}


Character::~Character(){
}

int Character::calc_hp(){
    return 10 * (int)con;
}

int Character::calc_mp(){
    return 5 * (int)wis;
}

void Character::set_hmp(int mode) {
    switch (mode) {
        case 1: // full reset hp to con stat
            hp = hp_max = calc_hp();
            mp = mp_max = calc_mp();
            break;
        case 2: // set reset max hp to con stat only
            hp_max = calc_hp();
            mp_max = calc_mp();
            if (hp > hp_max) {
                hp = hp_max;
            }
            if (mp > mp_max) {
                mp = mp_max;
            }
            break;
    }
}

string create_bar(float percent) {
    int num_blocks = (int)ceil(20 * percent);
    int num_empty_blocks = 20 - num_blocks;
    string _blocks;
    for (int b = 0; b < num_blocks; b++) {
        _blocks += "|";
    }
    string empty_blocks;
    for (int b = 0; b < num_empty_blocks; b++) {
        _blocks += "-";
    }
    string _string = "[" + _blocks + empty_blocks + "]";
    return _string;
}

void Character::draw_bars() {
    cout << "Lv " << level << "\t" << name << endl;
    cout << "HP:\t" << hp << "/" << hp_max << endl;
    float hp_percent = ((float)hp / (float)hp_max);
    Console::color(12);
    cout << create_bar(hp_percent) << endl;
    Console::color(15);
    cout << "MP:\t" << mp << "/" << mp_max << endl;
    float mp_percent = ((float)mp / (float)mp_max);
    Console::color(9);
    cout << create_bar(mp_percent) << endl << endl;
    Console::color(15);
}

void Character::wait() {
    turn_timer -= agi;
    if (turn_timer <= 0) {
        reset_timer();
        next_turn = true;
    }
}

void Character::reset_timer() {
    turn_timer = turn_length;
}

void Character::reset_turn() {
    if (next_turn) {
        next_turn = false;
    }
}

void Character::clear_buffs(){
    agi -= buffs.agi;
    buffs.agi = 0;
    hp_max -=  10 * buffs.con;
    if (hp > hp_max) {
        hp = hp_max;
    }
    con -= buffs.con;
    buffs.con = 0;
    mp_max -= 5 * buffs.wis;
    if (mp > mp_max) {
        mp = mp_max;
    }
    wis -= buffs.wis;
    buffs.wis = 0;
    str -= buffs.str;
    buffs.str = 0;
    update_stats();
}

void Character::update_stats(){
    // add buffs
    /*agi += buffs.agi;
    con += buffs.con;
    wis += buffs.wis;
    str += buffs.str;/**/
    // add equipment

    // check hp and mp
    set_hmp(2);
}
