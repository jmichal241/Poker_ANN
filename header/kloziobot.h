#ifndef KLOZIOBOT_H
#define KLOZIOBOT_H

#include "player.h"

class Kloziobot : public Player {
public:
    Kloziobot(int initialStack);

    // Kloziobot class
    Action makeAction(int raiseMoney, int pot, int tableButton);

};

#endif
