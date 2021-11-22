#ifndef TASKATTACKSPECIAL_H
#define TASKATTACKSPECIAL_H

#include "task-attack-basic.h"

///
/// \brief Additional action only if basic attack succeeds.
///
class TaskAttackSpecial : public TaskAttackBasic
{
  public:
    TaskAttackSpecial(GameObject* objectToControl,
                      const std::string& attackType,
                      bool ignoreArmor);

    BTResult Run() override;

  private:
    void ProcessStealHP();

    std::string _attackType;
    bool _shouldIgnoreArmor;
};

#endif // TASKATTACKSPECIAL_H
