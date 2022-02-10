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
#include "task-attack-effect.h"
#include "task-mine.h"

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

  std::string taskType = data->Params.at("p1");
  if (taskType == "move_rnd")
  {
    task = new TaskRandomMovement(AIComponentRef->OwnerGameObject);
  }
  else if (taskType == "idle")
  {
    task = new TaskIdle(AIComponentRef->OwnerGameObject);
  }
  else if (taskType == "attack_basic")
  {
    bool alwaysHitOverride = (data->Params.count("p2") == 1);
    task = new TaskAttackBasic(AIComponentRef->OwnerGameObject, alwaysHitOverride);
  }
  else if (taskType == "chase_player")
  {
    task = new TaskChasePlayer(AIComponentRef->OwnerGameObject);
  }
  else if (taskType == "move_away")
  {
    task = new TaskMoveAwayFromPlayer(AIComponentRef->OwnerGameObject);
  }
  else if (taskType == "save_player_pos")
  {
    task = new TaskRememberPlayerPos(AIComponentRef->OwnerGameObject);
  }
  else if (taskType == "goto_last_player_pos")
  {
    task = new TaskGotoLastPlayerPos(AIComponentRef->OwnerGameObject);
  }
  else if (taskType == "attack_effect")
  {
    std::string effectType = data->Params.at("p2");
    bool ignoreArmor = (data->Params.count("p3") == 1);

    ItemBonusStruct e;
    if (effectType == "Psd")
    {
      e.Type = ItemBonusType::POISONED;
      e.BonusValue = -AIComponentRef->OwnerGameObject->Attrs.Lvl.Get();
      e.Duration = AIComponentRef->OwnerGameObject->Attrs.Lvl.Get() * GlobalConstants::EffectDefaultDuration;
      e.Period = 5;
      e.Cumulative = true;
    }

    task = new TaskAttackEffect(AIComponentRef->OwnerGameObject, e, ignoreArmor);
  }
  else if (taskType == "attack_special")
  {
    std::string attackType = data->Params.at("p2");
    bool ignoreArmor = (data->Params.count("p3") == 1);

    task = new TaskAttackSpecial(AIComponentRef->OwnerGameObject, attackType, ignoreArmor);
  }
  else if (taskType == "attack_ranged")
  {
    std::string damageType = data->Params.at("p2");
    task = new TaskAttackRanged(AIComponentRef->OwnerGameObject,
                                damageType,
                                '*',
                                "#FF0000",
                                "#000000");
  }
  else if (taskType == "end")
  {
    task = new IgnoreFailure(AIComponentRef->OwnerGameObject);
  }
  else if (taskType == "mine")
  {
    task = new TaskMine(AIComponentRef->OwnerGameObject);
  }
  else if (taskType == "goto_last_mined_pos")
  {
    task = new TaskGotoLastMinedPos(AIComponentRef->OwnerGameObject);
  }
  else if (taskType == "move_smart")
  {
    task = new TaskMoveSmart(AIComponentRef->OwnerGameObject);
  }
  else if (taskType == "apply_effect")
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
  else if (taskType == "print_message")
  {
    std::string msg = data->Params.at("p2");
    task = new TaskPrintMessage(AIComponentRef->OwnerGameObject, msg);
  }

  // ===========================================================================

  if (task == nullptr)
  {
    auto who = Util::StringFormat("%s_%u", AIComponentRef->OwnerGameObject->ObjectName.data(), AIComponentRef->OwnerGameObject->ObjectId());
    Logger::Instance().Print(who, true);

    #ifdef DEBUG_BUILD
    DebugLog("\t[%s] no such task - %s!\n", who.data(), taskType.data());
    #endif
  }

  return task;
}

std::function<BTResult()> AIModelBase::GetConditionFunction(const ScriptNode* data)
{
  std::function<BTResult()> fn;

  std::string condType = data->Params.at("p1");

  // Roll d100 and check against p2 percent chance
  if (condType == "d100")
  {
    fn = GetD100CF(data);
  }
  // Player is linear distance visible
  else if (condType == "player_visible")
  {
    fn = GetIsPlayerVisibleCF(data);
  }
  // Checks GameObject::CanMove()
  else if (condType == "player_can_move")
  {
    fn = GetPlayerCanMoveCF(data);
  }
  // Checks if player is in square range specified by p2
  else if (condType == "player_in_range")
  {
    fn = GetInRangeCF(data);
  }
  // Player's action meter can be queried against value
  // of p3 and comparison function determined by p2.
  // Supported comparers are "gt", "lt", "eq".
  else if (condType == "player_energy")
  {
    fn = GetPlayerEnergyCF(data);
  }
  // Check if player's action meter will gain enough value
  // to become big enough to support amount of turns
  // specified by p2.
  // E.g. p2="1" means check if player's next WaitForTurn()
  // will gain enough energy for the player to be able to
  // perform >= 1 turns.
  else if (condType == "player_next_turn")
  {
    fn = GetPlayerNextTurnCF(data);
  }
  // Check if controlled GameObject's action meter
  // support amount of turns specified by p2.
  else if (condType == "turns_left")
  {
    fn = GetTurnsLeftCF(data);
  }
  // Check if actor in p2 has specific effect determined by
  // short name of SpellType specified in p3.
  // E.g. [COND p1="has_effect" p2="player" p3="Psd"]
  else if (condType == "has_effect")
  {
    fn = GetHasEffectCF(data);
  }
  // Check if current object's HP is less than 30%
  else if (condType == "hp_low")
  {
    fn = GetHPLowCF(data);
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

    Position plPos = { playerRef.PosX, playerRef.PosY };
    Position objPos = { AIComponentRef->OwnerGameObject->PosX, AIComponentRef->OwnerGameObject->PosY };

    bool res = Map::Instance().IsObjectVisible(objPos, plPos);
    if (res)
    {
      std::string plX = std::to_string(plPos.X);
      std::string plY = std::to_string(plPos.Y);

      Blackboard::Instance().Set(AIComponentRef->OwnerGameObject->ObjectId(), { "pl_x", plX });
      Blackboard::Instance().Set(AIComponentRef->OwnerGameObject->ObjectId(), { "pl_y", plY });
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
  // If range is not specified, it defaults to AgroRadius
  unsigned int range = AgroRadius;

  if (data->Params.count("p2"))
  {
    range = std::stoul(data->Params.at("p2"));
  }

  auto fn = [this, range]()
  {
    auto& playerRef = Application::Instance().PlayerInstance;
    auto& objRef    = AIComponentRef->OwnerGameObject;

    Position plPos  = { playerRef.PosX, playerRef.PosY };
    Position objPos = { objRef->PosX,   objRef->PosY   };

    //bool res = Util::IsObjectInRange(plPos, objPos, range, range);
    bool res = Util::IsObjectInRange(objPos, plPos, range, range);

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

    bool res = ((int)perc < 30);

    return res ? BTResult::Success : BTResult::Failure;
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

  std::string type = data->NodeName;

  if (type == "TREE")
  {
    n = new Root(AIComponentRef->OwnerGameObject);
  }
  else if (type == "SEQ")
  {
    n = new Sequence(AIComponentRef->OwnerGameObject);
  }
  else if (type == "SEL")
  {
    n = new Selector(AIComponentRef->OwnerGameObject);
  }
  else if (type == "SUCC")
  {
    n = new IgnoreFailure(AIComponentRef->OwnerGameObject);
  }
  else if (type == "FAIL")
  {
    n = new Failure(AIComponentRef->OwnerGameObject);
  }
  else if (type == "COND")
  {
    n = CreateConditionNode(data);
  }
  else if (type == "TASK")
  {
    n = CreateTask(data);
  }

  if (n == nullptr && type != "TASK")
  {
    auto who = Util::StringFormat("%s_%u", AIComponentRef->OwnerGameObject->ObjectName.data(), AIComponentRef->OwnerGameObject->ObjectId());
    Logger::Instance().Print(who);

    #ifdef DEBUG_BUILD
    DebugLog("[%s] no such node - %s!\n", who.data(), type.data());
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
