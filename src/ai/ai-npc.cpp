#include "ai-npc.h"
#include "ai-component.h"
#include "game-object.h"
#include "rng.h"
#include "application.h"
#include "map.h"
#include "trader-component.h"

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

  // FIXME: moar?

  switch (_npcType)
  {
    case NPCType::CLAIRE:
      SetDataClaire();
      break;

    case NPCType::CLOUD:
      SetDataCloud();
      break;

    case NPCType::IARSPIDER:
      SetDataIan();
      break;

    case NPCType::MILES:
      SetDataMiles();
      break;

    case NPCType::PHOENIX:
      SetDataPhoenix();
      break;

    case NPCType::TIGRA:
      SetDataTigra();
      break;

    case NPCType::STEVE:
      SetDataSteve();
      break;

    case NPCType::GIMLEY:
      SetDataGimley();
      break;

    case NPCType::MARTIN:
      SetDataMartin();
      break;

    default:
      SetDataDefault();
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

void AINPC::SetDataClaire()
{
  AIComponentRef->OwnerGameObject->ObjectName = "woman";

  auto& playerRef = Application::Instance().PlayerInstance;

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

void AINPC::SetDataCloud()
{
  AIComponentRef->OwnerGameObject->ObjectName = "man";

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

void AINPC::SetDataIan()
{
  AIComponentRef->OwnerGameObject->ObjectName = "man";

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

void AINPC::SetDataMiles()
{
  AIComponentRef->OwnerGameObject->ObjectName = "man";

  Data.UnacquaintedDescription = "You see a collected nobleman in fancy clothes";
  Data.Name = "Miles";
  Data.Job = "Prosecutor";

  Data.NameResponse = "Miles Edgeworth, at your service.";
  Data.JobResponse = "I work as a Prosecutor for the Department of Justice in the City.";

  Data.GossipResponsesByMap[MapType::TOWN] =
  {
    // =========================================================================== 80
    {
      "I'm here to investigate rumors concerning the murder of a local miner.",
      "But, as ill luck would have it, this excuse for a lawyer,",
      "Phoenix Wright, arrives as well!",
      "As if I don't have enough to worry about already!"
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

void AINPC::SetDataPhoenix()
{
  AIComponentRef->OwnerGameObject->ObjectName = "man";

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

void AINPC::SetDataTigra()
{
  AIComponentRef->OwnerGameObject->ObjectName = "girl";

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
      "a miner was killed down there during a quarrel,",
      "and now he has risen from the dead and seeking revenge on us for that!",
      "Scary!"
    },
    {
      "My parents are... not around anymore.",
      "Our priest was kind enough to adopt me, so I'm staying at the chapel now."
    }
  };
}

void AINPC::SetDataSteve()
{
  AIComponentRef->OwnerGameObject->ObjectName = "man";

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
      "Redstone is red and this is all true!",
      "What's a redstone? Well, it's the thing",
      "we use to make all our machinery work.",
      "Looks like red powder, it emits a faint red light too,",
      "so you can sort of dip a stick into it and use it as a makeshift torch.",
      "You have to go very deep below the ground to mine it, though...",
      "But never dig straight down!"
    },
    {
      "Baked bread or baked potatoes?",
      "Cooked porkchop or steak?",
      "...",
      "Cooked silkworm? Hm... Don't want to remember it..."
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
    },
    {
      "Would you believe me if I told you,",
      "that there are places where there is nothing but empty sky around you.",
      "You can see the clouds, the rainbows and even distant planets?",
      "Sometimes it even rains there!",
      "Or a place that literally looks pretty much like how you imagine Hell,",
      "with red color everywhere, fire, lava,",
      "and even soil itself sometimes is made of flesh?",
      "There was also a great desert, with nothing but dust,",
      "stretching all the way to the horizon and beyond in all directions.",
      "If you dig here and there you might discover that under it lie ruins of",
      "some ancient city of some advanced civilization!",
      "I wonder what happened to them and whether all",
      "this desert is somehow connected to their fate.",
      "Battling with heat was a big challenge back there...",
      "...",
      "Yeah, they all think I'm just crazy..."
    }
  };
}

void AINPC::SetDataGimley()
{
  AIComponentRef->OwnerGameObject->ObjectName = "man";

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

void AINPC::SetDataMartin()
{
  AIComponentRef->OwnerGameObject->ObjectName = "man";

  Data.UnacquaintedDescription = "You see a local cleric";
  Data.Name = "Martin";
  Data.Job = "Cleric";

  Data.NameResponse = "I'm brother Martin.";
  Data.JobResponse = "I'm a Cleric.";

  Data.GossipResponsesByMap[MapType::TOWN] =
  {
    // =========================================================================== 80
    {
      "All money you spend buying my wares are going straight to charity."
    }
  };

  TraderComponent* tc = AIComponentRef->OwnerGameObject->AddComponent<TraderComponent>();
  tc->NpcRef = this;
  tc->Init(TraderRole::CLERIC, 1000);
}

void AINPC::SetDataDefault()
{
  int gender = RNG::Instance().RandomRange(0, 2);

  Data.CanSpeak = false;
  Data.IsMale = (gender == 0);

  AIComponentRef->OwnerGameObject->ObjectName = (gender == 0) ? "man" : "woman";

  Data.UnacquaintedDescription = "You see a " + AIComponentRef->OwnerGameObject->ObjectName;
}