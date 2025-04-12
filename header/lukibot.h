#ifndef LUKIBOT_H
#define LUKIBOT_H

#include "player.h"

class Lukibot : public Player {
public:
    Lukibot(int initialStack);

    // Kloziobot class
    Action makeAction(int raiseMoney, int pot, int tableButton);

};

#endif
