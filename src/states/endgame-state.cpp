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
  Application::Instance().WriteObituary();
}

void EndgameState::HandleInput()
{
  _keyPressed = GetKeyDown();

  switch (_keyPressed)
  {
    case 'q':
      Application::Instance().ChangeState(GameStates::EXIT_GAME);
      break;
  }
}

void EndgameState::Update(bool forceUpdate)
{
  if (_keyPressed != -1 || forceUpdate)
  {
    Printer::Instance().Clear();

    Map::Instance().Draw(_playerRef->PosX, _playerRef->PosY);

    _playerRef->Draw();

    DrawHPMP();

    if (Printer::Instance().ShowLastMessage)
    {
      DisplayGameLog();
    }

    Printer::Instance().PrintFB(Printer::Instance().TerminalWidth / 2, 0, "Press 'q' to exit", Printer::kAlignCenter, "#FFFFFF");

    Printer::Instance().Render();
  }
}

void EndgameState::DisplayGameLog()
{
  int x = Printer::Instance().TerminalWidth - 1;
  int y = Printer::Instance().TerminalHeight - 1;

  //Printer::Instance().PrintFB(x, y, Printer::Instance().GetLastMessage(), Printer::kAlignRight, "#FFFFFF");

  int count = 0;
  auto msgs = Printer::Instance().GetLastMessages();
  for (auto& m : msgs)
  {
    Printer::Instance().PrintFB(x, y - count, m, Printer::kAlignRight, "#FFFFFF");
    count++;
  }
}

void EndgameState::DrawHPMP()
{
  int curHp = _playerRef->Attrs.HP.CurrentValue;
  int maxHp = _playerRef->Attrs.HP.OriginalValue;
  int curMp = _playerRef->Attrs.MP.CurrentValue;
  int maxMp = _playerRef->Attrs.MP.OriginalValue;

  int th = Printer::Instance().TerminalHeight;

  UpdateBar(0, th - 2, _playerRef->Attrs.HP);

  auto str = Util::StringFormat("%i/%i", curHp, maxHp);
  Printer::Instance().PrintFB(GlobalConstants::HPMPBarLength / 2, th - 2, str, Printer::kAlignCenter, "#FFFFFF", "#880000");

  UpdateBar(0, th - 1, _playerRef->Attrs.MP);

  str = Util::StringFormat("%i/%i", curMp, maxMp);
  Printer::Instance().PrintFB(GlobalConstants::HPMPBarLength / 2, th - 1, str, Printer::kAlignCenter, "#FFFFFF", "#000088");
}

std::string EndgameState::UpdateBar(int x, int y, Attribute attr)
{
  float ratio = ((float)attr.CurrentValue / (float)attr.OriginalValue);
  int len = ratio * GlobalConstants::HPMPBarLength;

  std::string bar = "[";
  for (int i = 0; i < GlobalConstants::HPMPBarLength; i++)
  {
    bar += (i < len) ? "=" : " ";
  }

  bar += "]";

  Printer::Instance().PrintFB(x, y, bar, Printer::kAlignLeft, "#FFFFFF");

  return bar;
}
