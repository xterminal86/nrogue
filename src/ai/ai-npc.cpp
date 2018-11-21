#include "ai-npc.h"
#include "ai-component.h"
#include "game-object.h"
#include "rng.h"
#include "application.h"
#include "map.h"

AINPC::AINPC()
{
  _hash = typeid(*this).hash_code();

  IsAgressive = false;
}

void AINPC::Update()
{
  if (Data.IsStanding)
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
  Data.IsStanding = standing;

  auto& playerRef = Application::Instance().PlayerInstance;

  // FIXME: moar?

  switch (_npcType)
  {
    case NPCType::CLAIRE:
    {
      Data.IsMale = false;
      Data.UnacquaintedDescription = "You see a pink-haired woman";
      Data.Name = "Claire";
      Data.Job = "Soldier";

      Data.NameResponse = "My name is Claire.";
      Data.JobResponse = "I'm a Soldier.";

      Data.GossipResponsesByMap[MapType::TOWN] =
      {
        {
          // =========================================================================== 80
          "I took a vacation only to arrive in a place",
          "with some evil lurking underground.",
          "Worst vacation ever."
        },
        {
          "They say the mines are enchanted or something.",
          "They're different every time you arrive here.",
          "Can you believe this?"
        },
        {
          "Lightning?",
          "What about it?"
        }
      };

      if (playerRef.GetClass() == PlayerClass::SOLDIER)
      {
        TextLines bonusStr =
        {
          "Greetings, fellow soldier! How are you today? ;-)",
          "What? You're here on vacation too?",
          "What a coincedence..."
        };

        Data.GossipResponsesByMap[MapType::TOWN].push_back(bonusStr);
      }
    }
    break;

    case NPCType::CLOUD:
    {
      Data.UnacquaintedDescription = "You see a man with ridiculously big sword";
      Data.Name = "Cloud";
      Data.Job = "Ex-Soldier";

      Data.NameResponse = "Cloud Strife.";
      Data.JobResponse = "I'm an Ex-Soldier.";

      Data.GossipResponsesByMap[MapType::TOWN] =
      {
        // =========================================================================== 80
        {
          "I'm not a \"soldier\" like those in your City.",
          "I'm from the East actually.",
          "We're... well, a little bit different up there."
        },
        {
          "No one knows how deep the mines are.",
          "Nobody ever returned to tell the tale."
        },
        {
          "I try no to use my sword.",
          "Use brings about wear, tear, and rust - and THAT is a real waste."
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
      Data.UnacquaintedDescription = "You see a man wearing a mask";
      Data.Name = "Ian";
      Data.Job = "Anonymous";

      Data.NameResponse = "We are Legion.";
      Data.JobResponse = "We fight the System.";

      Data.GossipResponsesByMap[MapType::TOWN] =
      {
        // =========================================================================== 80
        {
          "The Great Eye is watching you."
        },
        {
          "We are Anonymous. We never forget and never forgive."
        },
        {
          "Knowledge is power."
        }
      };
    }
    break;

    case NPCType::MILES:
    {
      Data.UnacquaintedDescription = "You see a collected nobleman in fancy clothes";
      Data.Name = "Miles";
      Data.Job = "Chief Prosecutor";

      Data.NameResponse = "Miles Edgeworth, at your service.";
      Data.JobResponse = "I'm Chief Prosecutor of this province.";

      Data.GossipResponsesByMap[MapType::TOWN] =
      {
        // =========================================================================== 80
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
      Data.UnacquaintedDescription = "You see a carefree guy with spiky hair";
      Data.Name = "Phoenix";
      Data.Job = "Attorney";

      Data.NameResponse = "I'm Phoenix Wright.";
      Data.JobResponse = "I'm a Defence Attorney.";

      Data.GossipResponsesByMap[MapType::TOWN] =
      {
        // =========================================================================== 80
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
      Data.IsMale = false;
      Data.UnacquaintedDescription = "You see a little girl wearing headband with cat ears";
      Data.Name = "Tigra";
      Data.Job = "Local";

      Data.NameResponse = "I'm Tigra! Nice to meet you!";
      Data.JobResponse = "Me? Oh, I'm just a local.";

      Data.GossipResponsesByMap[MapType::TOWN] =
      {
        // =========================================================================== 80
        {
          "Welcome to our small neighbourhood!",
          "It wasn't always this gloomy, just so you know..."
        },
        {
          "The dungeon? Oh, you mean the mine?",
          "Yeah, I remember people always running around,",
          "carrying stuff there and back, talking and being all busy.",
          "Lots of commotion!",
          "But then everything kinda stopped for some reason...",
          "I don't know why, but I overheard grownups saying that",
          "there was some kind of... in-ee-qui-ty ... happened down there.",
          "One day a miner was killed during a quarrel,",
          "and now he is seeking revenge on us for that!",
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
      Data.UnacquaintedDescription = "You see a blocky looking man carrying a pickaxe";
      Data.Name = "Steve";
      Data.Job = "Miner";

      Data.NameResponse = "My name's Steve.";
      Data.JobResponse = "I'm a Miner.";

      Data.GossipResponsesByMap[MapType::TOWN] =
      {
        // =========================================================================== 80
        {
          "Emeralds are green, diamonds are blue,",
          "Redstone is red and this is all true!"
        },
        {
          "What do you mean I'm blocky? Look who's talking!"
        },
        {
          "I've been working in these mines for almost eight years now.",
          "I never heard or saw anything that would be even remotely",
          "connected to those stories you may hear.",
          "Just think for yourself: how can something like",
          "that happened in a such small and isolated place like this,",
          "where people know each other since childhood,",
          "yet nobody says he knows or heard anything?"
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

    case NPCType::GIMLEY:
    {
      Data.UnacquaintedDescription = "You see a short but stubby looking guy with a long beard";
      Data.Name = "Gimley";
      Data.Job = "Warrior";

      Data.NameResponse = "Gimley, son of Gloine.";
      Data.JobResponse = "I'm a Warrior.";

      Data.GossipResponsesByMap[MapType::TOWN] =
      {
        // =========================================================================== 80
        {
          "(humming)",
          "Far over the misty mountains cold,",
          "To dungeons deep, and caverns old.",
          "We must away, ere break of day,",
          "To seek the pale, enchanted gold..."
        },
        {
          "(humming)",
          "Under the Mountain dark and tall,",
          "The King has come unto his hall!",
          "His foe is dead, the Worm of Dread,",
          "And ever so his foes shall fall..."
        },
        {
          "(humming)",
          "From the red sky of the East,",
          "To the sunset in the West,",
          "We have cheated Death,",
          "And he has cheated us...",
        },
        {
          "(humming)",
          "The forest raised a Christmas tree,",
          "'Twas silent and serene.",
          "In winter and in summer it was,",
          "Slender and so green...",
        }
      };
    }
    break;
  }

  AIComponentRef->OwnerGameObject->ObjectName = (Data.IsMale) ? "man" : "woman";
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

  // FIXME: redesign and rethink (maybe)

  /*
  std::vector<std::pair<float, float>> probs;

  std::vector<std::pair<int, int>> dirsToCheck =
  {
    { -1, -1 },
    { -1, 0 },
    { -1, 1 },
    { 0, -1 },
    { 0, 0 },
    { 0, 1 },
    { 1, -1 },
    { 1, 0 },
    { 1, -1 }
  };

  auto level = Map::Instance().CurrentLevel;

  for (auto& dir : dirsToCheck)
  {
    int x = AIComponentRef->OwnerGameObject->PosX + dir.first;
    int y = AIComponentRef->OwnerGameObject->PosY + dir.second;

    if (!level->MapArray[x][y]->Blocking && !level->MapArray[x][y]->Occupied)
    {
      auto res = GetDirectionProbability(dir.first, dir.second);
      probs.push_back(res);
    }
  }

  float max = 0.0f;
  int indexFound = -1;

  int index = 0;
  for (auto& c : probs)
  {
    float maxProb = std::max(c.first, c.second);

    if (maxProb > max)
    {
      max = maxProb;
      indexFound = index;
    }

    index++;
  }

  if (indexFound != -1)
  {
    auto pair = probs[indexFound];

    int chanceX = (int)(pair.first * 100);
    int chanceY = (int)(pair.second * 100);

    auto str = Util::StringFormat("[%i ; %i] Trying to pass %i %i\n", AIComponentRef->OwnerGameObject->PosX, AIComponentRef->OwnerGameObject->PosY, chanceX, chanceY);
    Logger::Instance().Print(str);

    int middleX = level->MapSize.X / 2;
    int middleY = level->MapSize.Y / 2;

    int sideX = AIComponentRef->OwnerGameObject->PosX - middleX;
    int sideY = AIComponentRef->OwnerGameObject->PosY - middleY;

    str = Util::StringFormat("%i %i\n", sideX, sideY);
    Logger::Instance().Print(str);

    bool succX = Util::Rolld100(chanceX);
    bool succY = Util::Rolld100(chanceY);

    if (succX || succY)
    {
      int dx = std::signbit(sideX) ? 1 : -1;
      int dy = std::signbit(sideY) ? 1 : -1;

      str = Util::StringFormat("Trying to move - %i %i\n", dx, dy);
      Logger::Instance().Print(str);

      AIComponentRef->OwnerGameObject->Move(dx, dy);
    }
  }
  */
}

std::pair<float, float> AINPC::GetDirectionProbability(int dirX, int dirY)
{
  std::pair<float, float> prob;

  auto level = Map::Instance().CurrentLevel;

  int middleX = level->MapSize.X / 2;
  int middleY = level->MapSize.Y / 2;

  int posX = AIComponentRef->OwnerGameObject->PosX + dirX;
  int posY = AIComponentRef->OwnerGameObject->PosY + dirY;

  int dx = std::abs(middleX - posX);
  int dy = std::abs(middleY - posY);

  float pX = ((float)dx / 100.0f) + 0.4f;
  float pY = ((float)dy / 100.0f) + 0.4f;

  prob.first = pX;
  prob.second = pY;

  return prob;
}
