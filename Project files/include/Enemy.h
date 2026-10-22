#ifndef ENEMY_H
#define ENEMY_H
#include "Character.h"
#include <stdlib.h>
#include <time.h>
#include "Player.h"
#include "Item.h"

class Player;
class Item;

class Enemy : public Character {

    public:
        Enemy();
        virtual ~Enemy();

        void initialize_enemy(string ename, int elevel);
        void get_stats();
        void take_damage(int damage, Player &player);
        string get_name();
        void attack(Player &player, Enemy &enemy);
        void add_drop(string name, double drop_chance);
        void get_drops();

    protected:

    private:

        void death(Player &player);
        void drop(Player &player);
        vector<string> available_drops;
        vector<double> available_drop_chances;
        void roll_drops(Player &player);
};

#endif // ENEMY_H
