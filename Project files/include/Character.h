#ifndef CHARACTER_H
#define CHARACTER_H
#include <string>
#include <iostream>
#include <fstream>
#include <math.h>
#include <vector>
#include "Console.h"

using namespace std;


class Character
{
    public:

        Character();
        virtual ~Character();

        void draw_bars();
        void wait();
        bool next_turn;
        void reset_turn();
        void reset_timer();
        bool is_dead;
        int level;
        string name;

        struct skill {
            string name;
            int cost;
            string effect;
            string amount;
        };

        vector<skill> skills;

        void clear_buffs();
        void update_stats();

    protected:

        struct buff_stack {
            int con, wis, agi, str;
        } buffs = {0, 0, 0, 0,};

        int hp, hp_max, mp, mp_max, str, con, agi, wis, turn_timer, turn_length;
        int calc_hp();
        int calc_mp();
        void set_hmp(int mode = 0);
        vector<string> attack_words;

    private:

};

#endif // CHARACTER_H
