#include "intro-state.h"
#include "printer.h"

#include "application.h"
#include "game-objects-factory.h"
#include "map.h"

void IntroState::Prepare()
{
  Printer::Instance().Clear();

  _lastTime = std::chrono::high_resolution_clock::now();

  _textPositionCursor = 0;
  _textPositionX = Printer::Instance().TerminalWidth / 2;

  int textIndex = Application::Instance().PlayerInstance.SelectedClass;
  _textPositionY = Printer::Instance().TerminalHeight / 2 - _introStrings[textIndex].size() / 2;

  AddBonusItems();
}

void IntroState::HandleInput()
{
  _keyPressed = getch();

  switch (_keyPressed)
  {
    case VK_ENTER:
      Application::Instance().ChangeState(Application::GameStates::MAIN_STATE);
      break;
  }
}

void IntroState::Update(bool forceUpdate)
{
  auto elapsed = std::chrono::high_resolution_clock::now() - _lastTime;
  auto msPassed = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed);

  if (msPassed > kTextPrintDelay)
  {
    _lastTime = std::chrono::high_resolution_clock::now();

    int textIndex = Application::Instance().PlayerInstance.SelectedClass;
    if (_stringIndex != _introStrings[textIndex].size())
    {
      if (_textPositionCursor != _introStrings[textIndex][_stringIndex].length())
      {
        int len = _introStrings[textIndex][_stringIndex].length();

        Printer::Instance().PrintFB(_textPositionX - len / 2, _textPositionY, _introStrings[textIndex][_stringIndex][_textPositionCursor], "#FFFFFF");

        _textPositionX++;

        _textPositionCursor++;
      }
      else
      {
        _textPositionCursor = 0;

        _textPositionX = Printer::Instance().TerminalWidth / 2;
        _textPositionY++;

        _stringIndex++;
      }
    }
    else
    {
      Printer::Instance().PrintFB(Printer::Instance().TerminalWidth / 2, Printer::Instance().TerminalHeight - 1, "Press 'Enter' to continue", Printer::kAlignCenter, "#FFFFFF");
    }
  }

  Printer::Instance().Render();
}

void IntroState::AddBonusItems()
{
  GameObject* go = nullptr;
  std::vector<std::string> text;

  auto& player = Application::Instance().PlayerInstance;

  switch(player.GetClass())
  {
    case Player::PlayerClass::THIEF:
    {
      text =
      {
        "REWARD 10,000",
        "",
        "For the capture dead or alive of one",
        "",
        player.Name,
        "",
        "the leader of the worst band of thieves",
        "the City has ever had to deal with.",
        "The above reward will be paid",
        "for his / her capture",
        "or positive proof of death.",
        "",
        "Signed:",
        "NORMAN TRUART, Sheriff",
        "City Watch HQ",
        "21 / II / 988"
      };

      go = GameObjectsFactory::Instance().CreateNote("Wanted Poster", text);
      player.Inventory.AddToInventory(go);
    }
    break;

    case Player::PlayerClass::SOLDIER:
    {
      text =
      {
        "Pvt. " + player.Name,
        "is hereby granted permission",
        "to take a leave of absence",
        "from 13 / III / 988",
        "until 13 / IV / 988",
        "",
        "Signed:",
        "Lt. LAURA MOSLEY",
        "City Watch, Stonemarket Dpt.",
        "21 / II / 988"
      };

      go = GameObjectsFactory::Instance().CreateNote("Leave Warrant", text);
      player.Inventory.AddToInventory(go);
    }
    break;

    case Player::PlayerClass::ARCANIST:
    {
      text =
      {
        player.Name,
        "",
        "The Order instructs you to visit",
        Map::Instance().CurrentLevel->LevelName,
        "Investigate the place as thorough as possible",
        "for we believe there is a disturbance",
        "lurking there somewhere.",
        "Find the source of this unbalance,",
        "restore it and come back to us.",
        "We'll be praying for your success.",
        "",
        "Signed:",
        "+ Bishop DANIEL"
      };

      go = GameObjectsFactory::Instance().CreateNote("Orders", text);
      player.Inventory.AddToInventory(go);
    }
    break;
  }
}
