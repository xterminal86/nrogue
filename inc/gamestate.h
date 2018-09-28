#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <ncurses.h>

#include <typeinfo>

class GameState
{
	public:
    virtual void Init() {}
    virtual void HandleInput() = 0;
    virtual void Update() = 0;    
};

#endif
