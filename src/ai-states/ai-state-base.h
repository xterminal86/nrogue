#ifndef AI_STATE_BASE_H
#define AI_STATE_BASE_H

class AIModelBase;
class GameObject;

class AIStateBase
{
  public:
    AIStateBase(AIModelBase* ownerRef);
    virtual ~AIStateBase() = default;

    virtual void Run() = 0;
    virtual void Exit() = 0;
    virtual void Enter() = 0;

  protected:
    // Basically a shortcut to
    // _ownerModel->AIComponentRef->OwnerGameObject
    GameObject* _objectToControl = nullptr;

    AIModelBase* _ownerModel = nullptr;
};

#endif
