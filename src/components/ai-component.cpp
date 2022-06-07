#include "ai-component.h"

#include "game-object.h"

AIComponent::AIComponent()
{
  _componentHash = typeid(*this).hash_code();
}

void AIComponent::Update()
{
  if (CurrentModel != nullptr)
  {
    CurrentModel->Update();
  }
  else
  {
    auto str = Util::StringFormat("%s (%s) no AI model set!", __PRETTY_FUNCTION__, OwnerGameObject->ObjectName.data());
    Logger::Instance().Print(str, true);

    DebugLog("%s\n", str.data());

    OwnerGameObject->FinishTurn();
  }
}
