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
    void PrintAttribute(int x, int y, const std::string& attrName, Attribute& attr);
    void PrintRangedAttribute(int x, int y, const std::string& attrName, RangedAttribute& attr);
    void PrintModifiers(int x, int y);
    void PrepareUseIdentifiedListToPrint();
    void DrawScrollBars();

    int FindAttrsMaxStringLength();

    std::pair<std::string, std::string> GetModifierString(int value);

    std::map<int, std::pair<std::string, std::string>> _useIdentifiedMapCopy;

    const int kMaxNameUnderscoreLength = 33;

    size_t _scrollIndex = 0;

    Player* _playerRef = nullptr;

    bool _scrollLimitReached = false;
};

#endif
