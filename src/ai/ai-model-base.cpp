#include "ai-model-base.h"
#include "ai-component.h"
#include "application.h"
#include "map.h"
#include "util.h"

#include "task-idle.h"
#include "task-print-message.h"
#include "task-random-movement.h"
#include "task-move-smart.h"
#include "task-apply-effect.h"
#include "task-attack.h"
#include "task-attack-ranged.h"
#include "task-chase-player.h"
#include "task-move-away-from-player.h"
#include "task-remember-player-pos.h"
#include "task-goto-last-player-pos.h"
#include "task-goto-last-mined-pos.h"
#include "task-find-and-destroy-container.h"
#include "task-try-pickup-items.h"
#include "task-drink-potion.h"
#include "task-mine-tunnel.h"
#include "task-mine-block.h"

#include "blackboard.h"
#include "bts-decompiler.h"

AIModelBase::AIModelBase()
{
  _playerRef = &Application::Instance().PlayerInstance;
  _bonusTypeByDisplayName = Util::Instance().FlipMap(GlobalConstants::BonusDisplayNameByType);
}

void AIModelBase::PrepareScript()
{
}

void AIModelBase::ConstructAI()
{
  PrepareScript();

  if (!_scriptCompiled.empty())
  {
    _scriptAsText = BTSDecompiler::Instance().Decompile(_scriptCompiled);
    _scriptCompiled = std::vector<uint8_t>();
  }

  _aiReader.Init(AIComponentRef->OwnerGameObject);
  _aiReader.ParseFromString(_scriptAsText);
  _aiReader.FormTree();

  _scriptAsText = std::string();

  std::map<const ScriptNode*, bool> scriptNodesChecked;
  std::map<const ScriptNode*, Node*> behaviourNodesCreated;

  const ScriptNode* nodeData = nullptr;
  const ScriptNode* parentNodeData = nullptr;

  Node* taskRef = nullptr;
  Node* parentRef = nullptr;

  for (auto& i : _aiReader.GetConstructionOrder())
  {
    nodeData = i.first;
    parentNodeData = i.second;

    Node* task = nullptr;
    Node* parent = nullptr;

    //std::string addInfo1 = nodeData->Params.size() != 0 ? nodeData->Params.at("p1") : "";
    //std::string addInfo2 = parentNodeData->Params.size() != 0 ? parentNodeData->Params.at("p1") : "";

    if (scriptNodesChecked.count(nodeData) != 1)
    {
      //DebugLog("\tinstantiate 0x%X (%s, %s)\n", nodeData, nodeData->NodeName.data(), addInfo1.data());
      scriptNodesChecked[nodeData] = true;
      task = CreateNode(nodeData);
      behaviourNodesCreated[nodeData] = task;
    }

    if (scriptNodesChecked.count(parentNodeData) != 1)
    {
      //DebugLog("\tinstantiate 0x%X (%s, %s)\n", parentNodeData, parentNodeData->NodeName.data(), addInfo2.data());
      scriptNodesChecked[parentNodeData] = true;
      parent = CreateNode(parentNodeData);
      behaviourNodesCreated[parentNodeData] = parent;
    }

    taskRef = behaviourNodesCreated[nodeData];
    parentRef = behaviourNodesCreated[parentNodeData];

    parentRef->AddNode(taskRef);

    //DebugLog("\n(0x%X)(%s) %s::AddNode(%s) (0x%X)(%s)\n\n", parentNodeData, addInfo2.data(), parentNodeData->NodeName.data(), nodeData->NodeName.data(), nodeData, addInfo1.data());
  }

  _root.reset(static_cast<Root*>(parentRef));

  //FIXME: debug
  /*
  if (_root)
  {
    DebugLog("\n");
    PrintBrains(_root->_root.get(), 0);
    DebugLog("\n");
  }
  */
}

void AIModelBase::Update()
{
  // NOTE: Multiple turns are checked in Map::UpdateActors()

  if (_root)
  {
    if (!AIComponentRef->OwnerGameObject->CanAct())
    {
      AIComponentRef->OwnerGameObject->WaitForTurn();
    }
    else
    {
      auto res = _root->Run();

      //DebugLog("%s AIModelBase::Update() _root->Run() = %i", AIComponentRef->OwnerGameObject->ObjectName.data(), (int)res);

      // TODO: should we add [RESET] node to reset behaviour when we
      // need to perform some additional actions from last stage
      // even after FinishTurn() has been called?
      // Can we use currently unused BTResult::Running state?
      if (res == BTResult::Failure)
      {
        _root->Reset();
      }
    }
  }
  else
  {
    auto str = Util::Instance().StringFormat("No AI on this object: %s!", AIComponentRef->OwnerGameObject->ObjectName.data());
    Logger::Instance().Print(str, true);

    DebugLog("%s\n", str.data());

    AIComponentRef->OwnerGameObject->FinishTurn();
  }
}

Node* AIModelBase::CreateTask(const ScriptNode* data)
{
  Node* task = nullptr;

  std::string taskName = data->Params.at("p1");

  ScriptParamNames taskType = ScriptParamNames::ANY;

  if (GlobalConstants::BTSParamNamesByName.count(taskName) == 1)
  {
    taskType = GlobalConstants::BTSParamNamesByName.at(taskName);
  }

  switch (taskType)
  {
    case ScriptParamNames::IDLE:
      task = new TaskIdle(AIComponentRef->OwnerGameObject);
      break;

    case ScriptParamNames::MOVE_RND:
      task = new TaskRandomMovement(AIComponentRef->OwnerGameObject);
      break;

    case ScriptParamNames::MOVE_SMART:
      task = new TaskMoveSmart(AIComponentRef->OwnerGameObject);
      break;

    case ScriptParamNames::MOVE_AWAY:
      task = new TaskMoveAwayFromPlayer(AIComponentRef->OwnerGameObject);
      break;

    case ScriptParamNames::ATTACK:
    {
      bool alwaysHitOverride = (data->Params.count("p2") == 1);
      task = new TaskAttack(AIComponentRef->OwnerGameObject, alwaysHitOverride);
    }
    break;

    case ScriptParamNames::ATTACK_RANGED:
    {
      std::string damageType = data->Params.at("p2");
      std::string spellType  = data->Params.at("p3");
      task = new TaskAttackRanged(AIComponentRef->OwnerGameObject,
                                  damageType,
                                  spellType,
                                  '*',
                                  Colors::RedColor,
                                  Colors::BlackColor);
    }
    break;

    case ScriptParamNames::BREAK_STUFF:
      task = new TaskFindAndDestroyContainer(AIComponentRef->OwnerGameObject);
      break;

    case ScriptParamNames::PICK_ITEMS:
      task = new TaskTryPickupItems(AIComponentRef->OwnerGameObject);
      break;

    case ScriptParamNames::CHASE_PLAYER:
      task = new TaskChasePlayer(AIComponentRef->OwnerGameObject);
      break;

    case ScriptParamNames::SAVE_PLAYER_POS:
      task = new TaskRememberPlayerPos(AIComponentRef->OwnerGameObject);
      break;

    case ScriptParamNames::GOTO_LAST_PLAYER_POS:
      task = new TaskGotoLastPlayerPos(AIComponentRef->OwnerGameObject);
      break;

    case ScriptParamNames::GOTO_LAST_MINED_POS:
      task = new TaskGotoLastMinedPos(AIComponentRef->OwnerGameObject);
      break;

    case ScriptParamNames::MINE_TUNNEL:
    {
      bool ignorePickaxe = (data->Params.count("p2") == 1);
      task = new TaskMineTunnel(AIComponentRef->OwnerGameObject, ignorePickaxe);
    }
    break;

    case ScriptParamNames::MINE_BLOCK:
    {
      bool ignorePickaxe = (data->Params.count("p2") == 1);
      task = new TaskMineBlock(AIComponentRef->OwnerGameObject, ignorePickaxe);
    }
    break;

    case ScriptParamNames::APPLY_EFFECT:
    {
      std::string type       = data->Params.at("p2");
      std::string bonusValue = data->Params.at("p3");
      std::string duration   = data->Params.at("p4");

      ItemBonusStruct bs;
      bs.Type       = _bonusTypeByDisplayName[type];
      bs.BonusValue = std::stoi(bonusValue);
      bs.Duration   = std::stoi(duration);
      bs.Id         = AIComponentRef->OwnerGameObject->ObjectId();

      task = new TaskApplyEffect(AIComponentRef->OwnerGameObject, bs);
    }
    break;

    case ScriptParamNames::DRINK_POTION:
    {
      std::string type = data->Params.at("p2");

      ScriptParamNames ref = ScriptParamNames::ANY;
      if (GlobalConstants::BTSParamNamesByName.count(type) == 1)
      {
        ref = GlobalConstants::BTSParamNamesByName.at(type);
      }

      task = new TaskDrinkPotion(AIComponentRef->OwnerGameObject, ref);
    }
    break;

    case ScriptParamNames::PRINT_MESSAGE:
    {
      std::string msg = data->Params.at("p2");
      task = new TaskPrintMessage(AIComponentRef->OwnerGameObject, msg);
    }
    break;

    case ScriptParamNames::END:
      task = new IgnoreFailure(AIComponentRef->OwnerGameObject);
      break;

    default:
      task = nullptr;
      break;
  }

  // ===========================================================================

  if (task == nullptr)
  {
    auto who = Util::Instance().StringFormat("%s_%u", AIComponentRef->OwnerGameObject->ObjectName.data(), AIComponentRef->OwnerGameObject->ObjectId());
    Logger::Instance().Print(who, true);

    DebugLog("\t[%s] no such task - %s!\n", who.data(), taskName.data());
  }

  return task;
}

std::function<BTResult()> AIModelBase::GetConditionFunction(const ScriptNode* data)
{
  std::function<BTResult()> fn;

  std::string condType = data->Params.at("p1");

  ScriptParamNames cf = ScriptParamNames::ANY;

  if (GlobalConstants::BTSParamNamesByName.count(condType) == 1)
  {
    cf = GlobalConstants::BTSParamNamesByName.at(condType);
  }

  switch (cf)
  {
    //
    // Roll d100 and check against p2 percent chance
    //
    case ScriptParamNames::D100:
      fn = GetD100CF(data);
      break;

    //
    // Player is linear distance visible
    //
    case ScriptParamNames::PLAYER_VISIBLE:
      fn = GetIsPlayerVisibleCF();
      break;

    //
    // Checks GameObject::CanMove()
    //
    case ScriptParamNames::PLAYER_CAN_MOVE:
      fn = GetPlayerCanMoveCF();
      break;

    //
    // Checks if player is in square range specified by p2
    //
    case ScriptParamNames::PLAYER_IN_RANGE:
      fn = GetInRangeCF(data);
      break;

    //
    // Player's action meter can be queried against value
    // of p3 and comparison function determined by p2.
    // Supported comparers are "gt", "lt", "eq".
    //
    case ScriptParamNames::PLAYER_ENERGY:
      fn = GetPlayerEnergyCF(data);
      break;

    //
    // Check if player's action meter will gain enough value
    // to become big enough to support amount of turns
    // specified by p2.
    // E.g. p2="1" means check if player's next WaitForTurn()
    // will gain enough energy for the player to be able to
    // perform >= 1 turns.
    //
    case ScriptParamNames::PLAYER_NEXT_TURN:
      fn = GetPlayerNextTurnCF(data);
      break;

    //
    // Check if controlled GameObject's action meter
    // support amount of turns specified by p2.
    //
    case ScriptParamNames::TURNS_LEFT:
      fn = GetTurnsLeftCF(data);
      break;

    //
    // Checks if action meter is enough to support number of turns
    // (specified in p2) greater or less than (specified in p3)
    //
    case ScriptParamNames::TURNS_CHECK:
      fn = GetTurnsCheckCF(data);
      break;

    //
    // Check if actor in p2 has specific effect determined by
    // short name of SpellType specified in p3.
    // E.g. [COND p1="has_effect" p2="player" p3="Psd"]
    //
    case ScriptParamNames::HAS_EFFECT:
      fn = GetHasEffectCF(data);
      break;

    //
    // Check if current object's HP is less than 30%
    //
    case ScriptParamNames::HP_LOW:
      fn = GetHPLowCF();
      break;

    case ScriptParamNames::HAS_EQUIPPED:
      fn = GetHasEquippedCF(data);
      break;

    default:
      DebugLog("no such condition function - %i", (int)cf);
      break;
  }

  return fn;
}

// =============================================================================

std::function<BTResult()> AIModelBase::GetPlayerEnergyCF(const ScriptNode* data)
{
  auto fn = [this, data]()
  {
    std::string compType = data->Params.at("p2");
    int compareWith = std::stoi(data->Params.at("p3"));
    int energy = _playerRef->Attrs.ActionMeter;

    bool res = false;

    ScriptParamNames comp = ScriptParamNames::ANY;

    if (GlobalConstants::BTSParamNamesByName.count(compType) == 1)
    {
      comp = GlobalConstants::BTSParamNamesByName.at(compType);
    }

    switch (comp)
    {
      case ScriptParamNames::GT:
        res = (energy >= compareWith);
        break;

      case ScriptParamNames::LT:
        res = (energy <= compareWith);
        break;

      case ScriptParamNames::EQ:
        res = (energy == compareWith);
        break;
    }

    return res ? BTResult::Success : BTResult::Failure;
  };

  return fn;
}

std::function<BTResult()> AIModelBase::GetPlayerNextTurnCF(const ScriptNode* data)
{
  //
  // NOTE: when implementing "hit and run" tactics
  // we cannot rely on this CF to determine if player can move
  // on next turn, because if SPDs are equal there might be
  // a situation like this:
  //
  // PLAYER:
  // 0 2 4 6 8 10
  //           ^
  //
  // MONSTER:
  // 0 2 4 6 8 10
  //         ^
  //
  // So, after player finished his turn:
  //
  // PLAYER:
  // 0 2 4 6 8 10
  // ^
  //
  // MONSTER:
  // 0 2 4 6 8 10
  //           ^
  //
  // Monster processes its AI script:
  //
  // PLAYER:
  // 0 2 4 6 8 10
  //   ^
  //
  // MONSTER:
  // 0 2 4 6 8 10
  // ^
  //
  // which will eventually, after all turn increments are processed,
  // loop into the first situation.
  // Meaning that from monster's point of view
  // it will look like it's OK to perform attack,
  // because player seems to be unable to catch up,
  // but he actually does.
  //
  // UpdateActors() checks CanAct() on each actor, so in cases
  // when actor can perform two or more turns in a row
  // player's energy gain will be the same,
  // but sometimes actor must wait for its turn, which can
  // still arrive faster than player's (if actor's SPD > player's),
  // but during this time player also incremented his ActionMeter,
  // which now contains different value and may also be closer
  // to TurnReadyValue, thus resulting in different outcome of this
  // condition function check.
  //
  // So when monster is greatly faster than player
  // (I'd estimate SPD > 3), there probably won't be any problems,
  // but if SPDs are more or less close, we'll get weird AI patterns.
  //
  // What this basically means is that sometimes monsters can exhibit
  // weird behavioural pattern, like come to melee range at the player
  // on one turn and then immediately move away on another,
  // not attacking player at all, while player just stands there,
  // skipping his turns.
  //
  // For example, if monster gains 4 action meter per turn,
  // and player gains only 2, the situation can be like this:
  //
  // Monster AM: 0, Player AM: 0
  //
  // WaitForTurn():
  //
  // M: 4 - 8 - 12 => monster now acts
  // P: 2 - 4 - 6  => player can't act yet
  //
  // Monster may see this situation as if player has no chance
  // to retaliate at hit-and-run and decides to come to melee range.
  // Thus, monster spends his turn moving one tile (12 - 10 = 2).
  //
  // Now we have this:
  //
  // Monster AM: 2, Player AM: 6
  //
  // WaitForTurn():
  //
  // M: 6 - 10 => monster now acts
  // P: 8 - 10 => player can now act too, but since WaitForTurn()
  //              of actors happens before player's in
  //              Application::Run(), they will Update() first.
  //
  // So now monster sees "oh, shit, I'm gonna get fucked" and
  // moves away.
  //
  // Now if player just skips his turn by waiting,
  // we'll return to the beginning of this example
  // and the circle repeats.
  //
  auto fn = [this, data]()
  {
    int turns       = std::stoi(data->Params.at("p2"));
    int turnsEnergy = (turns * GlobalConstants::TurnReadyValue);

    int plEnergy     = _playerRef->Attrs.ActionMeter;
    int plGain       = _playerRef->GetActionIncrement();
    int plEnergyGain = plEnergy + plGain;

    bool res = (plEnergyGain >= turnsEnergy);

    return res ? BTResult::Success : BTResult::Failure;
  };

  return fn;
}

std::function<BTResult()> AIModelBase::GetPlayerCanMoveCF()
{
  auto fn = [this]()
  {
    bool res = _playerRef->CanAct();
    return res ? BTResult::Success : BTResult::Failure;
  };

  return fn;
}

std::function<BTResult()> AIModelBase::GetD100CF(const ScriptNode* data)
{
  //
  // "If a non-reference entity is captured by reference,
  // implicitly or explicitly, and the function call operator
  // of the closure object is invoked after the entity's
  // lifetime has ended, undefined behavior occurs.
  //
  // The C++ closures do not extend the lifetimes of the captured references.
  // Same applies to the lifetime of the object pointed to by the captured
  // this pointer."
  //
  // TLDR: cannot capture 'succ', 'range' et al. by reference.
  //
  unsigned int succ = std::stoul(data->Params.at("p2"));
  auto fn = [succ]()
  {
    bool res = Util::Instance().Rolld100(succ);
    return res ? BTResult::Success : BTResult::Failure;
  };

  return fn;
}

std::function<BTResult()> AIModelBase::GetIsPlayerVisibleCF()
{
  auto fn = [this]()
  {
    auto& playerRef = Application::Instance().PlayerInstance;

    auto ogo = AIComponentRef->OwnerGameObject;
    Position plPos  = playerRef.GetPosition();
    Position objPos = ogo->GetPosition();

    bool res = Map::Instance().IsObjectVisible(objPos, plPos);
    if (res)
    {
      std::string plPosStr = Util::Instance().StringFormat("%i,%i", plPos.X, plPos.Y);

      Blackboard::Instance().Set(AIComponentRef->OwnerGameObject->ObjectId(), { Strings::BlackboardKeyPlayerPos, plPosStr });
    }

    //
    // If there is line of sight and player is invisible
    // and we don't have telepathy - fail.
    //
    if (res
    && (_playerRef->HasEffect(ItemBonusType::INVISIBILITY)
     && !ogo->HasEffect(ItemBonusType::TELEPATHY)))
    {
      res = false;
    }

    return res ? BTResult::Success : BTResult::Failure;
  };

  return fn;
}

std::function<BTResult()> AIModelBase::GetInRangeCF(const ScriptNode* data)
{
  // If range is not specified, it defaults to VisibilityRadius
  Attribute vr = AIComponentRef->OwnerGameObject->VisibilityRadius;

  unsigned int range = vr.Get();

  if (data->Params.count("p2"))
  {
    range = std::stoul(data->Params.at("p2"));
  }

  auto fn = [this, range]()
  {
    auto& playerRef = Application::Instance().PlayerInstance;
    auto& objRef    = AIComponentRef->OwnerGameObject;

    bool res = Util::Instance().IsObjectInRange(objRef->GetPosition(),
                                     playerRef.GetPosition(),
                                     range,
                                     range);

    return res ? BTResult::Success : BTResult::Failure;
  };

  return fn;
}

std::function<BTResult()> AIModelBase::GetTurnsLeftCF(const ScriptNode* data)
{
  int turnsLeftToCheck = std::stoul(data->Params.at("p2"));

  int meterL = turnsLeftToCheck * GlobalConstants::TurnReadyValue;
  int meterH = (turnsLeftToCheck + 1) * GlobalConstants::TurnReadyValue;

  auto fn = [this, meterL, meterH]()
  {
    int energy = AIComponentRef->OwnerGameObject->Attrs.ActionMeter;

    //
    // Turns left = 1 is action meter >= 10 and < 20
    //
    bool res = (energy >= meterL && energy < meterH);

    return res ? BTResult::Success : BTResult::Failure;
  };

  return fn;
}

std::function<BTResult()> AIModelBase::GetTurnsCheckCF(const ScriptNode* data)
{
  int turnsToCheck = std::stoul(data->Params.at("p2"));
  std::string cond = data->Params.at("p3");

  ScriptParamNames p = GlobalConstants::BTSParamNamesByName.at(cond);

  auto fn = [this, turnsToCheck, p]()
  {
    int energyToComp = turnsToCheck * GlobalConstants::TurnReadyValue;
    int energy = AIComponentRef->OwnerGameObject->Attrs.ActionMeter;

    bool res = false;

    switch (p)
    {
      case ScriptParamNames::LE:
        res = (energy <= energyToComp);
        break;

      case ScriptParamNames::GE:
        res = (energy >= energyToComp);
        break;

      case ScriptParamNames::LT:
        res = (energy < energyToComp);
        break;

      case ScriptParamNames::GT:
        res = (energy > energyToComp);
        break;

      default:
        DebugLog("unknown condition type: %i", (int)p);
        break;
    }

    return res ? BTResult::Success : BTResult::Failure;
  };

  return fn;
}

std::function<BTResult()> AIModelBase::GetHasEffectCF(const ScriptNode* data)
{
  std::string who = data->Params.at("p2");
  std::string effect = data->Params.at("p3");

  ScriptParamNames at = (who == "player")
                        ? ScriptParamNames::PLAYER
                        : ScriptParamNames::SELF;

  auto fn = [this, at, effect]()
  {
    bool res = (at == ScriptParamNames::PLAYER)
               ? _playerRef->HasEffect(_bonusTypeByDisplayName.at(effect))
               : AIComponentRef->OwnerGameObject->HasEffect(_bonusTypeByDisplayName.at(effect));

    return res ? BTResult::Success : BTResult::Failure;
  };

  return fn;
}

std::function<BTResult()> AIModelBase::GetHPLowCF()
{
  auto fn = [this]()
  {
    int maxHp = AIComponentRef->OwnerGameObject->Attrs.HP.Max().OriginalValue();
    int curHp = AIComponentRef->OwnerGameObject->Attrs.HP.Min().Get();

    float perc = (float)curHp * 100.0f / (float)maxHp;

    bool res = ((int)perc <= 25);

    return res ? BTResult::Success : BTResult::Failure;
  };

  return fn;
}

std::function<BTResult()> AIModelBase::GetHasEquippedCF(const ScriptNode* data)
{
  std::string eqType = data->Params.at("p2");

  EquipmentCategory eqCat = EquipmentCategory::NOT_EQUIPPABLE;
  ScriptParamNames paramName = ScriptParamNames::ANY;

  if (GlobalConstants::BTSParamNamesByName.count(eqType) == 1)
  {
    paramName = GlobalConstants::BTSParamNamesByName.at(eqType);
  }

  if (_eqCats.count(paramName) == 1)
  {
    eqCat = _eqCats.at(paramName);
  }

  auto fn = [this, eqCat]()
  {
    if (eqCat == EquipmentCategory::NOT_EQUIPPABLE)
    {
      return BTResult::Failure;
    }

    EquipmentComponent* ec = AIComponentRef->OwnerGameObject->GetComponent<EquipmentComponent>();
    if (ec == nullptr)
    {
      return BTResult::Failure;
    }

    ItemComponent* item = nullptr;

    if (eqCat == EquipmentCategory::RING)
    {
      for (auto& r : ec->EquipmentByCategory[eqCat])
      {
        if (r != nullptr)
        {
          item = r;
          break;
        }
      }
    }
    else
    {
      item = ec->EquipmentByCategory[eqCat][0];
    }

    //
    // Do not check for specific type of weapon (wand or sword),
    // check existence only.
    //
    return (item != nullptr) ? BTResult::Success : BTResult::Failure;
  };

  return fn;
}

// =============================================================================

Node* AIModelBase::CreateConditionNode(const ScriptNode* data)
{
  std::function<BTResult()> fn = GetConditionFunction(data);

  if (!Util::Instance().IsFunctionValid(fn))
  {
    auto str = Util::Instance().StringFormat("%s - empty COND function (%s)!", __PRETTY_FUNCTION__, data->Params.at("p1").data());
    Logger::Instance().Print(str);

    DebugLog("%s\n", str.data());
  }

  Node* node = new Condition(AIComponentRef->OwnerGameObject, fn);

  return node;
}

Node* AIModelBase::CreateNode(const ScriptNode* data)
{
  Node* n = nullptr;

  std::string nodeName = data->NodeName;

  ScriptTaskNames nodeType = ScriptTaskNames::NONE;

  if (GlobalConstants::BTSTaskNamesByName.count(nodeName) == 1)
  {
    nodeType = GlobalConstants::BTSTaskNamesByName.at(nodeName);
  }

  switch (nodeType)
  {
    case ScriptTaskNames::TREE:
      n = new Root(AIComponentRef->OwnerGameObject);
      break;

    case ScriptTaskNames::SEQ:
      n = new Sequence(AIComponentRef->OwnerGameObject);
      break;

    case ScriptTaskNames::SEL:
      n = new Selector(AIComponentRef->OwnerGameObject);
      break;

    case ScriptTaskNames::SUCC:
      n = new IgnoreFailure(AIComponentRef->OwnerGameObject);
      break;

    case ScriptTaskNames::FAIL:
      n = new Failure(AIComponentRef->OwnerGameObject);
      break;

    case ScriptTaskNames::COND:
      n = CreateConditionNode(data);
      break;

    case ScriptTaskNames::TASK:
      n = CreateTask(data);
      break;
  }

  if (n == nullptr && nodeType != ScriptTaskNames::TASK)
  {
    auto who = Util::Instance().StringFormat("%s_%u", AIComponentRef->OwnerGameObject->ObjectName.data(), AIComponentRef->OwnerGameObject->ObjectId());
    Logger::Instance().Print(who);

    DebugLog("[%s] no such node - %s!\n", who.data(), nodeName.data());
  }

  return n;
}

void AIModelBase::PrintBrains(Node* n, int indent)
{
  std::string tabs(indent, '.');

  std::string nodeName = n->ToString();
  DebugLog("%s0x%X - %s\n", tabs.data(), n, nodeName.data());

  ControlNode* cn = dynamic_cast<ControlNode*>(n);
  if (cn)
  {
    for (auto& i : cn->GetChildren())
    {
      PrintBrains(i.get(), indent + 1);
    }
  }
}
