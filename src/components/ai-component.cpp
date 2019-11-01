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
    auto str = Util::StringFormat("%s (%s) no AI model set!\n", __PRETTY_FUNCTION__, OwnerGameObject->ObjectName.data());
    Logger::Instance().Print(str);

    #ifdef DEBUG_BUILD
    printf("%s\n", str.data());
    #endif

    OwnerGameObject->FinishTurn();
  }
}
