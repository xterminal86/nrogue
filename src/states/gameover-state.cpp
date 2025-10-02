#include "gameover-state.h"
#include "printer.h"
#include "map.h"
#include "application.h"
#include "util.h"

void GameOverState::Init()
{
  _playerRef = &Game::gApp.PlayerInstance;
}

// =============================================================================

void GameOverState::Prepare()
{
  _playerRef->SetDestroyed();

  Game::gPrnt.AddMessage("You are dead. Not big surprise.");

  Game::gApp.WriteObituary();
}

// =============================================================================

void GameOverState::HandleInput()
{
  _keyPressed = GetKeyDown();

  switch (_keyPressed)
  {
    case VK_CANCEL:
    case 'n':
    case 'N':
      Game::gApp.ChangeState(GameStates::EXIT_GAME);
      break;

    default:
      break;
  }
}

// =============================================================================

void GameOverState::Update(bool forceUpdate)
{
  if (_keyPressed != -1 || forceUpdate)
  {
    Game::gPrnt.Clear();

    Game::gMap.Draw();

    _playerRef->Draw();

    DrawHPMP();

    if (Game::gPrnt.ShowLastMessage)
    {
      DisplayGameLog();
    }

    Game::gPrnt.PrintFB(_twHalf,
                        0,
                        "Press 'q' to exit",
                        Printer::kAlignCenter,
                        Colors::WhiteColor,
                        Colors::BlackColor);

    Game::gPrnt.Render();
  }
}

// =============================================================================

void GameOverState::DisplayGameLog()
{
  int x = Printer::TerminalWidth - 1;
  int y = Printer::TerminalHeight - 1;

  int count = 0;
  auto msgs = Game::gPrnt.GetLastMessages();
  for (GameLogMessageData* m : msgs)
  {
    if (m == nullptr)
    {
      break;
    }

    Game::gPrnt.PrintFB(x,
                        y - count,
                        m->Message,
                        Printer::kAlignRight,
                        m->FgColor,
                        m->BgColor);
    count++;
  }
}

// =============================================================================

void GameOverState::DrawHPMP()
{
  int curHp = _playerRef->Attrs.HP.Min().Get();
  int maxHp = _playerRef->Attrs.HP.Max().Get();
  int curMp = _playerRef->Attrs.MP.Min().Get();
  int maxMp = _playerRef->Attrs.MP.Max().Get();

  UpdateBar(0, _th - 2, _playerRef->Attrs.HP);

  auto str = Util::StringFormat("%i/%i", curHp, maxHp);
  Game::gPrnt.PrintFB(GlobalConstants::HPMPBarLength / 2,
                      _th - 2,
                      str,
                      Printer::kAlignCenter,
                      Colors::WhiteColor,
                      0x880000);

  UpdateBar(0, _th - 1, _playerRef->Attrs.MP);

  str = Util::StringFormat("%i/%i", curMp, maxMp);
  Game::gPrnt.PrintFB(GlobalConstants::HPMPBarLength / 2,
                      _th - 1,
                      str,
                      Printer::kAlignCenter,
                      Colors::WhiteColor,
                      0x000088);
}

// =============================================================================

void GameOverState::UpdateBar(int x, int y, RangedAttribute& attr)
{
  double ratio = ((double)attr.Min().Get() / (double)attr.Max().Get());
  int len = ratio * GlobalConstants::HPMPBarLength;

  _bar = "[";
  for (int i = 0; i < GlobalConstants::HPMPBarLength; i++)
  {
    _bar += (i < len) ? "=" : " ";
  }

  _bar += "]";

  Game::gPrnt.PrintFB(x,
                      y,
                      _bar,
                      Printer::kAlignLeft,
                      Colors::WhiteColor,
                      Colors::BlackColor);
}
