#include "ai-npc.h"
#include "ai-component.h"
#include "game-object.h"
#include "rng.h"
#include "application.h"
#include "trader-component.h"

AINPC::AINPC()
{
  _hash = typeid(*this).hash_code();

  IsAgressive = false;
}

void AINPC::Init(NPCType type, bool immovable, ServiceType serviceType)
{
  _npcType = type;

  Data.IsImmovable = immovable;
  Data.ProvidesService = serviceType;

  // TODO: moar?

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

    case NPCType::CASEY:
      SetDataCasey();
      break;

    case NPCType::MAYA:
      SetDataMaya();
      break;

    case NPCType::GRISWOLD:
      SetDataGriswold();
      break;

    default:
      SetDataDefault();
      break;
  }

  for (int i = 0; i < 19; i++)
  {
    AIComponentRef->OwnerGameObject->LevelUp();
  }

  AIComponentRef->OwnerGameObject->Attrs.HP.Restore();
  AIComponentRef->OwnerGameObject->Attrs.MP.Restore();

  ConstructAI();
}

void AINPC::PrepareScript()
{
  /*
  const std::string scriptImmovable =
R"(
[TREE]
  [SEL]
    [TASK p1="idle"]
)";
  */

  /*
  const std::string scriptMovable =
R"(
[TREE]
  [SEL]
    [COND p1="d100" p2="40"]
      [TASK p1="move_rnd"]
    [TASK p1="idle"]
)";
  */

  //_scriptAsText = Data.IsImmovable ? Util::DecodeString(scriptImmovable) : Util::DecodeString(scriptMovable);

  const std::vector<uint8_t> scriptImmovable =
  {
    0x00, 0x01, 0x02, 0x02, 0x04, 0x06, 0x65, 0xFF
  };

  std::vector<uint8_t> scriptMovable =
  {
    0x00, 0x01, 0x02, 0x02, 0x04, 0x07, 0x77, 0x28,
    0xFF, 0x06, 0x06, 0x66, 0xFF, 0x04, 0x06, 0x65,
    0xFF
  };

  _scriptCompiled = Data.IsImmovable ? scriptImmovable : scriptMovable;
}

void AINPC::SetDataClaire()
{
  AIComponentRef->OwnerGameObject->Attrs.Str.Talents = 3;
  AIComponentRef->OwnerGameObject->Attrs.Def.Talents = 2;
  AIComponentRef->OwnerGameObject->Attrs.Skl.Talents = 1;
  AIComponentRef->OwnerGameObject->Attrs.HP.Talents = 3;

  AIComponentRef->OwnerGameObject->ObjectName = "woman";

  Data.IsMale = false;
  Data.UnacquaintedDescription = "You see a pink-haired woman";
  Data.Name = "Claire";
  Data.Job = "Soldier";

  Data.NameResponse = "My name is Claire.";
  Data.JobResponse = "I'm a Soldier.";

  Data.GossipResponsesByMap[MapType::TOWN] =
  {
    {
      // ================================================================================ 80
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

  auto& playerRef = Application::Instance().PlayerInstance;

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
  AIComponentRef->OwnerGameObject->Attrs.Str.Talents = 3;
  AIComponentRef->OwnerGameObject->Attrs.Def.Talents = 2;
  AIComponentRef->OwnerGameObject->Attrs.Skl.Talents = 1;
  AIComponentRef->OwnerGameObject->Attrs.HP.Talents = 3;

  AIComponentRef->OwnerGameObject->ObjectName = "man";

  Data.UnacquaintedDescription = "You see a man with a ridiculously big sword";
  Data.Name = "Cloud";
  Data.Job = "Ex-Soldier";

  Data.NameResponse = "Cloud Strife.";
  Data.JobResponse = "I'm an Ex-Soldier.";

  Data.GossipResponsesByMap[MapType::TOWN] =
  {
    // ================================================================================ 80
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
      "I've had enough nasty stuff happened to me over the past years.",
      "I don't want any more."
    }
  };

  auto& playerRef = Application::Instance().PlayerInstance;

  if (playerRef.GetClass() == PlayerClass::SOLDIER)
  {
    TextLines bonusStr =
    {
      "You're a Soldier, huh?",
      "Well, say hello to Zack for me when you see him.",
    };

    Data.GossipResponsesByMap[MapType::TOWN].push_back(bonusStr);
  }
}

void AINPC::SetDataIan()
{
  AIComponentRef->OwnerGameObject->Attrs.Spd.Talents = 3;
  AIComponentRef->OwnerGameObject->Attrs.Skl.Talents = 2;
  AIComponentRef->OwnerGameObject->Attrs.Def.Talents = 1;
  AIComponentRef->OwnerGameObject->Attrs.HP.Talents = 1;

  AIComponentRef->OwnerGameObject->ObjectName = "man";

  Data.UnacquaintedDescription = "You see a man wearing a mask";
  Data.Name = "Ian";
  Data.Job = "Anonymous";

  Data.NameResponse = "We are Legion.";
  Data.JobResponse = "We fight the System.";

  Data.GossipResponsesByMap[MapType::TOWN] =
  {
    // ================================================================================ 80
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
    // ================================================================================ 80
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
    // ================================================================================ 80
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
    // ================================================================================ 80
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
      "Our priest was kind enough to adopt me so I'm staying at the chapel now."
    }
  };
}

void AINPC::SetDataSteve()
{
  AIComponentRef->OwnerGameObject->Attrs.Spd.Talents = 1;
  AIComponentRef->OwnerGameObject->Attrs.Skl.Talents = 3;
  AIComponentRef->OwnerGameObject->Attrs.Def.Talents = 1;
  AIComponentRef->OwnerGameObject->Attrs.HP.Talents = 2;

  AIComponentRef->OwnerGameObject->ObjectName = "man";

  Data.UnacquaintedDescription = "You see a blocky looking man carrying a pickaxe";
  Data.Name = "Steve";
  Data.Job = "Miner";

  Data.NameResponse = "My name's Steve.";
  Data.JobResponse = "I'm a Miner.";

  Data.GossipResponsesByMap[MapType::TOWN] =
  {
    // ================================================================================ 80
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
      "Just think for yourself: how could've something bad like that",
      "happenned in such small and isolated place like this,",
      "where people know each other since childhood,",
      "yet nobody says he knows or heard anything?",
      "I don't know, man..."
    },
    {
      "Business has been pretty much frozen since those events started.",
      "I'm thinking on going to the City for earnings.",
      "I wonder why they never bothered to send somebody",
      "to look into our situation here if it's THAT bad?"
    },
    {
      "Would you believe me if I told you,",
      "that there are places where there is nothing but empty sky around you?",
      "You can see the clouds, the rainbows and even distant planets.",
      "Sometimes it even rains there!",
      "Or a place that literally looks pretty much like how you imagine Hell,",
      "with red color everywhere, fire, lava,",
      "and even soil itself sometimes is made of flesh?",
      "There was also a great desert with nothing but dust,",
      "stretching all the way to the horizon and beyond in all directions.",
      "If you dig here and there you might discover",
      "that under the sand lie ruins of some ancient city",
      "of some advanced civilization!",
      "I wonder what happened to them and whether all",
      "that desert is somehow connected to their fate.",
      "Battling with heat and questing for water was a big challenge back there...",
      "...",
      "Yeah, they all think I'm just crazy..."
    }
  };
}

void AINPC::SetDataGimley()
{
  AIComponentRef->OwnerGameObject->Attrs.Str.Talents = 3;
  AIComponentRef->OwnerGameObject->Attrs.Def.Talents = 3;
  AIComponentRef->OwnerGameObject->Attrs.HP.Talents = 3;

  AIComponentRef->OwnerGameObject->ObjectName = "man";

  Data.UnacquaintedDescription = "You see a stubby looking guy with an impressive beard";
  Data.Name = "Gimley";
  Data.Job = "Warrior";

  Data.NameResponse = "Gimley, son of Gloine.";
  Data.JobResponse = "I'm a Warrior.";

  Data.GossipResponsesByMap[MapType::TOWN] =
  {
    // ================================================================================ 80
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
  AIComponentRef->OwnerGameObject->Attrs.Mag.Talents = 3;
  AIComponentRef->OwnerGameObject->Attrs.Res.Talents = 2;
  AIComponentRef->OwnerGameObject->Attrs.Spd.Talents = 1;
  AIComponentRef->OwnerGameObject->Attrs.MP.Talents = 3;

  AIComponentRef->OwnerGameObject->ObjectName = "man";

  Data.UnacquaintedDescription = "You see a local cleric";
  Data.Name = "Martin";
  Data.Job = "Cleric";

  Data.NameResponse = "I'm brother Martin.";

  Data.GossipResponsesByMap[MapType::TOWN] =
  {
    // ================================================================================ 80
    {
      "All money you spend buying my wares are going straight to charity."
    },
    {
      "I guess you know about Lost City, don't you?",
      "They say it predates our City and is buried somewhere deep underground.",
      "It was the center of an ancient civilization of the times long past,",
      "but some catastrophe led to its demise along with its denizens.",
      "I'd say it was arrogance - the more advanced we get,",
      "the more reckless and irresponsible we become.",
      "First we try to defeat some long-lasted issues",
      "that plague human existence for centuries, like diseases.",
      "And it seems just and good.",
      "After that we decide that we need to improve our quality of life even further.",
      "We create artificial lights that allow us to see during the night",
      "so we could extend our daytime this way, we create artificial instruments,",
      "so that they will do the work for us.",
      "Then we decide that our life must not end at all",
      "and we seek means to prolong it.",
      "In the end we just start to think that we \"own\" everything",
      "and can do anything and nothing is impossible.",
      "We try to become gods.",
      "How come that originally good intentions lead to such results?",
      "Can it be avoided? And how?..."
    }
  };

  auto& playerRef = Application::Instance().PlayerInstance;
  if (playerRef.GetClass() == PlayerClass::ARCANIST)
  {
    TextLines bonusStr =
    {
      "You can pray at the altar of St. Nestor the Scribe here.",
      "Just proceed past the Royal Gates behind me."
    };

    Data.GossipResponsesByMap[MapType::TOWN].push_back(bonusStr);
  }
  else
  {
    TextLines bonusStr =
    {
      "It is forbidden for laity to enter the Royal Gates."
    };

    Data.GossipResponsesByMap[MapType::TOWN].push_back(bonusStr);
  }

  TraderComponent* tc = AIComponentRef->OwnerGameObject->AddComponent<TraderComponent>();
  tc->NpcRef = this;
  tc->Init(TraderRole::CLERIC, 1000, 10);
}

void AINPC::SetDataCasey()
{
  AIComponentRef->OwnerGameObject->Attrs.Spd.Talents = 3;
  AIComponentRef->OwnerGameObject->Attrs.Def.Talents = 3;
  AIComponentRef->OwnerGameObject->Attrs.Skl.Talents = 3;
  AIComponentRef->OwnerGameObject->Attrs.HP.Talents = 1;

  AIComponentRef->OwnerGameObject->ObjectName = "man";

  Data.UnacquaintedDescription = "You see a man wearing white cook's clothes";
  Data.Name = "Casey";
  Data.Job = "Cook";

  Data.NameResponse = "My name's Casey.";

  Data.GossipResponsesByMap[MapType::TOWN] =
  {
    // ================================================================================ 80
    {
      "I used to be a cook in the Navy.",
      "One day we were boarded by the enemy, on our very own ship!",
      "I mananged to hide myself in the refrigerator room.",
      "Almost froze to death, but then boarders finally decided",
      "to check upon ship's kitchen.",
      "Well, it didn't end 'well' for them in the end,",
      "but it did end well for us!",
      "All's well that ends well, eh?",
      "Hahahahaha!"
    }
  };

  TraderComponent* tc = AIComponentRef->OwnerGameObject->AddComponent<TraderComponent>();
  tc->NpcRef = this;
  tc->Init(TraderRole::COOK, 500, 15);
}

void AINPC::SetDataMaya()
{
  AIComponentRef->OwnerGameObject->Attrs.Spd.Talents = 3;
  AIComponentRef->OwnerGameObject->Attrs.Mag.Talents = 2;
  AIComponentRef->OwnerGameObject->Attrs.Res.Talents = 2;
  AIComponentRef->OwnerGameObject->Attrs.Skl.Talents = 1;

  AIComponentRef->OwnerGameObject->ObjectName = "woman";

  Data.IsMale = false;
  Data.UnacquaintedDescription = "You see a blue-haired woman wearing strange clothes";
  Data.Name = "Maya";
  Data.Job = "Junker";

  Data.NameResponse = "I'm Maya.";

  Data.GossipResponsesByMap[MapType::TOWN] =
  {
    // ================================================================================ 80
    {
      "We, the Junkers, collect discarded and abandoned items.",
      "Our culture revolves around it - we believe that every item is usable",
      "as long as it's not destroyed completely.",
      "Because of this \"junky\" business, people think that we're some kind of",
      "a poor and homeless group of outcasts, making their living by rummaging",
      "through piles of garbage, hoping to find food or something valuable.",
      "Well, it may look repulsive and suspicious, but the thing is",
      "you never know WHAT you'll find, so it might be some",
      "useless trinket, a cursed ring or a blessed artifact, who knows..."
    },
    {
      "Think of my services as a gambling - you can try to buy something",
      "if you're feeling lucky and have money to spare.",
      "Also you can bring to me any unidentified items",
      "and I'll buy them from you for a higher price than other vendors.",
      "So it's kind of a compensation for the risk, don't you think? ;-)",
    },
    {
      "I hail from a distant town called Oasis.",
      "It's a long-long journey overseas, then across the barren lands,",
      "mountains and canyons, further into the West...",
      "I have an uncle and a little brother there.",
      "He's quite a little rascal he is! XD",
      "But it is to be expected, I suppose - our parents died",
      "when we were still very young so I guess it's up to me now",
      "to look after the little one...",
      "I just want to see the world as much as possible,",
      "that's why I'm so far away from home.",
      "You cannot imagine how different all of this looks compared to my homeland!",
      "Everything is so green!"
    }
  };

  TraderComponent* tc = AIComponentRef->OwnerGameObject->AddComponent<TraderComponent>();
  tc->NpcRef = this;
  tc->Init(TraderRole::JUNKER, 1000, 8);
}

void AINPC::SetDataDefault()
{
  int gender = RNG::Instance().RandomRange(0, 2);

  Data.CanSpeak = false;
  Data.IsMale = (gender == 0);

  AIComponentRef->OwnerGameObject->ObjectName = (gender == 0) ? "man" : "woman";

  Data.UnacquaintedDescription = "You see a " + AIComponentRef->OwnerGameObject->ObjectName;
}

void AINPC::SetDataGriswold()
{
  AIComponentRef->OwnerGameObject->Attrs.Str.Talents = 3;
  AIComponentRef->OwnerGameObject->Attrs.Def.Talents = 2;
  AIComponentRef->OwnerGameObject->Attrs.Skl.Talents = 1;
  AIComponentRef->OwnerGameObject->Attrs.HP.Talents = 3;

  AIComponentRef->OwnerGameObject->ObjectName = "man";

  Data.IsMale = true;
  Data.UnacquaintedDescription = "You see a bald man wearing an apron";
  Data.Name = "Griswold";
  Data.Job = "Blacksmith";

  Data.NameResponse = "Name's Griswold.";

  Data.GossipResponsesByMap[MapType::TOWN] =
  {
    // ================================================================================ 80
    {
      "Well, what kin I do fer ya?"
    }
  };

  TraderComponent* tc = AIComponentRef->OwnerGameObject->AddComponent<TraderComponent>();
  tc->NpcRef = this;
  tc->Init(TraderRole::BLACKSMITH, 1000, 10);
}
