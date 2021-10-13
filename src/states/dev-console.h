#ifndef DEVCONSOLE_H
#define DEVCONSOLE_H

#include "gamestate.h"

class DevConsole : public GameState
{
  public:
    void Init() override;
    void Prepare() override;
    void Cleanup() override;
    void HandleInput() override;
    void Update(bool forceUpdate = false) override;

  private:
    std::string _toEnter;

    std::vector<std::string> _entered;

    int _cursorX = 1;
    int _cursorY = 2;

    int _lineIndex = 0;
};

#endif // DEVCONSOLE_H
