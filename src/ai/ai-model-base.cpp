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

  _aiReader.Init();
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
    printf("No AI on this object: %s!\n", AIComponentRef->OwnerGameObject->ObjectName.data());
    AIComponentRef->OwnerGameObject->FinishTurn();
  }
}

Node* AIModelBase::CreateTask(const ScriptNode* data)
{
  Node* task = nullptr;

  std::string taskType = data->Params.at("p1");
  if (taskType == "task_move_rnd")
  {
    task = new TaskRandomMovement(AIComponentRef->OwnerGameObject);
  }
  else if (taskType == "task_idle")
  {
    task = new TaskIdle(AIComponentRef->OwnerGameObject);
  }
  else if (taskType == "task_attack_basic")
  {
    task = new TaskAttackBasic(AIComponentRef->OwnerGameObject);
  }
  else if (taskType == "task_chase_player")
  {
    task = new TaskChasePlayer(AIComponentRef->OwnerGameObject);
  }
  else if (taskType == "move_away")
  {
    task = new TaskMoveAwayFromPlayer(AIComponentRef->OwnerGameObject);
  }
  else if (taskType == "end")
  {
    task = new Failure(AIComponentRef->OwnerGameObject);
  }

  if (task == nullptr)
  {
    auto who = Util::StringFormat("%s_%u", AIComponentRef->OwnerGameObject->ObjectName.data(), AIComponentRef->OwnerGameObject->ObjectId());
    printf("\t[%s] no such task - %s!\n", who.data(), taskType.data());
  }

  return task;
}

std::function<BTResult()> AIModelBase::GetConditionFunction(const ScriptNode* data)
{
  std::function<BTResult()> fn;

  std::string condType = data->Params.at("p1");
  if (condType == "d100")
  {
    unsigned int succ = std::stoul(data->Params.at("p2"));
    fn = [=]()
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

      return res ? BTResult::Success : BTResult::Failure;
    };
  }
  else if (condType == "in_range")
  {
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
  else if (condType == "turn_left")
  {
    fn = [this]()
    {
      bool res = AIComponentRef->OwnerGameObject->Attrs.ActionMeter >= GlobalConstants::TurnReadyValue;
      return res ? BTResult::Success : BTResult::Failure;
    };
  }

  return fn;
}

Node* AIModelBase::CreateConditionNode(const ScriptNode* data)
{
  std::function<BTResult()> fn = GetConditionFunction(data);

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
    printf("[%s] no such node - %s!\n", who.data(), type.data());
  }

  return n;
}
