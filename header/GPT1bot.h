#ifndef GPT1BOT_H
#define GPT1BOT_H

#include "player.h"

class GPT1bot : public Player {
    public:
        GPT1bot(int initialStack);  // Konstruktor
        Action makeAction(int raiseMoney, int pot, int tableButton);
    };
    

#endif