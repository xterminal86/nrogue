#ifndef INTROSTATE_H
#define INTROSTATE_H

#include <string>
#include <vector>
#include <chrono>

#include "application.h"
#include "gamestate.h"
#include "map.h"

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
        "Custom class desctiption goes here"
      }
    };

    int _stringIndex = 0;
    int _textPositionCursor = 0;

    int _textPositionX = 0;
    int _textPositionY = 0;

    std::chrono::high_resolution_clock::time_point _lastTime;

    const std::chrono::milliseconds kTextPrintDelay = std::chrono::milliseconds(10);

    void PrepareTown();

    #ifdef DEBUG_BUILD
    template <typename T>
    void OverrideStartingLevel(MapType level, const Position& size)
    {
      Map::Instance()._levels[level] = std::unique_ptr<T>(new T(size.X, size.Y, level, (int)level));
      Map::Instance().CurrentLevel = Map::Instance()._levels[level].get();
      Map::Instance()._levels[level]->PrepareMap(Map::Instance()._levels[level].get());
      Application::Instance().PlayerInstance.SetLevelOwner(Map::Instance().CurrentLevel);
      Application::Instance().PlayerInstance.MoveTo(Map::Instance().CurrentLevel->LevelStart);
      Map::Instance().CurrentLevel->AdjustCamera();
    }
    #endif
};

#endif // INTROSTATE_H
