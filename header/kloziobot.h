#ifndef KLOZIOBOT_H
#define KLOZIOBOT_H

#include "player.h"

class Kloziobot : public Player {
public:
    Kloziobot(int initialStack);

    Action makeAction(int,int,int) override;
};

#endif
