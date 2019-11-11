#include "intro-state.h"
#include "printer.h"

#include "application.h"
#include "map.h"
#include "map-level-town.h"

void IntroState::Prepare()
{
  Printer::Instance().Clear();  

  _textPositionCursor = 0;
  _textPositionX = Printer::Instance().TerminalWidth / 2;

  int textIndex = Application::Instance().PlayerInstance.SelectedClass;
  _textPositionY = Printer::Instance().TerminalHeight / 2 - _introStrings[textIndex].size() / 2;  
}

void IntroState::HandleInput()
{
  _keyPressed = GetKeyDown();

  switch (_keyPressed)
  {
    case VK_ENTER:
    {
      PrepareTown();

      // FIXME: debug
      // OverrideStartingLevel<MapLevelMines>(MapType::MINES_5, { 60, 60 });

      Application::Instance().ChangeState(GameStates::MAIN_STATE);
    }
    break;

    default:
      break;
  }
}

void IntroState::PrepareTown()
{
  Printer::Instance().Clear();

  Map::Instance().ChangeOrInstantiateLevel(MapType::TOWN);

  auto& curLvl = Map::Instance().CurrentLevel;
  auto& playerRef = Application::Instance().PlayerInstance;

  // Some NPCs contain bonus lines
  // depending on selected player character class.
  MapLevelTown* mlt = static_cast<MapLevelTown*>(curLvl);
  mlt->CreateNPCs();

  playerRef.SetLevelOwner(curLvl);
  playerRef.Init();

  playerRef.AddBonusItems();
  playerRef.VisibilityRadius.Set(curLvl->VisibilityRadius);
}

void IntroState::Update(bool forceUpdate)
{
  if (Util::WaitForMs(10))
  {    
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
