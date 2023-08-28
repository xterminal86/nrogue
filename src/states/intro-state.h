#ifndef INTROSTATE_H
#define INTROSTATE_H

#include <string>
#include <vector>

#include "gamestate.h"

#ifdef DEBUG_BUILD
#include "application.h"
#include "map.h"
#endif

class IntroState : public GameState
{
  public:
    void Prepare() override;
    void Update(bool forceUpdate = false) override;
    void HandleInput() override;

  private:
    std::vector<std::vector<std::string>> _introStrings =
    {
      {
       //********************************************************************************80//
        "Another day - another septim. Or so they say.",
        "Finally your vacation time has arrived and you",
        "decide to spend it away from the City you sworn to protect.",
        "Somewhere in the countryside would be fine, so",
        "you pack your things and move out.",
        "You came across a remote village, which seems like a nice place though.",
        "Maybe this vacation will turn out nice as well..."
      },
      {
       //********************************************************************************80//
        "You lived all your life as an orphan.",
        "You've been everything: a messenger, a peddler, a pickpocket...",
        "And occasionally a killer.",
        "You don't remember how it started, but you're a criminal now.",
        "And the City Watch is after you.",
        "You decide to lay low for a while and no better place than a",
        "remote countryside would do better.",
        "And maybe you could help yourself as well, while you're at it...",
      },
      {
       //********************************************************************************80//
        "You left great deserts and pagodas of the East far behind.",
        "A long journey has finally come to an end - you have arrived.",
        "It was strange at best, being sent to some remote village",
        "as a final part of your training.",
        "And with some reconnaissance mission at that.",
        "Not something you expected to say the least.",
        "All you have to do is investigate this village and report back to the Order.",
        "Do this and you'll be finally ordained and may begin",
        "your service as a guardian of the Arcane.",
        "With farewell words of your master still echoing",
        "inside your head, you continue onward..."
      },
      {
       //********************************************************************************80//
        "Custom class description goes here"
      }
    };

    size_t _stringIndex = 0;
    size_t _textPositionCursor = 0;

    int _textPositionX = 0;
    int _textPositionY = 0;

    const std::unordered_map<PlayerClass, std::string> _scenarioNameByClass =
    {
      { PlayerClass::SOLDIER,  "WARRIOR'S REST"      },
      { PlayerClass::THIEF,    "THIEF'S HIDEOUT"     },
      { PlayerClass::ARCANIST, "HOLY QUEST"          },
      { PlayerClass::CUSTOM,   "MYSTERIOUS STRANGER" }
    };
};

#endif // INTROSTATE_H
