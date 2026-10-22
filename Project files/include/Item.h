#ifndef ITEM_H
#define ITEM_H
#include <iostream>
using namespace std;


class Item
{
    public:
        Item();
        virtual ~Item();

        string name;
        int level;
        struct item_stats {
        int str, agi, con, wis;
        } stats = {0, 0, 0, 0};
        string slot;
        int value;
        bool equipped;

    protected:

    private:
};

#endif // ITEM_H
