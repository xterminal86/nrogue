#include "task-attack.h"

#include "application.h"
#include "game-object.h"
#include "printer.h"

#ifdef DEBUG_BUILD
#include "logger.h"
#endif

TaskAttack::TaskAttack(GameObject* objectToControl,
                       bool alwaysHitOverride)
  : Node(objectToControl)
{
  _alwaysHitOverride = alwaysHitOverride;
  _playerRef = &Application::Instance().PlayerInstance;
}

// =============================================================================

BTResult TaskAttack::Run()
{
  //DebugLog("[TaskAttack]\n");

  switch (_objectToControl->Type)
  {
    // -------------------------------------------------------------------------

    case GameObjectType::RAT:
    case GameObjectType::BAT:
    case GameObjectType::TROLL:
    case GameObjectType::SKELETON:
    case GameObjectType::STALKER:
    {
      auto attRes = AttackUnarmed();
      if (attRes.first)
      {
        _playerRef->ReceiveDamage(_objectToControl,
                                  attRes.second,
                                  false,
                                  false,
                                  false,
                                  false);
      }
    }
    break;

    // -------------------------------------------------------------------------

    case GameObjectType::VAMPIRE_BAT:
    {
      auto attRes = AttackUnarmed();
      if (attRes.first)
      {
        _playerRef->ReceiveDamage(_objectToControl,
                                  attRes.second,
                                  false,
                                  false,
                                  false,
                                  false);

        if (!PlayerHasArmor())
        {
          bool shouldPrintMessage = !_objectToControl->Attrs.HP.IsFull();

          _objectToControl->Attrs.HP.AddMin(attRes.second);

          if (shouldPrintMessage)
          {
            Printer::Instance().AddMessage(_objectToControl->ObjectName +
                                           " is healed!");
          }
        }
      }
    }
    break;

    // -------------------------------------------------------------------------

    case GameObjectType::SPIDER:
    case GameObjectType::SHELOB:
    case GameObjectType::ZOMBIE:
    {
      auto attRes = AttackUnarmed();
      if (attRes.first)
      {
        _playerRef->ReceiveDamage(_objectToControl,
                                  attRes.second,
                                  false,
                                  false,
                                  false,
                                  false);

        if (!PlayerHasArmor())
        {
          ItemBonusStruct e;

          e.Type       = ItemBonusType::POISONED;
          e.BonusValue = -1;
          e.Duration   = GlobalConstants::EffectDefaultDuration;
          e.Period     = GlobalConstants::EffectDurationSkipsForTurn * 2;
          e.Cumulative = true;

          if (_objectToControl->Type == GameObjectType::ZOMBIE
           && Util::Rolld100(50))
          {
            _playerRef->AddEffect(e);
          }
          else
          {
            _playerRef->AddEffect(e);
          }
        }
      }
    }
    break;

    // -------------------------------------------------------------------------

    case GameObjectType::KOBOLD:
    case GameObjectType::HEROBRINE:
    case GameObjectType::MAD_MINER:
      AttackWithWeapon();
      break;

    // -------------------------------------------------------------------------

    case GameObjectType::WRAITH:
    {
      auto attRes = AttackUnarmed({ 2, 4 });
      if (attRes.first)
      {
        Printer::Instance().AddMessage("Your lifeforce is being drained!");
        _playerRef->AwardExperience(-attRes.second);
      }
    }
    break;

    // -------------------------------------------------------------------------

    default:
      DebugLog("[WAR] %s - attack not defined!",
               _objectToControl->ObjectName.data());
      break;
  }

  _objectToControl->FinishTurn();

  return BTResult::Success;
}

// =============================================================================

bool TaskAttack::AttackWithWeapon()
{
  bool result = false;

  ItemComponent* weapon = nullptr;

  EquipmentComponent* ec = _objectToControl->GetComponent<EquipmentComponent>();
  if (ec != nullptr)
  {
    weapon = ec->EquipmentByCategory[EquipmentCategory::WEAPON][0];
  }

  bool ignoreArmor = false;
  if (weapon != nullptr)
  {
    ignoreArmor = weapon->Data.HasBonus(ItemBonusType::IGNORE_ARMOR);
  }

  int hitChance = Util::CalculateHitChanceMelee(_objectToControl,
                                                _playerRef);

  #ifdef DEBUG_BUILD
  LogAttackData(hitChance);
  #endif

  int dmg = 0;

  if (_alwaysHitOverride || Util::Rolld100(hitChance))
  {
    dmg = Util::CalculateDamageValue(_objectToControl,
                                     _playerRef,
                                     weapon,
                                     false);

    Application::Instance().DisplayAttack(_playerRef,
                                          GlobalConstants::DisplayAttackDelayMs,
                                          std::string(),
                                          Colors::RedColor);

    // TODO: process weapon damage bonuses (life leech, knockback etc.)

    _playerRef->ReceiveDamage(_objectToControl,
                              dmg,
                              false,
                              ignoreArmor,
                              false,
                              false);

    //
    // Melee attack with ranged weapon shouldn't damage it.
    //
    if (weapon != nullptr
     && weapon->Data.ItemType_ != ItemType::RANGED_WEAPON)
    {
      if (PlayerHasArmor())
      {
        Util::TryToDamageEquipment(_objectToControl, weapon, -1);
      }
    }

    result = true;
  }
  else
  {
    auto msg = Util::StringFormat(Strings::FmtSMissed,
                                  Util::GetGameObjectDisplayCharacter(
                                    _objectToControl
                                  ).data());
    Application::Instance().DisplayAttack(_playerRef,
                                          GlobalConstants::DisplayAttackDelayMs,
                                          msg,
                                          Colors::WhiteColor);

    result = false;
  }

  return result;
}

// =============================================================================

AttackResult TaskAttack::AttackUnarmed(const DamageRoll& damageRoll)
{
  AttackResult res;

  int hitChance = Util::CalculateHitChanceMelee(_objectToControl,
                                                _playerRef);

  #if DEBUG_BUILD
  LogAttackData(hitChance);
  #endif

  int dmg = 0;

  if (_alwaysHitOverride || Util::Rolld100(hitChance))
  {
    Application::Instance().DisplayAttack(_playerRef,
                                          GlobalConstants::DisplayAttackDelayMs,
                                          std::string(),
                                          Colors::RedColor);

    dmg = Util::CalculateDamageValue(_objectToControl,
                                     _playerRef,
                                     nullptr,
                                     false);

    if (damageRoll.first != 0 && damageRoll.second != 0)
    {
      dmg += Util::RollDices(damageRoll.first, damageRoll.second);
    }

    res.first = true;
    res.second = dmg;
  }
  else
  {
    auto msg = Util::StringFormat(Strings::FmtSMissed,
                                  Util::GetGameObjectDisplayCharacter(
                                    _objectToControl
                                  ).data());
    Application::Instance().DisplayAttack(_playerRef,
                                          GlobalConstants::DisplayAttackDelayMs,
                                          msg,
                                          Colors::WhiteColor);
    res.first = false;
    res.second = 0;
  }

  return res;
}

// =============================================================================

bool TaskAttack::PlayerHasArmor()
{
  ItemComponent* armor =
      _playerRef->Equipment->EquipmentByCategory[EquipmentCategory::TORSO][0];
  return (armor != nullptr);
}

// =============================================================================

#if DEBUG_BUILD
void TaskAttack::LogAttackData(int hitChance)
{
  auto logMsg = Util::StringFormat("%s (SKL %i, LVL %i) "
                                   "attacks Player (SKL: %i, LVL %i): "
                                   "chance = %i",
                                   _objectToControl->ObjectName.data(),
                                   _objectToControl->Attrs.Skl.Get(),
                                   _objectToControl->Attrs.Lvl.Get(),
                                   _playerRef->Attrs.Skl.Get(),
                                   _playerRef->Attrs.Lvl.Get(),
                                   hitChance);
  Logger::Instance().Print(logMsg);
}
#endif
