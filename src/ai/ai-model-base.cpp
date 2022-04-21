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
#include "task-attack-basic.h"
#include "task-attack-special.h"
#include "task-attack-ranged.h"
#include "task-chase-player.h"
#include "task-move-away-from-player.h"
#include "task-remember-player-pos.h"
#include "task-goto-last-player-pos.h"
#include "task-goto-last-mined-pos.h"
#include "task-find-and-destroy-container.h"
#include "task-attack-effect.h"
#include "task-try-pickup-items.h"
#include "task-drink-potion.h"
#include "task-mine-tunnel.h"
#include "task-mine-block.h"

#include "blackboard.h"

AIModelBase::AIModelBase()
{
  _playerRef = &Application::Instance().PlayerInstance;
  _bonusTypeByDisplayName = Util::FlipMap(GlobalConstants::BonusDisplayNameByType);
}

void AIModelBase::PrepareScript()
{
}

void AIModelBase::ConstructAI()
{
  PrepareScript();

  _aiReader.Init(AIComponentRef->OwnerGameObject);
  _aiReader.ParseFromString(_script);
  _aiReader.FormTree();

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
  // NOTE: Multiple turns are checked in Map::UpdateGameObjects()

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
    auto str = Util::StringFormat("No AI on this object: %s!", AIComponentRef->OwnerGameObject->ObjectName.data());
    Logger::Instance().Print(str, true);

    #ifdef DEBUG_BUILD
    DebugLog("%s\n", str.data());
    #endif

    AIComponentRef->OwnerGameObject->FinishTurn();
  }
}

Node* AIModelBase::CreateTask(const ScriptNode* data)
{
  Node* task = nullptr;

  std::string taskName = data->Params.at("p1");

  AITasks taskType = AITasks::NONE;

  if (_aiTasksByName.count(taskName) == 1)
  {
    taskType = _aiTasksByName.at(taskName);
  }

  switch (taskType)
  {
    case AITasks::IDLE:
      task = new TaskIdle(AIComponentRef->OwnerGameObject);
      break;

    case AITasks::MOVE_RND:
      task = new TaskRandomMovement(AIComponentRef->OwnerGameObject);
      break;

    case AITasks::MOVE_SMART:
      task = new TaskMoveSmart(AIComponentRef->OwnerGameObject);
      break;

    case AITasks::MOVE_AWAY:
      task = new TaskMoveAwayFromPlayer(AIComponentRef->OwnerGameObject);
      break;

    case AITasks::ATTACK_BASIC:
    {
      bool alwaysHitOverride = (data->Params.count("p2") == 1);
      task = new TaskAttackBasic(AIComponentRef->OwnerGameObject, alwaysHitOverride);
    }
    break;

    case AITasks::ATTACK_EFFECT:
    {
      std::string effectType = data->Params.at("p2");
      bool ignoreArmor = (data->Params.count("p3") == 1);

      ItemBonusStruct e;
      if (effectType == "Psd")
      {
        e.Type       = ItemBonusType::POISONED;
        e.BonusValue = -1;
        e.Duration   = GlobalConstants::EffectDefaultDuration;
        e.Period     = GlobalConstants::EffectDurationSkipsForTurn * 2;
        e.Cumulative = true;
      }

      task = new TaskAttackEffect(AIComponentRef->OwnerGameObject, e, ignoreArmor);
    }
    break;

    case AITasks::ATTACK_SPECIAL:
    {
      std::string attackType = data->Params.at("p2");
      bool ignoreArmor = (data->Params.count("p3") == 1);

      task = new TaskAttackSpecial(AIComponentRef->OwnerGameObject, attackType, ignoreArmor);
    }
    break;

    case AITasks::ATTACK_RANGED:
    {
      std::string damageType = data->Params.at("p2");
      std::string spellType  = data->Params.at("p3");
      task = new TaskAttackRanged(AIComponentRef->OwnerGameObject,
                                  damageType,
                                  spellType,
                                  '*',
                                  "#FF0000",
                                  "#000000");
    }
    break;

    case AITasks::BREAK_STUFF:
      task = new TaskFindAndDestroyContainer(AIComponentRef->OwnerGameObject);
      break;

    case AITasks::PICK_ITEMS:
      task = new TaskTryPickupItems(AIComponentRef->OwnerGameObject);
      break;

    case AITasks::CHASE_PLAYER:
      task = new TaskChasePlayer(AIComponentRef->OwnerGameObject);
      break;

    case AITasks::SAVE_PLAYER_POS:
      task = new TaskRememberPlayerPos(AIComponentRef->OwnerGameObject);
      break;

    case AITasks::GOTO_LAST_PLAYER_POS:
      task = new TaskGotoLastPlayerPos(AIComponentRef->OwnerGameObject);
      break;

    case AITasks::GOTO_LAST_MINED_POS:
      task = new TaskGotoLastMinedPos(AIComponentRef->OwnerGameObject);
      break;

    case AITasks::MINE_TUNNEL:
    {
      bool ignorePickaxe = (data->Params.count("p2") == 1);
      task = new TaskMineTunnel(AIComponentRef->OwnerGameObject, ignorePickaxe);
    }
    break;

    case AITasks::MINE_BLOCK:
    {
      bool ignorePickaxe = (data->Params.count("p2") == 1);
      task = new TaskMineBlock(AIComponentRef->OwnerGameObject, ignorePickaxe);
    }
    break;

    case AITasks::APPLY_EFFECT:
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

    case AITasks::DRINK_POTION:
    {
      std::string type = data->Params.at("p2");
      task = new TaskDrinkPotion(AIComponentRef->OwnerGameObject, type);
    }
    break;

    case AITasks::PRINT_MESSAGE:
    {
      std::string msg = data->Params.at("p2");
      task = new TaskPrintMessage(AIComponentRef->OwnerGameObject, msg);
    }
    break;

    case AITasks::END:
      task = new IgnoreFailure(AIComponentRef->OwnerGameObject);
      break;

    default:
      task = nullptr;
      break;
  }

  // ===========================================================================

  if (task == nullptr)
  {
    auto who = Util::StringFormat("%s_%u", AIComponentRef->OwnerGameObject->ObjectName.data(), AIComponentRef->OwnerGameObject->ObjectId());
    Logger::Instance().Print(who, true);

    #ifdef DEBUG_BUILD
    DebugLog("\t[%s] no such task - %s!\n", who.data(), taskName.data());
    #endif
  }

  return task;
}

std::function<BTResult()> AIModelBase::GetConditionFunction(const ScriptNode* data)
{
  std::function<BTResult()> fn;

  std::string condType = data->Params.at("p1");

  AIConditionFunctions cf = AIConditionFunctions::NONE;

  if (_cfByName.count(condType) == 1)
  {
    cf = _cfByName.at(condType);
  }

  switch (cf)
  {
    // Roll d100 and check against p2 percent chance
    case AIConditionFunctions::D100:
      fn = GetD100CF(data);
      break;

    // Player is linear distance visible
    case AIConditionFunctions::PLAYER_VISIBLE:
      fn = GetIsPlayerVisibleCF(data);
      break;

    // Checks GameObject::CanMove()
    case AIConditionFunctions::PLAYER_CAN_MOVE:
      fn = GetPlayerCanMoveCF(data);
      break;

    // Checks if player is in square range specified by p2
    case AIConditionFunctions::PLAYER_IN_RANGE:
      fn = GetInRangeCF(data);
      break;

    // Player's action meter can be queried against value
    // of p3 and comparison function determined by p2.
    // Supported comparers are "gt", "lt", "eq".
    case AIConditionFunctions::PLAYER_ENERGY:
      fn = GetPlayerEnergyCF(data);
      break;

    // Check if player's action meter will gain enough value
    // to become big enough to support amount of turns
    // specified by p2.
    // E.g. p2="1" means check if player's next WaitForTurn()
    // will gain enough energy for the player to be able to
    // perform >= 1 turns.
    case AIConditionFunctions::PLAYER_NEXT_TURN:
      fn = GetPlayerNextTurnCF(data);
      break;

    // Check if controlled GameObject's action meter
    // support amount of turns specified by p2.
    case AIConditionFunctions::TURNS_LEFT:
      fn = GetTurnsLeftCF(data);
      break;

    // Check if actor in p2 has specific effect determined by
    // short name of SpellType specified in p3.
    // E.g. [COND p1="has_effect" p2="player" p3="Psd"]
    case AIConditionFunctions::HAS_EFFECT:
      fn = GetHasEffectCF(data);
      break;

    // Check if current object's HP is less than 30%
    case AIConditionFunctions::HP_LOW:
      fn = GetHPLowCF(data);
      break;

    case AIConditionFunctions::HAS_EQUIPPED:
      fn = GetHasEquippedCF(data);
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

    if (compType == "gt")
    {
      res = (energy >= compareWith);
    }
    else if (compType == "lt")
    {
      res = (energy <= compareWith);
    }
    else if (compType == "eq")
    {
      res = (energy == compareWith);
    }

    return res ? BTResult::Success : BTResult::Failure;
  };

  return fn;
}

std::function<BTResult()> AIModelBase::GetPlayerNextTurnCF(const ScriptNode* data)
{
  auto fn = [this, data]()
  {
    int turns = std::stoi(data->Params.at("p2"));

    int energy = _playerRef->Attrs.ActionMeter;
    int actionIncrement = _playerRef->GetActionIncrement();
    int energyGain = energy + actionIncrement;

    bool res = (energyGain >= (turns * GlobalConstants::TurnReadyValue));

    return res ? BTResult::Success : BTResult::Failure;
  };

  return fn;
}

std::function<BTResult()> AIModelBase::GetPlayerCanMoveCF(const ScriptNode* data)
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
    bool res = Util::Rolld100(succ);
    return res ? BTResult::Success : BTResult::Failure;
  };

  return fn;
}

std::function<BTResult()> AIModelBase::GetIsPlayerVisibleCF(const ScriptNode* data)
{
  auto fn = [this]()
  {
    auto& playerRef = Application::Instance().PlayerInstance;

    Position plPos  = playerRef.GetPosition();
    Position objPos = AIComponentRef->OwnerGameObject->GetPosition();

    bool res = Map::Instance().IsObjectVisible(objPos, plPos);
    if (res)
    {
      std::string plPosStr = Util::StringFormat("%i,%i", plPos.X, plPos.Y);

      Blackboard::Instance().Set(AIComponentRef->OwnerGameObject->ObjectId(), { Strings::BlackboardKeyPlayerPos, plPosStr });
    }

    // TODO: what if monsters can see invisible?
    if (res && playerRef.HasEffect(ItemBonusType::INVISIBILITY))
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

    //bool res = Util::IsObjectInRange(plPos, objPos, range, range);
    bool res = Util::IsObjectInRange(objRef->GetPosition(),
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

  auto fn = [this, turnsLeftToCheck]()
  {
    int energy = AIComponentRef->OwnerGameObject->Attrs.ActionMeter;

    int left = energy - GlobalConstants::TurnReadyValue * turnsLeftToCheck;

    bool res = (left >= 0);

    return res ? BTResult::Success : BTResult::Failure;
  };

  return fn;
}

std::function<BTResult()> AIModelBase::GetHasEffectCF(const ScriptNode* data)
{
  std::string who = data->Params.at("p2");
  std::string effect = data->Params.at("p3");

  auto fn = [this, who, effect]()
  {
    bool res = (who == "player")
               ? _playerRef->HasEffect(_bonusTypeByDisplayName.at(effect))
               : AIComponentRef->OwnerGameObject->HasEffect(_bonusTypeByDisplayName.at(effect));

    return res ? BTResult::Success : BTResult::Failure;
  };

  return fn;
}

std::function<BTResult()> AIModelBase::GetHPLowCF(const ScriptNode* data)
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
  auto fn = [this, eqType]()
  {
    if (_eqCategoryByName.count(eqType) == 0)
    {
      return BTResult::Failure;
    }

    auto cat = _eqCategoryByName.at(eqType);

    EquipmentComponent* ec = AIComponentRef->OwnerGameObject->GetComponent<EquipmentComponent>();
    if (ec == nullptr)
    {
      return BTResult::Failure;
    }

    ItemComponent* item = nullptr;

    if (cat == EquipmentCategory::RING)
    {
      for (auto& r : ec->EquipmentByCategory[cat])
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
      item = ec->EquipmentByCategory[cat][0];
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

  if (!Util::IsFunctionValid(fn))
  {
    auto str = Util::StringFormat("%s - empty COND function (%s)!", __PRETTY_FUNCTION__, data->Params.at("p1").data());
    Logger::Instance().Print(str);

    #ifdef DEBUG_BUILD
    DebugLog("%s\n", str.data());
    #endif
  }

  Node* node = new Condition(AIComponentRef->OwnerGameObject, fn);

  return node;
}

Node* AIModelBase::CreateNode(const ScriptNode* data)
{
  Node* n = nullptr;

  std::string nodeName = data->NodeName;

  BTNodeType nodeType = BTNodeType::NONE;

  if (_btNodeTypeByName.count(nodeName) == 1)
  {
    nodeType = _btNodeTypeByName.at(nodeName);
  }

  switch (nodeType)
  {
    case BTNodeType::TREE:
      n = new Root(AIComponentRef->OwnerGameObject);
      break;

    case BTNodeType::SEQ:
      n = new Sequence(AIComponentRef->OwnerGameObject);
      break;

    case BTNodeType::SEL:
      n = new Selector(AIComponentRef->OwnerGameObject);
      break;

    case BTNodeType::SUCC:
      n = new IgnoreFailure(AIComponentRef->OwnerGameObject);
      break;

    case BTNodeType::FAIL:
      n = new Failure(AIComponentRef->OwnerGameObject);
      break;

    case BTNodeType::COND:
      n = CreateConditionNode(data);
      break;

    case BTNodeType::TASK:
      n = CreateTask(data);
      break;
  }

  if (n == nullptr && nodeType != BTNodeType::TASK)
  {
    auto who = Util::StringFormat("%s_%u", AIComponentRef->OwnerGameObject->ObjectName.data(), AIComponentRef->OwnerGameObject->ObjectId());
    Logger::Instance().Print(who);

    #ifdef DEBUG_BUILD
    DebugLog("[%s] no such node - %s!\n", who.data(), nodeName.data());
    #endif
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
