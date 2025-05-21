#include "ai-component.h"

#include "game-object.h"

AIComponent::AIComponent()
{
}

// =============================================================================

void AIComponent::Update()
{
  if (CurrentModel != nullptr)
  {
    CurrentModel->Update();
  }
  else
  {
    #ifdef DEBUG_BUILD
    auto str = Util::StringFormat("%s (%s) no AI model set!",
                                  __PRETTY_FUNCTION__,
                                  OwnerGameObject->ObjectName.data());
    LogPrint(str, true);

    DebugLog("%s\n", str.data());
    #endif

    OwnerGameObject->FinishTurn();
  }
}
