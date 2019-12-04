#include "ai-model-base.h"
#include "ai-component.h"
#include "application.h"
#include "map.h"
#include "util.h"

#include "task-idle.h"
#include "task-random-movement.h"
#include "task-attack-basic.h"
#include "task-chase-player.h"
#include "task-move-away-from-player.h"
#include "task-goto-last-player-pos.h"
#include "task-attack-effect.h"
#include "blackboard.h"

AIModelBase::AIModelBase()
{
  _playerRef = &Application::Instance().PlayerInstance;  
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

    std::string addInfo1 = nodeData->Params.size() != 0 ? nodeData->Params.at("p1") : "";
    std::string addInfo2 = parentNodeData->Params.size() != 0 ? parentNodeData->Params.at("p1") : "";

    if (scriptNodesChecked.count(nodeData) != 1)
    {
      //printf("\tinstantiate 0x%X (%s, %s)\n", nodeData, nodeData->NodeName.data(), addInfo1.data());
      scriptNodesChecked[nodeData] = true;
      task = CreateNode(nodeData);
      behaviourNodesCreated[nodeData] = task;
    }

    if (scriptNodesChecked.count(parentNodeData) != 1)
    {
      //printf("\tinstantiate 0x%X (%s, %s)\n", parentNodeData, parentNodeData->NodeName.data(), addInfo2.data());
      scriptNodesChecked[parentNodeData] = true;
      parent = CreateNode(parentNodeData);
      behaviourNodesCreated[parentNodeData] = parent;
    }

    taskRef = behaviourNodesCreated[nodeData];
    parentRef = behaviourNodesCreated[parentNodeData];

    parentRef->AddNode(taskRef);

    //printf("\n(0x%X)(%s) %s::AddNode(%s) (0x%X)(%s)\n\n", parentNodeData, addInfo2.data(), parentNodeData->NodeName.data(), nodeData->NodeName.data(), nodeData, addInfo1.data());
  }

  _root.reset(static_cast<Root*>(parentRef));

  //FIXME: debug
  /*
  if (_root)
  {
    printf("\n");
    PrintBrains(_root->_root.get(), 0);
    printf("\n");
  }
  */
}

void AIModelBase::Update()
{
  // NOTE: Multiple turns are checked in Map::UpdateGameObjects()

  if (_root)
  {
    if (AIComponentRef->OwnerGameObject->Attrs.ActionMeter < GlobalConstants::TurnReadyValue)
    {
      AIComponentRef->OwnerGameObject->WaitForTurn();
    }
    else
    {      
      if (_root->Run() == BTResult::Failure)
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
    printf("%s\n", str.data());
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
    task = new TaskAttackBasic(AIComponentRef->OwnerGameObject);
  }
  else if (taskType == "chase_player")
  {
    task = new TaskChasePlayer(AIComponentRef->OwnerGameObject);
  }
  else if (taskType == "move_away")
  {
    task = new TaskMoveAwayFromPlayer(AIComponentRef->OwnerGameObject);
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
      e.BonusValue = AIComponentRef->OwnerGameObject->Attrs.Lvl.Get();
      e.Duration = AIComponentRef->OwnerGameObject->Attrs.Lvl.Get() * 10;
      e.Period = 5;      
      e.Cumulative = true;
    }

    task = new TaskAttackEffect(AIComponentRef->OwnerGameObject, e, ignoreArmor);
  }
  else if (taskType == "end")
  {
    task = new Failure(AIComponentRef->OwnerGameObject);
  }

  if (task == nullptr)
  {
    auto who = Util::StringFormat("%s_%u", AIComponentRef->OwnerGameObject->ObjectName.data(), AIComponentRef->OwnerGameObject->ObjectId());
    Logger::Instance().Print(who, true);

    #ifdef DEBUG_BUILD
    printf("\t[%s] no such task - %s!\n", who.data(), taskType.data());
    #endif
  }

  return task;
}

std::function<BTResult()> AIModelBase::GetConditionFunction(const ScriptNode* data)
{
  std::function<BTResult()> fn;

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

  std::string condType = data->Params.at("p1");
  if (condType == "d100")
  {
    unsigned int succ = std::stoul(data->Params.at("p2"));
    fn = [succ]()
    {      
      bool res = Util::Rolld100(succ);
      return res ? BTResult::Success : BTResult::Failure;
    };
  }
  else if (condType == "player_visible")
  {
    fn = [this]()
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

      return res ? BTResult::Success : BTResult::Failure;
    };
  }
  else if (condType == "in_range")
  {
    // If range is not specified, it defaults to AgroRadius
    unsigned int range = AgroRadius;

    if (data->Params.count("p2"))
    {
      range = std::stoul(data->Params.at("p2"));
    }

    fn = [this, range]()
    {
      auto& playerRef = Application::Instance().PlayerInstance;

      Position plPos = { playerRef.PosX, playerRef.PosY };
      Position objPos = { AIComponentRef->OwnerGameObject->PosX, AIComponentRef->OwnerGameObject->PosY };

      bool res = Util::IsObjectInRange(plPos, objPos, range, range);

      return res ? BTResult::Success : BTResult::Failure;
    };
  }
  else if (condType == "turns_left")
  {    
    fn = [this]()
    {
      bool res = AIComponentRef->OwnerGameObject->Attrs.ActionMeter >= GlobalConstants::TurnReadyValue;
      return res ? BTResult::Success : BTResult::Failure;
    };
  }
  else if (condType == "has_effect")
  {
    std::string who = data->Params.at("p2");
    std::string effect = data->Params.at("p3");

    auto invMap = Util::FlipMap(GlobalConstants::BonusDisplayNameByType);

    fn = [this, invMap, who, effect]()
    {
      bool res = (who == "player")
                 ? _playerRef->HasEffect(invMap.at(effect))
                 : AIComponentRef->OwnerGameObject->HasEffect(invMap.at(effect));

      return res ? BTResult::Success : BTResult::Failure;
    };
  }

  return fn;
}

Node* AIModelBase::CreateConditionNode(const ScriptNode* data)
{
  std::function<BTResult()> fn = GetConditionFunction(data);

  if (fn.target_type() == typeid(void))
  {
    auto str = Util::StringFormat("%s - empty COND function (%s)!", __PRETTY_FUNCTION__, data->Params.at("p1").data());
    Logger::Instance().Print(str);

    #ifdef DEBUG_BUILD
    printf("%s\n", str.data());
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
    printf("[%s] no such node - %s!\n", who.data(), type.data());
    #endif
  }

  return n;
}

void AIModelBase::PrintBrains(Node* n, int indent)
{
  std::string tabs(indent, '.');

  printf("%s0x%X - %s\n", tabs.data(), n, n->ToString().data());

  ControlNode* cn = dynamic_cast<ControlNode*>(n);
  if (cn)
  {
    for (auto& i : cn->GetChildren())
    {
      PrintBrains(i.get(), indent + 1);
    }
  }
}
