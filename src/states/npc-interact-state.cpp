#include "npc-interact-state.h"

#include "application.h"
#include "printer.h"
#include "util.h"
#include "ai-npc.h"
#include "ai-component.h"
#include "trader-component.h"
#include "shopping-state.h"

void NPCInteractState::Prepare()
{
  _charPos = 0;
  _currentLine = 0;

  Printer::Instance().Clear();

  PrintHeader();
  PrintFooter();

  Printer::Instance().Render();
}

void NPCInteractState::Cleanup()
{
  _npcRef = nullptr;
  _gossipBlockIndex = 0;
  _whatKey = WhatKey::NONE;
  _textPrinting = false;
  _charPos = 0;
  _currentLine = 0;
  _blockToPrint.clear();
  Util::WaitForMs(0, true);
}

void NPCInteractState::HandleInput()
{  
  _keyPressed = GetKeyDown();

  switch (_keyPressed)
  {
    case 'n':
      _whatKey = WhatKey::NAME;
      _npcRef->Data.IsAquainted = true;
      _blockToPrint = { _npcRef->Data.NameResponse };
      _textPrinting = true;
      Prepare();
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
        _blockToPrint = { _npcRef->Data.JobResponse };
        _textPrinting = true;
        Prepare();
      }
    }
    break;

    case 'g':
    {
      _gossipBlockIndex++;

      int maxBlocks = _npcRef->Data.GossipResponsesByMap.at(MapType::TOWN).size();
      if (_gossipBlockIndex > maxBlocks - 1)
      {
        _gossipBlockIndex = 0;
      }

      _whatKey = WhatKey::GOSSIP;
      _blockToPrint = _npcRef->Data.GossipResponsesByMap.at(MapType::TOWN)[_gossipBlockIndex];
      _textPrinting = true;
      Prepare();
    }
    break;

    case VK_CANCEL:
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
    if (Util::WaitForMs(10))
    {
      AnimateText();
    }
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
  // To print 'Listening...' during animation
  PrintFooter();

  auto line = _blockToPrint[_currentLine];

  Printer::Instance().PrintFB(_charPos + 1, _currentLine + 2, ' ',  "#000000", "#FFFFFF");
  Printer::Instance().PrintFB(_charPos, _currentLine + 2, line[_charPos], "#FFFFFF");

  Printer::Instance().Render();

  _charPos++;

  if (_charPos >= line.length())
  {
    Printer::Instance().PrintFB(_charPos, _currentLine + 2, ' ', "#000000");
    _charPos = 0;
    _currentLine++;
  }

  if (_currentLine >= _blockToPrint.size())
  {
    _textPrinting = false;
    DisplayStillText();
  }
}

void NPCInteractState::DisplayStillText()
{
  Printer::Instance().Clear();

  PrintHeader();

  int yPos = 2;
  for (auto& l : _blockToPrint)
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
  Printer::Instance().PrintFB(tw / 2 + part * 2, th - 1, "'Q' - bye", Printer::kAlignRight, "#FFFFFF");
}
