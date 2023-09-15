#include "ai-npc.h"
#include "ai-component.h"
#include "game-object.h"
#include "rng.h"
#include "application.h"
#include "trader-component.h"
#include "string-obfuscator.h"

AINPC::AINPC()
{
  _hash = typeid(*this).hash_code();

  IsAgressive = false;
}

// =============================================================================

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

// =============================================================================

void AINPC::PrepareScript()
{
  const std::string scriptImmovable =
R"(
[TREE]
  [SEL]
    [TASK p1="idle"]
)";

  const std::string scriptMovable =
R"(
[TREE]
  [SEL]
    [COND p1="d100" p2="40"]
      [TASK p1="move_rnd"]
    [TASK p1="idle"]
)";

  _scriptAsText = Data.IsImmovable ? scriptImmovable : scriptMovable;

  /*
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
  */
}

// =============================================================================

void AINPC::SetDataClaire()
{
  AIComponentRef->OwnerGameObject->Attrs.Str.Talents = 3;
  AIComponentRef->OwnerGameObject->Attrs.Def.Talents = 2;
  AIComponentRef->OwnerGameObject->Attrs.Skl.Talents = 2;
  AIComponentRef->OwnerGameObject->Attrs.HP.Talents = 3;

  AIComponentRef->OwnerGameObject->ObjectName = kWoman;

  Data.IsMale = false;
  Data.UnacquaintedDescription = HIDE("You see a pink-haired woman");
  Data.Name = HIDE("Claire");
  Data.Job  = HIDE("Soldier");

  Data.NameResponse = HIDE("My name is Claire.");
  Data.JobResponse  = HIDE("I'm a Soldier.");

  Data.GossipResponsesByMap[MapType::TOWN] =
  {
    {
      { HIDE("I took a vacation only to arrive in a place") },
      { HIDE("with some evil lurking underground.")         },
      { HIDE("Worst vacation ever.")                        }
    },
    {
      { HIDE("They say the mines are enchanted or something.") },
      { HIDE("They're different every time you arrive here.")  },
      { HIDE("Can you believe this?")                          }
    },
    {
      { HIDE("Lightning?")     },
      { HIDE("What about it?") }
    }
  };

  auto& playerRef = Application::Instance().PlayerInstance;

  if (playerRef.GetClass() == PlayerClass::SOLDIER)
  {
    TextLines bonusStr =
    {
      { HIDE("Greetings, fellow soldier! How are you today? ;-)") },
      { HIDE("What? You're here on vacation too?")                },
      { HIDE("What a coincedence...")                             }
    };

    Data.GossipResponsesByMap[MapType::TOWN].push_back(bonusStr);
  }
}

// =============================================================================

void AINPC::SetDataCloud()
{
  AIComponentRef->OwnerGameObject->Attrs.Str.Talents = 3;
  AIComponentRef->OwnerGameObject->Attrs.Def.Talents = 2;
  AIComponentRef->OwnerGameObject->Attrs.Skl.Talents = 1;
  AIComponentRef->OwnerGameObject->Attrs.HP.Talents = 3;

  AIComponentRef->OwnerGameObject->ObjectName = kMan;

  Data.UnacquaintedDescription = HIDE("You see a man with a ridiculously big sword");
  Data.Name = HIDE("Cloud");
  Data.Job  = HIDE("Ex-Soldier");

  Data.NameResponse = HIDE("Cloud Strife.");
  Data.JobResponse  = HIDE("I'm an Ex-Soldier.");

  Data.GossipResponsesByMap[MapType::TOWN] =
  {
    {
      { HIDE("I'm not a \"soldier\" like those in your City.")  },
      { HIDE("I'm from the East actually.")                     },
      { HIDE("We're... well, a little bit different up there.") }
    },
    {
      { HIDE("No one knows how deep the mines are.")   },
      { HIDE("Nobody ever returned to tell the tale.") }
    },
    {
      { HIDE("I try no to use my sword.")                                         },
      { HIDE("Use brings about wear, tear, and rust - and THAT is a real waste.") }
    },
    {
      { HIDE("Why I never went to the mines myself?!")                          },
      { HIDE("Because I'm retired, that's why!")                                },
      { HIDE("I've had enough nasty stuff happened to me over the past years.") },
      { HIDE("I don't want any more. Enough is enough.")                        }
    }
  };

  auto& playerRef = Application::Instance().PlayerInstance;

  if (playerRef.GetClass() == PlayerClass::SOLDIER)
  {
    TextLines bonusStr =
    {
      { HIDE("You're a Soldier, huh?")                           },
      { HIDE("Well, say hello to Zack for me when you see him.") },
    };

    Data.GossipResponsesByMap[MapType::TOWN].push_back(bonusStr);
  }
}

// =============================================================================

void AINPC::SetDataIan()
{
  AIComponentRef->OwnerGameObject->Attrs.Spd.Talents = 3;
  AIComponentRef->OwnerGameObject->Attrs.Skl.Talents = 2;
  AIComponentRef->OwnerGameObject->Attrs.Def.Talents = 1;
  AIComponentRef->OwnerGameObject->Attrs.HP.Talents = 1;

  AIComponentRef->OwnerGameObject->ObjectName = kMan;

  Data.UnacquaintedDescription = HIDE("You see a man wearing a mask");
  Data.Name = HIDE("Ian");
  Data.Job  = HIDE("Anonymous");

  Data.NameResponse = HIDE("We are Legion.");
  Data.JobResponse  = HIDE("We fight the System.");

  Data.GossipResponsesByMap[MapType::TOWN] =
  {
    {
      { HIDE("The Great Eye is watching you.") }
    },
    {
      { HIDE("We are Anonymous. We never forget and never forgive.") }
    },
    {
      { HIDE("Knowledge is power.") }
    }
  };
}

// =============================================================================

void AINPC::SetDataMiles()
{
  AIComponentRef->OwnerGameObject->ObjectName = kMan;

  Data.UnacquaintedDescription = HIDE("You see a collected nobleman in fancy clothes");
  Data.Name = HIDE("Miles");
  Data.Job  = HIDE("Prosecutor");

  Data.NameResponse = HIDE("Miles Edgeworth, at your service.");
  Data.JobResponse  = HIDE("I work as a Prosecutor for the Department of Justice in the City.");

  Data.GossipResponsesByMap[MapType::TOWN] =
  {
    {
      { HIDE("I'm here to investigate rumors concerning the murder of a local miner.") },
      { HIDE("But, as ill luck would have it, this excuse for a lawyer,")              },
      { HIDE("Phoenix Wright, arrives as well!")                                       },
      { HIDE("As if I don't have enough to worry about already!")                      }
    },
    {
      { HIDE("If you want something done - hire a bounty hunter.") },
      { HIDE("...")                                                },
      { HIDE("Wait, that's not my line...")                        },
      { HIDE("And not from here either!")                          }
    },
    {
      { HIDE("To be or not to be?")                     },
      { HIDE("What kind of a stupid question is that?") }
    }
  };
}

// =============================================================================

void AINPC::SetDataPhoenix()
{
  AIComponentRef->OwnerGameObject->ObjectName = kMan;

  Data.UnacquaintedDescription = HIDE("You see a carefree guy with spiky hair");
  Data.Name = HIDE("Phoenix");
  Data.Job  = HIDE("Attorney");

  Data.NameResponse = HIDE("I'm Phoenix Wright.");
  Data.JobResponse  = HIDE("I'm a Defence Attorney.");

  Data.GossipResponsesByMap[MapType::TOWN] =
  {
    {
      { HIDE("Miles may act as if he doesn't know me,")                            },
      { HIDE("but we actually studied law together at the Academy.")               },
      { HIDE("Believe it or not, but he wanted to become a defense attorney too!") },
      { HIDE("I wonder what changed him...")                                       }
    },
    {
      { HIDE("I've been to the villages before. But never to the \"classic\" ones. XD") }
    },
    {
      { HIDE("Look, it's my attorney's badge!")                                               },
      { HIDE("(shows you small round gold plated pin with Scales of Justice engraved on it)") }
    }
  };
}

// =============================================================================

void AINPC::SetDataTigra()
{
  AIComponentRef->OwnerGameObject->ObjectName = "girl";

  Data.IsMale = false;
  Data.UnacquaintedDescription = HIDE("You see a little girl wearing headband with cat ears");

  Data.Name = HIDE("Tigra");
  Data.Job  = HIDE("Local");

  Data.NameResponse = HIDE("I'm Tigra! Nice to meet you!");
  Data.JobResponse  = HIDE("Me? Oh, I'm just a local.");

  Data.GossipResponsesByMap[MapType::TOWN] =
  {
    {
      { HIDE("Welcome to our small neighbourhood!")               },
      { HIDE("It wasn't always this gloomy, just so you know...") }
    },
    {
      { HIDE("The dungeon? Oh, you mean the mine?")                                    },
      { HIDE("Yeah, I remember people always running around,")                         },
      { HIDE("carrying stuff there and back, talking and being all busy.")             },
      { HIDE("Lots of commotion!")                                                     },
      { HIDE("But then everything kinda stopped for some reason...")                   },
      { HIDE("I don't know why, but I overheard grownups saying that")                 },
      { HIDE("a miner was killed down there during a quarrel,")                        },
      { HIDE("and now he has risen from the dead and seeking revenge on us for that!") },
      { HIDE("Scary!")                                                                 }
    },
    {
      { HIDE("My parents are... not around anymore.")                                    },
      { HIDE("Our priest was kind enough to adopt me so I'm staying at the chapel now.") }
    }
  };
}

// =============================================================================

void AINPC::SetDataSteve()
{
  AIComponentRef->OwnerGameObject->Attrs.Spd.Talents = 1;
  AIComponentRef->OwnerGameObject->Attrs.Skl.Talents = 3;
  AIComponentRef->OwnerGameObject->Attrs.Def.Talents = 1;
  AIComponentRef->OwnerGameObject->Attrs.HP.Talents = 2;

  AIComponentRef->OwnerGameObject->ObjectName = kMan;

  Data.UnacquaintedDescription = HIDE("You see a blocky looking man carrying a pickaxe");

  Data.Name = HIDE("Steve");
  Data.Job  = HIDE("Miner");

  Data.NameResponse = HIDE("My name's Steve.");
  Data.JobResponse  = HIDE("I'm a Miner.");

  Data.GossipResponsesByMap[MapType::TOWN] =
  {
    {
      { HIDE("Emeralds are green, diamonds are blue,")                                  },
      { HIDE("Redstone is red and this is all true!")                                   },
      { HIDE("What's a redstone? Well, it's the thing")                                 },
      { HIDE("we use to make all our machinery work.")                                  },
      { HIDE("Looks like red powder, it emits a faint red light too,")                  },
      { HIDE("so you can sort of dip a stick into it and use it as a makeshift torch.") },
      { HIDE("You have to go very deep below the ground to mine it, though...")         },
      { HIDE("But never dig straight down!")                                            }
    },
    {
      { HIDE("Baked bread or baked potatoes?")                      },
      { HIDE("Cooked porkchop or steak?")                           },
      { HIDE("...")                                                 },
      { HIDE("Cooked silkworm? Hm... Don't want to remember it...") }
    },
    {
      { HIDE("What do you mean I'm blocky? Look who's talking!") }
    },
    {
      { HIDE("I've been working in these mines for almost eight years now.")  },
      { HIDE("I never heard or saw anything that would be even remotely")     },
      { HIDE("connected to those stories you may hear.")                      },
      { HIDE("Just think for yourself: how could've something bad like that") },
      { HIDE("happenned in such small and isolated place like this,")         },
      { HIDE("where people know each other since childhood,")                 },
      { HIDE("yet nobody says he knows or heard anything?")                   },
      { HIDE("I don't know, man...")                                          }
    },
    {
      { HIDE("Business has been pretty much frozen since those events started.") },
      { HIDE("I'm thinking on going to the City for earnings.")                  },
      { HIDE("I wonder why they never bothered to send somebody")                },
      { HIDE("to look into our situation here if it's THAT bad?")                }
    },
    {
      { HIDE("Would you believe me if I told you,")                                         },
      { HIDE("that there are places where there is nothing but empty sky around you?")      },
      { HIDE("You can see the clouds, the rainbows and even distant planets.")              },
      { HIDE("Sometimes it even rains there!")                                              },
      { HIDE("Or a place that literally looks pretty much like how you imagine Hell,")      },
      { HIDE("with red color everywhere, fire, lava,")                                      },
      { HIDE("and even soil itself sometimes is made of flesh?")                            },
      { HIDE("There was also a great desert with nothing but dust,")                        },
      { HIDE("stretching all the way to the horizon and beyond in all directions.")         },
      { HIDE("If you dig here and there you might discover")                                },
      { HIDE("that under the sand lie ruins of some ancient city")                          },
      { HIDE("of some advanced civilization!")                                              },
      { HIDE("I wonder what happened to them and whether all")                              },
      { HIDE("that desert is somehow connected to their fate.")                             },
      { HIDE("Battling with heat and questing for water was a big challenge back there...") },
      { HIDE("...")                                                                         },
      { HIDE("Yeah, they all think I'm just crazy...")                                      }
    }
  };
}

// =============================================================================

void AINPC::SetDataGimley()
{
  AIComponentRef->OwnerGameObject->Attrs.Str.Talents = 3;
  AIComponentRef->OwnerGameObject->Attrs.Def.Talents = 3;
  AIComponentRef->OwnerGameObject->Attrs.HP.Talents = 3;

  AIComponentRef->OwnerGameObject->ObjectName = kMan;

  Data.UnacquaintedDescription = HIDE("You see a stubby looking guy with an impressive beard");

  Data.Name = HIDE("Gimley");
  Data.Job  = HIDE("Warrior");

  Data.NameResponse = HIDE("Gimley, son of Gloine.");
  Data.JobResponse  = HIDE("I'm a Warrior.");

  Data.GossipResponsesByMap[MapType::TOWN] =
  {
    {
      { HIDE("(humming)")                           },
      { HIDE("Far over the misty mountains cold,")  },
      { HIDE("To dungeons deep, and caverns old.")  },
      { HIDE("We must away, ere break of day,")     },
      { HIDE("To seek the pale, enchanted gold...") }
    },
    {
      { HIDE("(humming)")                           },
      { HIDE("Under the Mountain dark and tall,")   },
      { HIDE("The King has come unto his hall!")    },
      { HIDE("His foe is dead, the Worm of Dread,") },
      { HIDE("And ever so his foes shall fall...")  }
    },
    {
      { HIDE("(humming)")                     },
      { HIDE("From the red sky of the East,") },
      { HIDE("To the sunset in the West,")    },
      { HIDE("We have cheated Death,")        },
      { HIDE("And he has cheated us...")      }
    },
    {
      { HIDE("(humming)")                           },
      { HIDE("The forest raised a Christmas tree,") },
      { HIDE("'Twas silent and serene.")            },
      { HIDE("In winter and in summer it was,")     },
      { HIDE("Slender and so green...")             }
    }
  };
}

// =============================================================================

void AINPC::SetDataMartin()
{
  AIComponentRef->OwnerGameObject->Attrs.Mag.Talents = 3;
  AIComponentRef->OwnerGameObject->Attrs.Res.Talents = 2;
  AIComponentRef->OwnerGameObject->Attrs.Spd.Talents = 1;
  AIComponentRef->OwnerGameObject->Attrs.MP.Talents = 3;

  AIComponentRef->OwnerGameObject->ObjectName = kMan;

  Data.UnacquaintedDescription = HIDE("You see a local cleric");

  Data.Name = HIDE("Martin");
  Data.Job  = HIDE("Cleric");

  Data.NameResponse = HIDE("I'm brother Martin.");

  Data.GossipResponsesByMap[MapType::TOWN] =
  {
    {
      { HIDE("All money you spend buying my wares are going straight to charity.") }
    },
    {
      { HIDE("I guess you know about Lost City, don't you?")                                   },
      { HIDE("They say it predates our City and is buried somewhere deep underground.")        },
      { HIDE("It was the center of an ancient civilization of the times long past,")           },
      { HIDE("but some catastrophe led to its demise along with its denizens.")                },
      { HIDE("I'd say it was arrogance - the more advanced we get,")                           },
      { HIDE("the more reckless and irresponsible we become.")                                 },
      { HIDE("First we try to defeat some long-lasted issues")                                 },
      { HIDE("that plague human existence for centuries, like diseases.")                      },
      { HIDE("And it seems just and good.")                                                    },
      { HIDE("After that we decide that we need to improve our quality of life even further.") },
      { HIDE("We create artificial lights that allow us to see during the night")              },
      { HIDE("so we could extend our daytime this way, we create artificial instruments,")     },
      { HIDE("so that they will do the work for us.")                                          },
      { HIDE("Then we decide that our life must not end at all")                               },
      { HIDE("and we seek means to prolong it.")                                               },
      { HIDE("In the end we just start to think that we \"own\" everything")                   },
      { HIDE("and can do anything and nothing is impossible.")                                 },
      { HIDE("We try to become gods.")                                                         },
      { HIDE("How come that originally good intentions lead to such results?")                 },
      { HIDE("Can it be avoided? And how?...")                                                 }
    }
  };

  auto& playerRef = Application::Instance().PlayerInstance;
  if (playerRef.GetClass() == PlayerClass::ARCANIST)
  {
    TextLines bonusStr =
    {
      { HIDE("You can pray at the altar of St. Nestor the Scribe here.") },
      { HIDE("Just proceed past the Royal Gates behind me.")             }
    };

    Data.GossipResponsesByMap[MapType::TOWN].push_back(bonusStr);
  }
  else
  {
    TextLines bonusStr =
    {
      { HIDE("It is forbidden for laity to enter the Royal Gates.") }
    };

    Data.GossipResponsesByMap[MapType::TOWN].push_back(bonusStr);
  }

  TraderComponent* tc = AIComponentRef->OwnerGameObject->AddComponent<TraderComponent>();
  tc->NpcRef = this;
  tc->Init(TraderRole::CLERIC, 1000, 10);
}

// =============================================================================

void AINPC::SetDataCasey()
{
  AIComponentRef->OwnerGameObject->Attrs.Spd.Talents = 3;
  AIComponentRef->OwnerGameObject->Attrs.Def.Talents = 3;
  AIComponentRef->OwnerGameObject->Attrs.Skl.Talents = 3;
  AIComponentRef->OwnerGameObject->Attrs.HP.Talents = 1;

  AIComponentRef->OwnerGameObject->ObjectName = kMan;

  Data.UnacquaintedDescription = HIDE("You see a man wearing white cook's clothes");

  Data.Name = HIDE("Casey");
  Data.Job  = HIDE("Cook");

  Data.NameResponse = HIDE("My name's Casey.");

  Data.GossipResponsesByMap[MapType::TOWN] =
  {
    {
      { HIDE("I used to be a cook in the Navy.")                            },
      { HIDE("One day we were boarded by the enemy, on our very own ship!") },
      { HIDE("I mananged to hide myself in the refrigerator room.")         },
      { HIDE("Almost froze to death, but then boarders finally decided")    },
      { HIDE("to check upon ship's kitchen.")                               },
      { HIDE("Well, it didn't end 'well' for them in the end,")             },
      { HIDE("but it did end well for us!")                                 },
      { HIDE("All's well that ends well, eh?")                              },
      { HIDE("Hahahahaha!")                                                 }
    }
  };

  TraderComponent* tc = AIComponentRef->OwnerGameObject->AddComponent<TraderComponent>();
  tc->NpcRef = this;
  tc->Init(TraderRole::COOK, 500, 15);
}

// =============================================================================

void AINPC::SetDataMaya()
{
  AIComponentRef->OwnerGameObject->Attrs.Spd.Talents = 3;
  AIComponentRef->OwnerGameObject->Attrs.Mag.Talents = 2;
  AIComponentRef->OwnerGameObject->Attrs.Res.Talents = 2;
  AIComponentRef->OwnerGameObject->Attrs.Skl.Talents = 1;

  AIComponentRef->OwnerGameObject->ObjectName = kWoman;

  Data.IsMale = false;
  Data.UnacquaintedDescription = HIDE("You see a blue-haired woman wearing strange clothes");

  Data.Name = HIDE("Maya");
  Data.Job  = HIDE("Junker");

  Data.NameResponse = HIDE("I'm Maya.");

  Data.GossipResponsesByMap[MapType::TOWN] =
  {
    {
      { HIDE("We, the Junkers, collect discarded and abandoned items.")                  },
      { HIDE("Our culture revolves around it - we believe that every item is usable")    },
      { HIDE("as long as it's not destroyed completely.")                                },
      { HIDE("Because of this \"junky\" business, people think that we're some kind of") },
      { HIDE("a poor and homeless group of outcasts, making their living by rummaging")  },
      { HIDE("through piles of garbage, hoping to find food or something valuable.")     },
      { HIDE("Well, it may look repulsive and suspicious, but the thing is")             },
      { HIDE("you never know WHAT you'll find, so it might be some")                     },
      { HIDE("useless trinket, a cursed ring or a blessed artifact, who knows...")       }
    },
    {
      { HIDE("Think of my services as a gambling - you can try to buy something") },
      { HIDE("if you're feeling lucky and have money to spare.")                  },
      { HIDE("Also you can bring to me any unidentified items")                   },
      { HIDE("and I'll buy them from you for a higher price than other vendors.") },
      { HIDE("So it's kind of a compensation for the risk, don't you think? ;-)") },
    },
    {
      { HIDE("I hail from a distant town called Oasis.")                                    },
      { HIDE("It's a long-long journey overseas, then across the barren lands,")            },
      { HIDE("mountains and canyons, further into the West...")                             },
      { HIDE("I have an uncle and a little brother there.")                                 },
      { HIDE("He's quite a little rascal he is! XD")                                        },
      { HIDE("But it is to be expected, I suppose - our parents died")                      },
      { HIDE("when we were still very young so I guess it's up to me now")                  },
      { HIDE("to look after the little one...")                                             },
      { HIDE("I just want to see the world as much as possible,")                           },
      { HIDE("that's why I'm so far away from home.")                                       },
      { HIDE("You cannot imagine how different all of this looks compared to my homeland!") },
      { HIDE("Everything is so green!")                                                     }
    }
  };

  TraderComponent* tc = AIComponentRef->OwnerGameObject->AddComponent<TraderComponent>();
  tc->NpcRef = this;
  tc->Init(TraderRole::JUNKER, 1000, 8);
}

// =============================================================================

void AINPC::SetDataDefault()
{
  int gender = RNG::Instance().RandomRange(0, 2);

  Data.Name = "Default Actor";
  Data.CanSpeak = false;
  Data.IsMale = (gender == 0);

  AIComponentRef->OwnerGameObject->ObjectName = (gender == 0) ? kMan : kWoman;

  Data.UnacquaintedDescription = "You see a " + AIComponentRef->OwnerGameObject->ObjectName;
}

// =============================================================================

void AINPC::SetDataGriswold()
{
  AIComponentRef->OwnerGameObject->Attrs.Str.Talents = 3;
  AIComponentRef->OwnerGameObject->Attrs.Def.Talents = 2;
  AIComponentRef->OwnerGameObject->Attrs.Skl.Talents = 1;
  AIComponentRef->OwnerGameObject->Attrs.HP.Talents = 3;

  AIComponentRef->OwnerGameObject->ObjectName = kMan;

  Data.IsMale = true;
  Data.UnacquaintedDescription = HIDE("You see a bald man wearing an apron");

  Data.Name = HIDE("Griswold");
  Data.Job  = HIDE("Blacksmith");

  Data.NameResponse = HIDE("Name's Griswold.");

  Data.GossipResponsesByMap[MapType::TOWN] =
  {
    {
      { HIDE("Well, what kin I do fer ya?") }
    }
  };

  TraderComponent* tc = AIComponentRef->OwnerGameObject->AddComponent<TraderComponent>();
  tc->NpcRef = this;
  tc->Init(TraderRole::BLACKSMITH, 1000, 10);
}
