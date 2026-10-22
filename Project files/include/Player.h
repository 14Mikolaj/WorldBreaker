#ifndef PLAYER_H
#define PLAYER_H
#include "Character.h"
#include "Enemy.h"
#include "Menus.h"
#include "Item.h"

class Enemy;
class Item;

class Player : public Character {

    public:
        Player();
        virtual ~Player();

        int money;

        void new_character(string input_name, string input_class_, string input_slot);
        void load_character(string slot);
        void get_stats();
        string get_location();
        void save_character();
        void load_skills ();
        void load_inventory();
        void load_equip(int inv_index, int slot);
        void attack(Enemy &enemy, Player &player);
        void take_damage(int damage, Enemy &enemy);
        void change_location(string new_location);
        void add_exp(int amount);
        void show_skills();
        void show_select_skill();
        void use_skill(int skill_num, Enemy &enemy, Player &player);
        void use_skill_damage(string stat, int amount, Enemy &enemy, Player &player);
        void use_skill_heal(string stat, int amount);
        void use_skill_buff(string stat, int amount);

        struct equipment_struct {
        Item head;
        Item chest;
        Item gloves;
        Item legs;
        Item rhand;
        Item lhand;
        Item acc1;
        Item acc2;
        Item acc3;
        } equipment, load_equipment;

        Item no_item;
        vector<Item> inventory;
        int get_inv_max_pages();
        void show_inv(int page);
        void show_equipment();
        void recieve_item(string item_name);
        Item find_item(string item_name);
        void equip_item(int index);
        void inspect_item(int index);
        void sell_item(int index);
        void format_equipment(Item loaded_item, bool add = true);
        int get_mp();

    protected:

    private:

        struct equip_stack {
            int con, wis, agi, str;
        } equip = {0, 0, 0, 0};

        string class_, location, save_slot;
        int exp, sp, exp_next;
        void distribute_sp(string level_text);
        void death(Enemy &enemy);
        void level_up();

};

#endif // PLAYER_H
