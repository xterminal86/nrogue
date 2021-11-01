#ifndef NPCINTERACTSTATE_H
#define NPCINTERACTSTATE_H

#include <vector>
#include <string>

#include "gamestate.h"

class AINPC;

class NPCInteractState : public GameState
{
  public:
    void Prepare() override;
    void Cleanup() override;
    void HandleInput() override;
    void Update(bool forceUpdate = false) override;
    void SetNPCRef(AINPC* npc);

  private:
    bool _textPrinting = false;
    int _gossipBlockIndex = 0;

    AINPC* _npcRef;

    void DisplayStillText();
    void AnimateText();
    void PrintHeader();
    void PrintFooter();

    enum class WhatKey
    {
      NAME = 0,
      JOB,
      GOSSIP,
      SERVICES,
      NONE
    };

    WhatKey _whatKey = WhatKey::NONE;

    std::vector<std::string> _blockToPrint;

    const size_t _textStartPosX = 1;

    size_t _charPos = _textStartPosX;
    size_t _currentLine = 0;
    size_t _textBlockCharIndex = 0;

    const std::string StrName     = "'n' - name";
    const std::string StrJob      = "'j' - job";
    const std::string StrServices = "'s' - services";
    const std::string StrGossip   = "'g' - gossip";
    const std::string StrBye      = "'q' - bye";
};

#endif // NPCINTERACTSTATE_H
