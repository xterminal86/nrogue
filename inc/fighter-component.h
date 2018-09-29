#ifndef FIGHTER_H
#define FIGHTER_H

#include "component.h"

class FighterComponent : public Component
{
  public:
    FighterComponent()
    {
      _hash = typeid(*this).hash_code();
    }
    
    void Update() override
    {
      printf("dbiwuncowieco\n");
    }
};

#endif
