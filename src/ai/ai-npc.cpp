#include "ai-npc.h"
#include "ai-component.h"
#include "game-object.h"
#include "rng.h"
#include "application.h"

AINPC::AINPC()
{
  _hash = typeid(*this).hash_code();

  IsAgressive = false;
}

void AINPC::Update()
{
  if (_data.IsStanding)
  {
    return;
  }

  if (AIComponentRef->OwnerGameObject->Attrs.ActionMeter < 100)
  {
    AIComponentRef->OwnerGameObject->WaitForTurn();
  }
  else
  {
    RandomMovement();

    AIComponentRef->OwnerGameObject->FinishTurn();
  }
}

void AINPC::Init(NPCType type, bool standing)
{
  _npcType = type;
  _data.IsStanding = standing;

  auto& playerRef = Application::Instance().PlayerInstance;

  switch (_npcType)
  {
    case NPCType::CLAIRE:
    {
      _data.IsMale = false;
      _data.UnacquaintedDescription = "You see a pink-haired woman";
      _data.Name = "Claire";
      _data.Job = "Soldier";

      _data.NameResponse = "My name is Claire";
      _data.JobResponse = "I'm a soldier";

      if (playerRef.GetClass() == PlayerClass::SOLDIER)
      {
        TextLines bonusStr =
        {
          "Greetings, fellow soldier! How are you today? ;-)",
          "What? You're here on vacation too?",
          "What a coincedence..."
        };

        _data.GossipResponsesByMap[MapType::TOWN].push_back(bonusStr);
      }

      _data.GossipResponsesByMap[MapType::TOWN] =
      {
        {
          "I took a vacation only to arrive in a place with some evil lurking underground.",
          "Worst vacation ever."
        },
        {
          "They say the mines are enchanted or something - they're different every time you arrive here.",
          "Can you believe this?"
        },
        {
          "Lightning?",
          "What about it?"
        }
      };
    }
    break;

    case NPCType::CLOUD:
    {
      _data.UnacquaintedDescription = "You see a man with ridiculously big sword";
      _data.Name = "Cloud";
      _data.Job = "Ex-Soldier";

      _data.NameResponse = "Cloud Strife";
      _data.JobResponse = "I'm an Ex-Soldier";

      _data.GossipResponsesByMap[MapType::TOWN] =
      {
        {
          "No, I'm not a \"soldier\" like those in your City."
          "I'm from the East actually. We're... well, a little bit different up there."
        },
        {
          "No one knows how deep the mines are. Nobody ever returned to tell the tale."
        },
        {
          "I try no to use my sword.",
          "Use brings about wear, tear, and rust — and THAT is a real waste."
        },
        {
          "Why I never went to the mines myself?",
          "Because I'm retired, that's why!",
          "I've had enough nasty stuff happened to me for the past years.",
          "I don't want any more."
        }
      };
    }
    break;

    case NPCType::IARSPIDER:
    {
      _data.UnacquaintedDescription = "You see a man wearing a mask";
      _data.Name = "Ivan";
      _data.Job = "Anonymous";

      _data.NameResponse = "We are Legion";
      _data.JobResponse = "We fight the System";

      _data.GossipResponsesByMap[MapType::TOWN] =
      {
        {
          "The Great Eye is watching you."
        },
        {
          "We are Anonymous. We never forget and never forgive."
        },
        {
          "Knowledge is power"
        }
      };
    }
    break;

    case NPCType::MILES:
    {
      _data.UnacquaintedDescription = "You see a collected nobleman in fancy clothes";
      _data.Name = "Miles";
      _data.Job = "Chief Prosecutor";

      _data.NameResponse = "Miles Edgeworth, at your service.";
      _data.JobResponse = "I'm Chief Prosecutor of this province.";

      _data.GossipResponsesByMap[MapType::TOWN] =
      {
        {
          "They sent me here to maintain order and,",
          "as ill luck would have it, this excuse for a lawyer,",
          "Phoenix Wright, arrives as well!",
          "As if we don't have enough problems already!"
        },
        {
          "If you want something done - hire a bounty hunter.",
          "...",
          "Wait, that's not my line...",
          "And not from here either!"
        },
        {
          "To be or not to be?",
          "What kind of a stupid question is that?"
        }
      };
    }
    break;

    case NPCType::PHOENIX:
    {
      _data.UnacquaintedDescription = "You see a carefree guy with spiky hair";
      _data.Name = "Phoenix";
      _data.Job = "Attorney";

      _data.NameResponse = "I'm Phoenix Wright.";
      _data.JobResponse = "I'm a defence attorney.";

      _data.GossipResponsesByMap[MapType::TOWN] =
      {
        {
          "Miles may act as if he doesn't know me,",
          "but we actually studied law together at the Academy.",
          "Believe it or not, but he wanted to become a defense attorney too!",
          "I wonder what changed him..."
        },
        {
          "I've been to the villages before. But never to the \"classic\" ones."
        },
        {
          "Look, it's my attorney's badge!",
          "(shows you small round gold plated pin with Scales of Justice engraved on it)"
        }
      };
    }
    break;

    case NPCType::TIGRA:
    {
      _data.IsMale = false;
      _data.UnacquaintedDescription = "You see a little girl wearing headband with cat ears";
      _data.Name = "Tigra";
      _data.Job = "local";

      _data.NameResponse = "I'm Tigra! Nice to meet you!";
      _data.JobResponse = "Oh, I'm just a local.";

      _data.GossipResponsesByMap[MapType::TOWN] =
      {
        {
          "Welcome to our small neighbourhood!",
          "It wasn't always this gloomy, just so you know..."
        },
        {
          "The dungeon? Oh, you mean the mine?",
          "Yeah, I remember people always running around, carrying stuff there and back, talking and being all busy.",
          "Lots of commotion!",
          "But then everything kinda stopped for some reason...",
          "I don't know why, but I overheard grownups saying that",
          "there was some kind of... in-ee-qui-ty ... happened down there.",
          "One day a miner was killed during a quarrel, and now he is seeking revenge on us for that!",
          "Scary!"
        },
        {
          "My parents are... not around anymore.",
          "Our priest was kind enough to adopt me, so I'm staying at the chapel now."
        }
      };
    }
    break;

    case NPCType::STEVE:
    {
      _data.UnacquaintedDescription = "You see a blocky looking man carrying a pickaxe";
      _data.Name = "Steve";
      _data.Job = "Miner";

      _data.NameResponse = "My name's Steve";
      _data.JobResponse = "I'm a miner.";

      _data.GossipResponsesByMap[MapType::TOWN] =
      {
        {
          "Emeralds are green, diamonds are blue, Redstone is red and this is all true!"
        },
        {
          "What do you mean I'm blocky? Look who's talking!"
        },
        {
          "I've been working in these mines for almost eight years now.",
          "I never heard or saw anything that would be even remotely connected to those stories you may hear.",
          "Just think for yourself: how can something like",
          "that happened in a such small and isolated place like this,",
          "where people know each other from childhood, yet nobody says he knows or heard anything?"
        },
        {
          "Business has been pretty much frozen since those events started.",
          "I'm thinking on going to the City for earnings.",
          "I wonder, why they never bothered to send some guards,",
          "or soldiers or whatever here to look into our situation?"
        }
      };
    }
    break;
  }
}

void AINPC::RandomMovement()
{
  int dx = RNG::Instance().Random() % 2;
  int dy = RNG::Instance().Random() % 2;

  int signX = (RNG::Instance().Random() % 2) == 0 ? -1 : 1;
  int signY = (RNG::Instance().Random() % 2) == 0 ? -1 : 1;

  dx *= signX;
  dy *= signY;

  AIComponentRef->OwnerGameObject->Move(dx, dy);
}
