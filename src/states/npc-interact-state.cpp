#include "npc-interact-state.h"

#include "application.h"
#include "printer.h"
#include "util.h"
#include "ai-npc.h"
#include "ai-component.h"
#include "service-state.h"
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

    case 's':
    {
      if (_npcRef->Data.ProvidesService != ServiceType::NONE)
      {
        TraderComponent* tc = _npcRef->AIComponentRef->OwnerGameObject->GetComponent<TraderComponent>();
        auto s = Application::Instance().GetGameStateRefByName(GameStates::SERVICE_STATE);
        ServiceState* ss = static_cast<ServiceState*>(s);
        ss->Setup(tc);
        Application::Instance().ChangeState(GameStates::SERVICE_STATE);
      }
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

  Printer::Instance().PrintFB(_charPos + 1, _currentLine + 2, ' ',  GlobalConstants::BlackColor, GlobalConstants::WhiteColor);
  Printer::Instance().PrintFB(_charPos, _currentLine + 2, line[_charPos], GlobalConstants::WhiteColor);

  Printer::Instance().Render();

  _charPos++;

  if (_charPos >= line.length())
  {
    Printer::Instance().PrintFB(_charPos, _currentLine + 2, ' ', GlobalConstants::BlackColor);
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
    Printer::Instance().PrintFB(0, yPos, l, Printer::kAlignLeft, GlobalConstants::WhiteColor);
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
  size_t tw = Printer::TerminalWidth;
  size_t th = Printer::TerminalHeight;

  if (_textPrinting)
  {
    Printer::Instance().PrintFB(tw / 2, th - 1, "Listening...", Printer::kAlignCenter, GlobalConstants::WhiteColor);
    return;
  }

  if (_npcRef->Data.ProvidesService != ServiceType::NONE)
  {
    Printer::Instance().PrintFB(1,               th - 1, "'n' - name",     Printer::kAlignLeft,   GlobalConstants::WhiteColor);
    Printer::Instance().PrintFB(tw / 2 - tw / 4, th - 1, "'j' - job",      Printer::kAlignCenter, GlobalConstants::WhiteColor);
    Printer::Instance().PrintFB(tw / 2,          th - 1, "'s' - services", Printer::kAlignCenter, GlobalConstants::WhiteColor);
    Printer::Instance().PrintFB(tw / 2 + tw / 4, th - 1, "'g' - gossip",   Printer::kAlignCenter, GlobalConstants::WhiteColor);
    Printer::Instance().PrintFB(tw - 1,          th - 1, "'q' - bye",      Printer::kAlignRight,  GlobalConstants::WhiteColor);
  }
  else
  {
    Printer::Instance().PrintFB(1,               th - 1, "'n' - name",   Printer::kAlignLeft,   GlobalConstants::WhiteColor);
    Printer::Instance().PrintFB(tw / 2 - tw / 8, th - 1, "'j' - job",    Printer::kAlignCenter, GlobalConstants::WhiteColor);
    Printer::Instance().PrintFB(tw / 2 + tw / 8, th - 1, "'g' - gossip", Printer::kAlignCenter, GlobalConstants::WhiteColor);
    Printer::Instance().PrintFB(tw - 1,          th - 1, "'q' - bye",    Printer::kAlignRight,  GlobalConstants::WhiteColor);
  }
}
