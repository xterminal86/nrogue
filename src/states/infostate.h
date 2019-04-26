#ifndef INFOSTATE_H
#define INFOSTATE_H

#include "gamestate.h"

class Attribute;

class InfoState : public GameState
{
  public:
    void HandleInput() override;
    void Update(bool forceUpdate = false) override;

  private:
    void PrintAttribute(int x, int y, const std::string& attrName, const Attribute& attr, bool displayMaxValue = false);
    void PrintModifiers(int x, int y);

    int FindAttrsMaxStringLength();

    std::pair<std::string, std::string> GetModifierString(int value);

    const int kMaxNameUnderscoreLength = 33;
};

#endif
