#include "npc-interact-state.h"

#include "application.h"
#include "constants.h"
#include "printer.h"
#include "util.h"
#include "ai-npc.h"
#include "ai-component.h"
#include "trader-component.h"
#include "shopping-state.h"

void NPCInteractState::Cleanup()
{
  _npcRef = nullptr;
  _gossipBlockIndex = 0;
  _animatedTextFinished.clear();
  _whatKey = WhatKey::NONE;
}

void NPCInteractState::HandleInput()
{
  if (_textPrinting)
  {
    return;
  }

  _keyPressed = GetKeyDown();

  switch (_keyPressed)
  {
    case 'n':
      _whatKey = WhatKey::NAME;
      _textPrinting = true;
      break;

    case 'j':
    {
      TraderComponent* tc = _npcRef->AIComponentRef->OwnerGameObject->GetComponent<TraderComponent>();
      if (tc != nullptr)
      {
        auto state = Application::Instance().GetGameStateRefByName(GameStates::SHOPPING_STATE);
        ShoppingState* ss = static_cast<ShoppingState*>(state);
        ss->PassShopOwner(tc);
        Application::Instance().ChangeState(GameStates::SHOPPING_STATE);
      }
      else
      {
        _whatKey = WhatKey::JOB;
        _textPrinting = true;
      }
    }
    break;

    case 'g':
      _whatKey = WhatKey::GOSSIP;
      _textPrinting = true;
      break;

    case 'q':
    {
      std::string msg;
      if (_npcRef->Data.IsAquainted)
      {
        auto name = _npcRef->Data.Name;
        auto title = _npcRef->Data.Job;

        msg = Util::StringFormat("You finished speaking with %s the %s", name.data(), title.data());
      }
      else
      {
        auto name = _npcRef->AIComponentRef->OwnerGameObject->ObjectName;
        msg = Util::StringFormat("You finished speaking with %s", name.data());
      }

      Printer::Instance().AddMessage(msg);
      Application::Instance().ChangeState(GameStates::MAIN_STATE);
    }
    break;

    default:
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

  PrintHeader();
  PrintFooter();

  _animatedTextFinished.clear();

  int linePosY = 2;

  std::vector<std::string> block;

  switch (_whatKey)
  {
    case WhatKey::NAME:
      _npcRef->Data.IsAquainted = true;
      block = { _npcRef->Data.NameResponse };
      break;

    case WhatKey::JOB:
      block = { _npcRef->Data.JobResponse };
      break;

    case WhatKey::GOSSIP:
      block = _npcRef->Data.GossipResponsesByMap.at(MapType::TOWN)[_gossipBlockIndex];
      break;
  }

  _animatedTextFinished = block;

  for (auto& line : block)
  {
    int cursorPos = 0;

    std::string l = line;
    for (int i = 0; i < l.length(); i++)
    {
      cursorPos = i + 1;

      Printer::Instance().PrintFB(cursorPos, linePosY, ' ',  "#000000", "#FFFFFF");

      Printer::Instance().PrintFB(i, linePosY, l[i], "#FFFFFF");
      Printer::Instance().Render();
      Util::Sleep(10);
    }

    Printer::Instance().PrintFB(cursorPos, linePosY, ' ',  "#000000");

    linePosY++;
  }

  if (_whatKey == WhatKey::GOSSIP)
  {
    _gossipBlockIndex++;

    int maxBlocks = _npcRef->Data.GossipResponsesByMap.at(MapType::TOWN).size();

    if (_gossipBlockIndex > maxBlocks - 1)
    {
      _gossipBlockIndex = 0;
    }
  }

  _textPrinting = false;

  DisplayStillText();
}

void NPCInteractState::DisplayStillText()
{
  Printer::Instance().Clear();

  PrintHeader();

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

void NPCInteractState::PrintHeader()
{
  int tw = Printer::Instance().TerminalWidth;

  std::string desc;

  if (_npcRef->Data.IsAquainted)
  {
    auto str = Util::StringFormat(" %s the %s ", _npcRef->Data.Name.data(), _npcRef->Data.Job.data());
    desc = str;
  }
  else
  {
    desc = " ??? ";
  }

  DrawHeader(desc);
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
