#include "ai-model-base.h"
#include "ai-component.h"
#include "application.h"
#include "map.h"
#include "util.h"

#include "task-idle.h"

AIModelBase::AIModelBase()
{
  _playerRef = &Application::Instance().PlayerInstance;
}

void AIModelBase::ConstructAI()
{
  // NOTE:
  // AI behaviour tree of specific models
  // should be constructed in this method via override.
  //
  // If that wasn't done (e.g. forgot), we add a failsafe idle state
  // to avoid infinite while loop in Map::UpdateGameObjects().

  auto& objRef = AIComponentRef->OwnerGameObject;

  auto str = Util::StringFormat("%c (%s) has no AI! Adding IdleTask...\n", objRef->Image, objRef->ObjectName.data());
  printf(str.data());
  Logger::Instance().Print(str);

  Root* rootNode = new Root(objRef);

  TaskIdle* taskIdle = new TaskIdle(objRef);
  rootNode->SetNode(taskIdle);

  _root.reset(rootNode);
}

void AIModelBase::Update()
{
  // NOTE: Multiple turns are checked in Map::UpdateGameObjects()

  if (_root)
  {
    _root->Run();
  }
}

