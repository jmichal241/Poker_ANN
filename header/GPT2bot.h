#ifndef GPT2BOT_H
#define GPT2BOT_H

#include "player.h"

class GPT2bot : public Player {
    public:
        GPT2bot(int initialStack);  // Konstruktor
        Action makeAction(int raiseMoney, int pot, int tableButton);
    };
    

#endif