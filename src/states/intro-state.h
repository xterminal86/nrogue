#ifndef INTROSTATE_H
#define INTROSTATE_H

#include <string>
#include <vector>

#include "gamestate.h"
#include "string-obfuscator.h"

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
              //---------0---------0---------0---------0---------0---------0---------0---------0 <- 80
        { HIDE("Another day - another septim. Or so they say.")                           },
        { HIDE("Finally your vacation time has arrived and you")                          },
        { HIDE("decide to spend it away from the City you sworn to protect.")             },
        { HIDE("Somewhere in the countryside would be fine, so")                          },
        { HIDE("you pack your things and move out.")                                      },
        { HIDE("You came across a remote village, which seems like a nice place though.") },
        { HIDE("Maybe this vacation will turn out nice as well...")                       }
      },
      {
              //---------0---------0---------0---------0---------0---------0---------0---------0 <- 80
        { HIDE("You lived all your life as an orphan.")                            },
        { HIDE("You've been everything: a messenger, a peddler, a pickpocket...")  },
        { HIDE("And occasionally a killer.")                                       },
        { HIDE("You don't remember how it started, but you're a criminal now.")    },
        { HIDE("And the City Watch is after you.")                                 },
        { HIDE("You decide to lay low for a while and no better place than a")     },
        { HIDE("remote countryside would do better.")                              },
        { HIDE("And maybe you could help yourself as well, while you're at it...") }
      },
      {
              //---------0---------0---------0---------0---------0---------0---------0---------0 <- 80
        { HIDE("You left great deserts and pagodas of the East far behind.")                   },
        { HIDE("A long journey has finally come to an end - you have arrived.")                },
        { HIDE("It was strange at best, being sent to some remote village")                    },
        { HIDE("as a final part of your training.")                                            },
        { HIDE("And with some reconnaissance mission at that.")                                },
        { HIDE("Not something you expected to say the least.")                                 },
        { HIDE("All you have to do is investigate this village and report back to the Order.") },
        { HIDE("Do this and you'll be finally ordained and may begin")                         },
        { HIDE("your service as a guardian of the Arcane.")                                    },
        { HIDE("With parting words of your master still echoing")                             },
        { HIDE("inside your head, you continue onward...")                                     }
      },
      {
              //---------0---------0---------0---------0---------0---------0---------0---------0 <- 80
        { HIDE("Custom class description goes here") }
      }
    };

    size_t _stringIndex        = 0;
    size_t _textPositionCursor = 0;

    int _textPositionX = 0;
    int _textPositionY = 0;

    const std::unordered_map<PlayerClass, std::string> _scenarioNameByClass =
    {
      { PlayerClass::SOLDIER,  { HIDE("WARRIOR'S REST")      } },
      { PlayerClass::THIEF,    { HIDE("THIEF'S HIDEOUT")     } },
      { PlayerClass::ARCANIST, { HIDE("HOLY QUEST")          } },
      { PlayerClass::CUSTOM,   { HIDE("MYSTERIOUS STRANGER") } }
    };
};

#endif // INTROSTATE_H
