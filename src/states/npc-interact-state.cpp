#include "npc-interact-state.h"

#include "application.h"
#include "printer.h"
#include "util.h"
#include "ai-npc.h"
#include "ai-component.h"
#include "service-state.h"
#include "trader-component.h"
#include "shopping-state.h"

void NPCInteractState::Init()
{
}

// =============================================================================

void NPCInteractState::Prepare()
{
  _charPos            = _textStartPosX;
  _currentLine        = 0;
  _textBlockCharIndex = 0;

  Game::gPrnt.Clear();

  PrintHeader();
  PrintFooter();

  Game::gPrnt.Render();
}

// =============================================================================

void NPCInteractState::Cleanup()
{
  _npcRef             = nullptr;
  _gossipBlockIndex   = 0;
  _whatKey            = WhatKey::NONE;
  _textPrinting       = false;
  _charPos            = _textStartPosX;
  _currentLine        = 0;
  _textBlockCharIndex = 0;

  _blockToPrint.clear();

  Util::WaitForMs(0, true);
}

// =============================================================================

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
      GameObject* ogo = _npcRef->AIComponentRef->OwnerGameObject;
      TraderComponent* tc = ogo->GetComponent<TraderComponent>();
      if (tc != nullptr)
      {
        GameStates gs = GameStates::SHOPPING_STATE;
        auto state = Game::gApp.GetGameStateRefByName(gs);
        ShoppingState* ss = static_cast<ShoppingState*>(state);
        ss->PassShopOwner(tc);
        Game::gApp.ChangeState(GameStates::SHOPPING_STATE);
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

      int maxBlocks =
          _npcRef->Data.GossipResponsesByMap.at(MapType::TOWN).size();

      if (_gossipBlockIndex > maxBlocks - 1)
      {
        _gossipBlockIndex = 0;
      }

      MapType mt = MapType::TOWN;

      _whatKey = WhatKey::GOSSIP;

      _blockToPrint =
          _npcRef->Data.GossipResponsesByMap.at(mt)[_gossipBlockIndex];

      _textPrinting = true;
      Prepare();
    }
    break;

    case 's':
    {
      if (_npcRef->Data.ProvidesService != ServiceType::NONE)
      {
        GameObject* ogo = _npcRef->AIComponentRef->OwnerGameObject;
        TraderComponent* tc = ogo->GetComponent<TraderComponent>();

        GameStates gs = GameStates::SERVICE_STATE;
        auto s = Game::gApp.GetGameStateRefByName(gs);

        ServiceState* ss = static_cast<ServiceState*>(s);
        ss->Setup(tc);

        Game::gApp.ChangeState(GameStates::SERVICE_STATE);
      }
    }
    break;

    case VK_CANCEL:
    {
      std::string msg;
      if (_npcRef->Data.IsAquainted)
      {
        msg = Util::StringFormat("You finished speaking with %s the %s",
                                 _npcRef->Data.Name.data(),
                                 _npcRef->Data.Job.data());
      }
      else
      {
        const std::string& name =
            _npcRef->AIComponentRef->OwnerGameObject->ObjectName;

        msg = Util::StringFormat("You finished speaking with %s", name.data());
      }

      Game::gPrnt.AddMessage(msg);
      Game::gApp.ChangeState(GameStates::MAIN_STATE);
    }
    break;

    default:
      break;
  }
}

// =============================================================================

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

// =============================================================================

void NPCInteractState::AnimateText()
{
  //
  // To print 'Listening...' during animation.
  //
  PrintFooter();

  auto line = _blockToPrint[_currentLine];

  Game::gPrnt.PrintFB(_charPos + 1,
                      _currentLine + 2,
                      ' ',
                      Colors::BlackColor,
                      Colors::WhiteColor);

  Game::gPrnt.PrintFB(_charPos,
                      _currentLine + 2,
                      line[_textBlockCharIndex],
                      Colors::WhiteColor,
                      Colors::BlackColor);

  Game::gPrnt.Render();

  _charPos++;
  _textBlockCharIndex++;

  if (_textBlockCharIndex >= line.length())
  {
    Game::gPrnt.PrintFB(_charPos,
                        _currentLine + 2,
                        ' ',
                        Colors::BlackColor,
                        Colors::BlackColor);
    _charPos = _textStartPosX;
    _textBlockCharIndex = 0;
    _currentLine++;
  }

  if (_currentLine >= _blockToPrint.size())
  {
    _textPrinting = false;
    DisplayStillText();
  }
}

// =============================================================================

void NPCInteractState::DisplayStillText()
{
  Game::gPrnt.Clear();

  PrintHeader();

  int yPos = 2;
  for (auto& l : _blockToPrint)
  {
    Game::gPrnt.PrintFB(_textStartPosX,
                        yPos,
                        l,
                        Printer::kAlignLeft,
                        Colors::WhiteColor,
                        Colors::BlackColor);
    yPos++;
  }

  PrintFooter();

  Game::gPrnt.Render();
}

// =============================================================================

void NPCInteractState::SetNPCRef(AINPC* npcRef)
{
  _npcRef = npcRef;
}

// =============================================================================

void NPCInteractState::PrintHeader()
{
  std::string desc;

  if (_npcRef->Data.IsAquainted)
  {
    auto str = Util::StringFormat(" %s the %s ",
                                  _npcRef->Data.Name.data(),
                                  _npcRef->Data.Job.data());
    desc = str;
  }
  else
  {
    desc = " ??? ";
  }

  DrawHeader(desc);
}

// =============================================================================

void NPCInteractState::PrintFooter()
{
  size_t tw = Printer::TerminalWidth;
  size_t th = Printer::TerminalHeight;

  if (_textPrinting)
  {
    Game::gPrnt.PrintFB(tw / 2,
                        th - 1,
                        "Listening...",
                        Printer::kAlignCenter,
                        Colors::WhiteColor,
                        Colors::BlackColor);
    return;
  }

  if (_npcRef->Data.ProvidesService != ServiceType::NONE)
  {
    Game::gPrnt.PrintFB(1,
                        th - 1, StrName,
                        Printer::kAlignLeft,
                        Colors::WhiteColor,
                        Colors::BlackColor);

    Game::gPrnt.PrintFB(tw / 2 - tw / 4,
                        th - 1, StrJob,
                        Printer::kAlignCenter,
                        Colors::WhiteColor,
                        Colors::BlackColor);

    Game::gPrnt.PrintFB(tw / 2,
                        th - 1,
                        StrServices,
                        Printer::kAlignCenter,
                        Colors::WhiteColor,
                        Colors::BlackColor);

    Game::gPrnt.PrintFB(tw / 2 + tw / 4,
                        th - 1,
                        StrGossip,
                        Printer::kAlignCenter,
                        Colors::WhiteColor,
                        Colors::BlackColor);

    Game::gPrnt.PrintFB(tw - 1,
                        th - 1,
                        StrBye,
                        Printer::kAlignRight,
                        Colors::WhiteColor,
                        Colors::BlackColor);
  }
  else
  {
    Game::gPrnt.PrintFB(1,
                        th - 1, StrName,
                        Printer::kAlignLeft,
                        Colors::WhiteColor,
                        Colors::BlackColor);

    Game::gPrnt.PrintFB(tw / 2 - tw / 8,
                        th - 1,
                        StrJob,
                        Printer::kAlignCenter,
                        Colors::WhiteColor,
                        Colors::BlackColor);

    Game::gPrnt.PrintFB(tw / 2 + tw / 8,
                        th - 1,
                        StrGossip,
                        Printer::kAlignCenter,
                        Colors::WhiteColor,
                        Colors::BlackColor);

    Game::gPrnt.PrintFB(tw - 1,
                        th - 1,
                        StrBye,
                        Printer::kAlignRight,
                        Colors::WhiteColor,
                        Colors::BlackColor);
  }
}
