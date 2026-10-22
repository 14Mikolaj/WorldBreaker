#include "Enemy.h"

Enemy::Enemy() {
}

Enemy::~Enemy() {
}

void Enemy::initialize_enemy(string ename, int elevel) {
    name = ename;
    level = elevel;
    srand(time(NULL));
    str = rand() %4 + 3;
    con = rand() %4 + 3;
    agi = rand() %4 + 3;
    wis = rand() %4 + 3;
    int extra_stats = rand() %(int)(ceil(level * 0.5)) + level;
    int selected_stat;
    while (extra_stats > 0) {
        selected_stat = rand() %4;
        switch (selected_stat) {
            case 0:
                str ++;
                break;
            case 1:
                con++;
                break;
            case 2:
                agi++;
                break;
            case 3:
                wis++;
                break;
        }
        extra_stats--;
    }
    set_hmp(1);
}

void Enemy::get_stats() {
    std::cout << name << endl << level << endl << str <<
     endl << con << endl << agi << endl << wis << endl <<
     hp << endl << hp_max << endl << mp << endl << mp_max << endl;
    Console::pause();
    Console::clear();
}

void Enemy::take_damage(int damage, Player &player) {
    hp -= damage;
    if (hp <= 0) {
        is_dead = true;
        death(player);
    }
}

string Enemy::get_name(){
    return name;
}

void Enemy::attack(Player &player, Enemy &enemy) {
    int critical = rand() %101;
    bool critical_hit = false;
    if (critical < agi) critical_hit = true;
    int dmg = rand() %(int)ceil(str/2) + (int)ceil(str/2);
    if (critical_hit)   dmg = (int)dmg * 1.5;
    player.take_damage(dmg, enemy);
    if (!player.is_dead){
        if (critical_hit)   cout << "CRITICAL HIT!\n\n";
        int random_word_num = rand() %attack_words.size();
        string attack_word = attack_words[random_word_num];
        string text = "The " + name + " has " + attack_word + " you for " + to_string(dmg) + " damage.\n\n";
        Menus::simple_text(text);
    }
}

void Enemy::death(Player &player) {
    player.clear_buffs();
    int before = player.level;
    int random_word_num = rand() %attack_words.size();
    string attack_word = attack_words[random_word_num];
    cout << "VICTORY\n\n" <<
    "You " << attack_word << " the " << name << " to their death.\n\n";
    roll_drops(player);
    int exp_drop = level * 3;
    player.add_exp(exp_drop);
    int after = player.level;
    if (before == after) {
        Menus::simple_text("");
    }
}

void Enemy::add_drop(string name, double drop_chance) {
    available_drops.push_back(name);
    available_drop_chances.push_back(drop_chance);
}

void Enemy::get_drops() {
    int num_drops = available_drops.size();
    for (int i = 0; i < num_drops; i++) {
        cout << "Available drop: " << available_drops[i] <<
        "\nChance: " << available_drop_chances[i] << endl;
    }
}
void Enemy::roll_drops(Player &player) {
    int num_drops = available_drops.size();
    for (int i = 0; i < num_drops; i++) {
        double roll = rand() % 10001;
        roll /= 10000;
        if (roll  < available_drop_chances[i]) {
            player.recieve_item(available_drops[i]);
        }
    }
}

