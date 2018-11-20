#include "npc-interact-state.h"

#include "application.h"
#include "constants.h"
#include "printer.h"
#include "util.h"
#include "ai-npc.h"

void NPCInteractState::Cleanup()
{
  _npcRef = nullptr;
  _gossipBlockIndex = 0;
  _animatedTextFinished.clear();
}

void NPCInteractState::HandleInput()
{
  if (_textPrinting)
  {
    return;
  }

  _keyPressed = getch();

  switch (_keyPressed)
  {
    case 'g':
      _textPrinting = true;
      break;

    case 'q':
      Application::Instance().ChangeState(GameStates::MAIN_STATE);
      break;
  }
}

void NPCInteractState::Update(bool forceUpdate)
{
  if (_textPrinting)
  {
    AnimateText();
  }
  else
  {
    if (_keyPressed != -1 || forceUpdate)
    {
      DisplayStillText();
    }
  }
}

void NPCInteractState::AnimateText()
{
  Printer::Instance().Clear();

  int tw = Printer::Instance().TerminalWidth;
  int th = Printer::Instance().TerminalHeight;

  Printer::Instance().PrintFB(tw / 2, 0, _npcRef->Data().UnacquaintedDescription, Printer::kAlignCenter, "#FFFFFF");

  PrintFooter();

  _animatedTextFinished.clear();

  int linePosY = 2;

  auto block = _npcRef->Data().GossipResponsesByMap.at(MapType::TOWN)[_gossipBlockIndex];

  _animatedTextFinished = block;

  for (auto& line : block)
  {
    std::string l = line;
    for (int i = 0; i < l.length(); i++)
    {
      Printer::Instance().PrintFB(i, linePosY, l[i], "#FFFFFF");
      Printer::Instance().Render();
      Util::Sleep(10);
    }

    linePosY++;
  }

  _gossipBlockIndex++;

  int maxBlocks = _npcRef->Data().GossipResponsesByMap.at(MapType::TOWN).size();

  if (_gossipBlockIndex > maxBlocks - 1)
  {
    _gossipBlockIndex = 0;
  }

  _textPrinting = false;

  DisplayStillText();
}

void NPCInteractState::DisplayStillText()
{
  Printer::Instance().Clear();

  int tw = Printer::Instance().TerminalWidth;
  int th = Printer::Instance().TerminalHeight;

  Printer::Instance().PrintFB(tw / 2, 0, _npcRef->Data().UnacquaintedDescription, Printer::kAlignCenter, "#FFFFFF");

  int yPos = 2;
  for (auto& l : _animatedTextFinished)
  {
    Printer::Instance().PrintFB(0, yPos, l, Printer::kAlignLeft, "#FFFFFF");
    yPos++;
  }

  PrintFooter();

  Printer::Instance().Render();
}

void NPCInteractState::SetNPCRef(AINPC* npcRef)
{
  _npcRef = npcRef;
}

void NPCInteractState::PrintFooter()
{
  int tw = Printer::Instance().TerminalWidth;
  int th = Printer::Instance().TerminalHeight;

  if (_textPrinting)
  {
    Printer::Instance().PrintFB(tw / 2, th - 1, "Listening...", Printer::kAlignCenter, "#FFFFFF");
    return;
  }

  int part = tw / 5;

  Printer::Instance().PrintFB(tw / 2 - part * 2, th - 1, "'n' - name", Printer::kAlignLeft, "#FFFFFF");
  Printer::Instance().PrintFB(tw / 2 - part, th - 1, "'j' - job", Printer::kAlignLeft, "#FFFFFF");
  Printer::Instance().PrintFB(tw / 2 + part, th - 1, "'g' - gossip", Printer::kAlignRight, "#FFFFFF");
  Printer::Instance().PrintFB(tw / 2 + part * 2, th - 1, "'q' - bye", Printer::kAlignRight, "#FFFFFF");
}
