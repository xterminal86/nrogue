#ifndef ENTERNAMESTATE_H
#define ENTERNAMESTATE_H

#include <string>

#include "gamestate.h"

class EnterNameState : public GameState
{
  public:
    void Prepare() override;
    void Update(bool forceUpdate = false) override;
    void HandleInput() override;

  private:
    enum class InputType
    {
      NAME = 0,
      SEED_STRING,
      SEED_HEX,
      ENUM_END
    };

    std::string _nameEntered;
    std::string _seedEntered;

    std::string _seedAsHex;

    const std::string kEnterNameString = "Enter your name";
    const std::string kEnterSeedString = "Enter seed (leave blank for random)";

    const int MaxNameLength = 24;
    const int MaxSeedStringLength = 65;

    InputType _inputType = InputType::NAME;

    int _x = 0;
    int _y = 0;
    int _cursorPos = 0;

    const int _maxNameHalf = MaxNameLength / 2;
    const int _maxSeedHalf = MaxSeedStringLength / 2;

    int _inputTypeIndex = 0;

    size_t _seedHex = 0;

    const std::map<size_t, InputType> _inputTypeByIndex =
    {
      { 0, InputType::NAME        },
      { 1, InputType::SEED_STRING },
      { 2, InputType::SEED_HEX    }
    };    

    void UpdateSeedAsHex();
};

#endif // ENTERNAMESTATE_H
