#include "task-attack-special.h"

#include "game-object.h"
#include "printer.h"

TaskAttackSpecial::TaskAttackSpecial(GameObject* objectToControl,
                                     const std::string& attackType,
                                     bool ignoreArmor)
  : TaskAttackBasic(objectToControl)
{
  _attackType = attackType;
  _shouldIgnoreArmor = ignoreArmor;
}

BTResult TaskAttackSpecial::Run()
{
  TaskAttackBasic::Run();

  if (_attackResult.Success)
  {
    if (_shouldIgnoreArmor || !_attackResult.HasArmor)
    {
      if (_attackType == "Lch")
      {
        ProcessStealHP();
      }
    }
  }

  return BTResult::Success;
}

void TaskAttackSpecial::ProcessStealHP()
{
  bool shouldPrintMessage = !_objectToControl->Attrs.HP.IsFull();

  _objectToControl->Attrs.HP.AddMin(_attackResult.DamageDone);

  if (shouldPrintMessage)
  {
    Printer::Instance().AddMessage(_objectToControl->ObjectName + " is healed!");
  }
}
