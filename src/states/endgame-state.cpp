#include "endgame-state.h"
#include "printer.h"
#include "map.h"
#include "application.h"
#include "util.h"

void EndgameState::Init()
{
  _playerRef = &Application::Instance().PlayerInstance;
}

void EndgameState::Prepare()
{
  _playerRef->SetDestroyed();

  Printer::Instance().AddMessage("You are dead. Not big surprise.");

  Application::Instance().WriteObituary();
}

void EndgameState::HandleInput()
{
  _keyPressed = GetKeyDown();

  switch (_keyPressed)
  {
    case VK_CANCEL:
      Application::Instance().ChangeState(GameStates::EXIT_GAME);
      break;

    default:
      break;
  }
}

void EndgameState::Update(bool forceUpdate)
{
  if (_keyPressed != -1 || forceUpdate)
  {
    Printer::Instance().Clear();

    Map::Instance().Draw();

    _playerRef->Draw();

    DrawHPMP();

    if (Printer::Instance().ShowLastMessage)
    {
      DisplayGameLog();
    }

    Printer::Instance().PrintFB(_twHalf,
                                0,
                                "Press 'q' to exit",
                                Printer::kAlignCenter,
                                Colors::WhiteColor);

    Printer::Instance().Render();
  }
}

void EndgameState::DisplayGameLog()
{
  int x = Printer::TerminalWidth - 1;
  int y = Printer::TerminalHeight - 1;

  int count = 0;
  auto msgs = Printer::Instance().GetLastMessages();
  for (GameLogMessageData& m : msgs)
  {
    Printer::Instance().PrintFB(x,
                                y - count,
                                m.Message,
                                Printer::kAlignRight,
                                m.FgColor,
                                m.BgColor);
    count++;
  }
}

void EndgameState::DrawHPMP()
{
  int curHp = _playerRef->Attrs.HP.Min().Get();
  int maxHp = _playerRef->Attrs.HP.Max().Get();
  int curMp = _playerRef->Attrs.MP.Min().Get();
  int maxMp = _playerRef->Attrs.MP.Max().Get();

  UpdateBar(0, _th - 2, _playerRef->Attrs.HP);

  auto str = Util::StringFormat("%i/%i", curHp, maxHp);
  Printer::Instance().PrintFB(GlobalConstants::HPMPBarLength / 2,
                              _th - 2,
                              str,
                              Printer::kAlignCenter,
                              Colors::WhiteColor,
                              "#880000");

  UpdateBar(0, _th - 1, _playerRef->Attrs.MP);

  str = Util::StringFormat("%i/%i", curMp, maxMp);
  Printer::Instance().PrintFB(GlobalConstants::HPMPBarLength / 2,
                              _th - 1,
                              str,
                              Printer::kAlignCenter,
                              Colors::WhiteColor,
                              "#000088");
}

std::string EndgameState::UpdateBar(int x, int y, RangedAttribute& attr)
{
  float ratio = ((float)attr.Min().Get() / (float)attr.Max().Get());
  int len = ratio * GlobalConstants::HPMPBarLength;

  std::string bar = "[";
  for (int i = 0; i < GlobalConstants::HPMPBarLength; i++)
  {
    bar += (i < len) ? "=" : " ";
  }

  bar += "]";

  Printer::Instance().PrintFB(x, y, bar, Printer::kAlignLeft, Colors::WhiteColor);

  return bar;
}
