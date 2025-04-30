#ifndef INFOSTATE_H
#define INFOSTATE_H

#include "gamestate.h"

class Player;
class Attribute;
class RangedAttribute;

class InfoState : public GameState
{
  public:
    void Prepare() override;
    void HandleInput() override;
    void Update(bool forceUpdate = false) override;

  private:
    void PrintExp(int x, int y);
    void PrintAttribute(int x,
                        int y,
                        const std::string& attrName,
                        Attribute& attr);

    void PrintRangedAttribute(int x,
                              int y,
                              const std::string& attrName,
                              RangedAttribute& attr);

    void PrintModifiers(int x, int y);

    int FindAttrsMaxStringLength();

    std::pair<uint32_t, std::string> GetModifierString(int value);

    const int kMaxNameUnderscoreLength = 33;

    Player* _playerRef = nullptr;
};

#endif
