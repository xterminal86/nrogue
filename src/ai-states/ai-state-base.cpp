#include "ai-state-base.h"
#include "ai-component.h"

#include "ai-model-base.h"

AIStateBase::AIStateBase(AIModelBase* ownerRef)
{
  _ownerModel = ownerRef;
  _objectToControl = ownerRef->AIComponentRef->OwnerGameObject;
}
