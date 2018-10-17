#ifndef INFOSTATE_H
#define INFOSTATE_H

#include "gamestate.h"
#include "constants.h"

class InfoState : public GameState
{
  public:
    void HandleInput() override;
    void Update(bool forceUpdate = false) override;

  private:
    void ShowInventory();
    void PrintAttribute(int x, int y, std::string attrName, Attribute& attr);
};

#endif
