#ifndef AIDUMMY_H
#define AIDUMMY_H

#include "component.h"

class AIDummy : public Component
{
  public:
    AIDummy()
    {
      _hash = typeid(*this).hash_code();      
    }

    void Update() override;  
};

#endif // AIDUMMY_H
