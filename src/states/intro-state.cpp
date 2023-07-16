#include "intro-state.h"
#include "printer.h"

#include "application.h"
#include "map.h"
#include "map-level-town.h"

void IntroState::Prepare()
{
  Printer::Instance().Clear();

  _textPositionCursor = 0;
  _textPositionX = _twHalf;

  int textIndex = Application::Instance().PlayerInstance.SelectedClass;
  _textPositionY = _thHalf - _introStrings[textIndex].size() / 2;

  Util::WaitForMs(0, true);
}

// =============================================================================

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

// =============================================================================

void IntroState::PrepareTown()
{
  Printer::Instance().Clear();
  Printer::Instance().Render();

  Map::Instance().ChangeOrInstantiateLevel(MapType::TOWN);

  auto& curLvl = Map::Instance().CurrentLevel;
  auto& playerRef = Application::Instance().PlayerInstance;

  //
  // Some NPCs contain bonus lines
  // depending on selected player character class and stats,
  // as well as other initializations (e.g. food cost in shops
  // depends on player hunger rate which is determined by selected class)
  // so we need to initialize player first.
  //
  playerRef.SetLevelOwner(curLvl);
  playerRef.Init();
  playerRef.MoveTo({ 5, 8 });
  playerRef.AddExtraItems();
  playerRef.VisibilityRadius.Set(curLvl->VisibilityRadius);

  curLvl->AdjustCamera();

  MapLevelTown* mlt = static_cast<MapLevelTown*>(curLvl);
  mlt->CreateNPCs();
}

// =============================================================================

void IntroState::Update(bool forceUpdate)
{
  Printer::Instance().PrintFB(_twHalf,
                              (Printer::TerminalHeight - _introStrings[Application::Instance().PlayerInstance.SelectedClass].size()) / 4,
                              _scenarioNameByClass.at(Application::Instance().PlayerInstance.GetClass()),
                              2,
                              Printer::kAlignCenter,
                              Colors::WhiteColor,
                              Colors::BlackColor);
  if (Util::WaitForMs(10))
  {
    int textIndex = Application::Instance().PlayerInstance.SelectedClass;
    if (_stringIndex != _introStrings[textIndex].size())
    {
      int len = _introStrings[textIndex][_stringIndex].length();

      if (_textPositionCursor != _introStrings[textIndex][_stringIndex].length())
      {
        Printer::Instance().PrintFB(_textPositionX - len / 2,
                                    _textPositionY,
                                    _introStrings[textIndex][_stringIndex][_textPositionCursor],
                                    Colors::WhiteColor);

        Printer::Instance().PrintFB(_textPositionX - len / 2 + 1,
                                    _textPositionY,
                                    ' ',
                                    Colors::BlackColor,
                                    Colors::WhiteColor);

        _textPositionX++;
        _textPositionCursor++;
      }
      else
      {
        Printer::Instance().PrintFB(_textPositionX - len / 2,
                                    _textPositionY,
                                    ' ',
                                    Colors::BlackColor);

        _textPositionCursor = 0;

        _textPositionX = _twHalf;
        _textPositionY++;

        _stringIndex++;
      }
    }
    else
    {
      Printer::Instance().PrintFB(_twHalf,
                                  Printer::TerminalHeight - 1,
                                  "Press 'Enter' to continue",
                                  Printer::kAlignCenter,
                                  Colors::WhiteColor);
    }

    Printer::Instance().Render();
  }
}
