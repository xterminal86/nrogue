#include "ai-model-base.h"
#include "ai-component.h"
#include "application.h"
#include "map.h"
#include "util.h"

AIModelBase::AIModelBase()
{
  _playerRef = &Application::Instance().PlayerInstance;
}

void AIModelBase::ConstructAI()
{
  // NOTE:
  // AI behaviour tree of specific models
  // should be constructed in this method
}

void AIModelBase::Update()
{
  // NOTE: Multiple turns are checked in Map::UpdateGameObjects()

  if (_root)
  {
    _root->Run();
  }
}

