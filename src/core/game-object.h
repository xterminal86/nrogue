#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <string>
#include <map>
#include <memory>
#include <functional>
#include <queue>

#ifndef USE_SDL
#include <ncurses.h>
#endif

#include "component.h"
#include "constants.h"
#include "enumerations.h"
#include "attribute.h"
#include "item-data.h"
#include "util.h"

class GameObjectInfo;
class MapLevelBase;
class Position;
class Node;
class NRS;

class GameObject
{
  public:
    GameObject(MapLevelBase* levelOwner = nullptr);
    virtual ~GameObject();

    GameObject(MapLevelBase* levelOwner,
               int x, int y,
               int avatar,
               const uint32_t& htmlColor,
               const uint32_t& bgColor = Colors::BlackColor);

    void SetLevelOwner(MapLevelBase* levelOwner);

    IR Interact();

    void Init(MapLevelBase* levelOwner,
              int x, int y,
              int avatar,
              const uint32_t& fgColor,
              const uint32_t& bgColor = Colors::BlackColor);

    ///
    /// Adds \p dx and \p dy to corresponding game object's coordinates.
    /// Use to move actors only.
    ///
    bool Move(int dx, int dy);

    ///
    /// Sets game object's coordinates to \p x and \p y.
    /// Use to move actors only.
    ///
    bool MoveTo(int x, int y, bool force = false);
    bool MoveTo(const Position& pos, bool force = false);

    bool CanMoveTo(const Position& pos);

    void Draw(const uint32_t& overrideFgColor = Colors::None,
              const uint32_t& overrideBgColor = Colors::None,
              int imageOverride = -1);

    template <typename T, typename ... Args>
    inline T* AddComponent(Args ... args)
    {
      if (_components.count(typeid(T).hash_code()) == 1)
      {
        DebugLog("[WAR] trying to add existing component %s "
                 "on game object [0x%X] - returning existing 0x%X",
                 typeid(T).name(),
                 this,
                 _components[typeid(T).hash_code()].get());

        return static_cast<T*>(_components[typeid(T).hash_code()].get());
      }

      std::unique_ptr<T> cp = std::make_unique<T>(args ...);

      cp->Prepare(this);

      // cp is null after std::move
      _components[typeid(T).hash_code()] = std::move(cp);

      return static_cast<T*>(_components[typeid(T).hash_code()].get());
    }

    template <typename T>
    inline T* GetComponent()
    {
      for (auto& c : _components)
      {
        if (c.second.get() != nullptr && typeid(*c.second.get()) == typeid(T))
        {
          return static_cast<T*>(c.second.get());
        }
      }

      return nullptr;
    }

    template <typename T>
    inline bool HasComponent()
    {
      return (GetComponent<T>() != nullptr);
    }

    void Serialize(NRS& section);

    void MakeTile(const GameObjectInfo& t,
                  GameObjectType typeOverride = GameObjectType::GROUND);

    void Update();

    // ---------------------------------------
    //
    // Item imposed effects and stat modifiers
    //
    void ApplyBonuses(ItemComponent* itemRef);
    void UnapplyBonuses(ItemComponent* itemRef);

    void ApplyBonus(ItemComponent* itemRef, const ItemBonusStruct& bonus);
    void UnapplyBonus(ItemComponent* itemRef, const ItemBonusStruct& bonus);

    // ---------------------------------------

    const Position& GetPosition();

    Attribute VisibilityRadius;

    int Money = 0;

    int PosX = 0;
    int PosY = 0;

    TransformedRoom ZoneMarker = TransformedRoom::UNMARKED;

    // -------------------------------------------------------------------------
    //
    // E.g. to disallow walking into a tile for NPC
    //
    bool Special = false;

    bool Blocking    = false;
    bool BlocksSight = false;
    bool Revealed    = false;
    bool Corporeal   = true;

    //
    // Determines if objects on the map tile should be drawn.
    //
    bool Visible = false;

    // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //
    // Don't serialize these because they either don't make sense
    // or their value is driven by code.
    //

    //
    // NOTE: set manually for NPCs during creation via GameObject::Move(),
    // because some game objects can be stepped on (e.g. bench, money etc)
    //
    bool Occupied = false;

    //
    // Will be set to true if object needs to be destroyed on next game update.
    //
    bool IsDestroyed = false;

    // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    //
    // Undead or not.
    //
    bool IsLiving = false;

    void MaskToBoolFlags(const uint16_t mask);

    // -------------------------------------------------------------------------

    int Image = '?';

    uint32_t FgColor = Colors::WhiteColor;
    uint32_t BgColor = Colors::MagentaColor;

    std::string ObjectName;
    std::string FogOfWarName;

    std::function<IR()> InteractionCallback;

    std::function<void()> GenerateLootFunction;
    std::function<void(GameObject*)> OnDestroy;

    size_t ComponentsSize();

    virtual bool ReceiveDamage(GameObject* from,
                               int amount,
                               bool isMagical,
                               bool ignoreArmor,
                               bool directDamage,
                               bool suppressLog);
    bool CanAct();

    void FinishTurn();
    void WaitForTurn();

    virtual void AwardExperience(int amount);
    virtual void LevelUp(int baseHpOverride = -1);
    virtual void LevelDown();

    void AddEffect(const ItemBonusStruct& effectToAdd);

    //
    // Erases only effects
    // that are not connected to any equipped items.
    //
    void DispelEffectFirstFound(const ItemBonusType& type);
    void DispelEffectsAllOf(const ItemBonusType& type);

#ifdef DEBUG_BUILD
    void DispelEffects();
#endif

    //
    // Removes effects imposed by items or objects.
    //
    void RemoveEffect(const ItemBonusType& type, const uint64_t& causer);

    void AttachTrigger(TriggerType type,
                       const std::function<bool()>& condition,
                       const std::function<void()>& handler);

    void CheckPerish();

    bool HasEffect(const ItemBonusType& e);
    bool HasNonZeroHP();
    bool IsOnDangerousTile();
    bool IsOnTile(GameObjectType tileType);

    const std::unordered_map<uint64_t, std::vector<ItemBonusStruct>>& GetActiveEffects();
    const std::map<int, std::map<PlayerStats, int>>& GetLevelUpHistory();

    const uint64_t& ObjectId();

    uint64_t StackObjectId = 0;

    Attributes Attrs;

    int HealthRegenTurns = 0;

    int GetActionIncrement();

    GameObjectType Type = GameObjectType::HARMLESS;

    //
    // For handling of reanimation of monsters, we need to
    // check what object created current remains game object
    // in case of multiple remains on one cell.
    //
    uint64_t RemainsOf = 0;

#ifdef DEBUG_BUILD
    std::vector<std::string> DebugInfo();
    std::string HexAddressString;
#endif

    struct SaveDataMinimal
    {
      GameObjectType Type = GameObjectType::NONE;
      TransformedRoom ZoneMarker = TransformedRoom::UNMARKED;
      int Image = '?';
      int PosX  = 0;
      int PosY  = 0;
      uint32_t FgColor = Colors::None;
      uint32_t BgColor = Colors::None;
      std::string Name;
      std::string FowName;
      uint16_t Mask;

      std::string ToStringKey() const
      {
        std::stringstream ss;

        ss << (int)Type
           << (int)ZoneMarker
           << Image
           << Util::NumberToHexString(FgColor)
           << Util::NumberToHexString(BgColor)
           << Name
           << FowName
           << Mask;

        return ss.str();
      }
    };

    const SaveDataMinimal& GetSaveDataMinimal();

  protected:
    std::unordered_map<size_t, std::unique_ptr<Component>> _components;
    std::unordered_map<uint64_t, std::vector<ItemBonusStruct>> _activeEffects;

    GameObject* _previousCell = nullptr;
    GameObject* _currentCell = nullptr;

    SaveDataMinimal _sdm;

    Position _position;

    //
    // Level this object belongs to.
    // Needed for correct drawing on the screen.
    // (see comments in InventoryState::DropItem() for details)
    //
    MapLevelBase* _levelOwner = nullptr;

    int _healthRegenTurnsCounter = 0;
    int _manaRegenTurnsCounter   = 0;
    int _skipTurnsCounter        = 0;

    void MoveGameObject(int dx, int dy);
    void ProcessEffects();
    void ProcessItemsEffects();
    void ApplyEffect(const ItemBonusStruct& e);
    void UnapplyEffect(const ItemBonusStruct& e);
    void EffectAction(const ItemBonusStruct& e);
    void MarkAndCreateRemains();
    void TryToBurnItem();
    void ProcessNaturalRegenHP();
    void ProcessNaturalRegenMP();
    void ConsumeEnergy();
    void DropItemsHeld();
    void TileStandingCheck();

    bool CanRaiseAttribute(Attribute& attr);
    bool ShouldSkipTurn();
    bool IsImmune(const ItemBonusStruct& effectToAdd);

    void LevelUpFromHistory(int gainedLevel, bool positive);
    void LevelUpNatural(int gainedLevel, int baseHpOverride);

    // Unique in-game id
    uint64_t _objectId = 0;

    const std::unordered_map<ItemBonusType, Attribute&> _attributesRefsByBonus =
    {
      { ItemBonusType::STR, Attrs.Str },
      { ItemBonusType::DEF, Attrs.Def },
      { ItemBonusType::MAG, Attrs.Mag },
      { ItemBonusType::RES, Attrs.Res },
      { ItemBonusType::SKL, Attrs.Skl },
      { ItemBonusType::SPD, Attrs.Spd }
    };

    const std::unordered_map<ItemBonusType, RangedAttribute&> _rangedAttributesRefsByBonus =
    {
      { ItemBonusType::HP, Attrs.HP },
      { ItemBonusType::MP, Attrs.MP }
    };

    const std::map<int, std::pair<PlayerStats, Attribute&>> _mainAttributesByIndex =
    {
      { 0, { PlayerStats::STR, Attrs.Str } },
      { 1, { PlayerStats::DEF, Attrs.Def } },
      { 2, { PlayerStats::MAG, Attrs.Mag } },
      { 3, { PlayerStats::RES, Attrs.Res } },
      { 4, { PlayerStats::SKL, Attrs.Skl } },
      { 5, { PlayerStats::SPD, Attrs.Spd } }
    };

    const std::map<int, Attribute&> _weaknessPenaltyStats =
    {
      { 0, Attrs.Str },
      { 1, Attrs.Def },
      { 2, Attrs.Skl },
      { 3, Attrs.Spd }
    };

    //
    // Stats increases by gained level
    //
    std::map<int, std::map<PlayerStats, int>> _levelUpHistory =
    {
      {
        1,
        {
          { PlayerStats::STR, 0 },
          { PlayerStats::DEF, 0 },
          { PlayerStats::MAG, 0 },
          { PlayerStats::RES, 0 },
          { PlayerStats::SKL, 0 },
          { PlayerStats::SPD, 0 },
          { PlayerStats::HP,  0 },
          { PlayerStats::MP,  0 }
        }
      }
    };

    friend class GameObjectsFactory;

#ifdef DEBUG_BUILD
    friend class DevConsole;
#endif
};

#ifdef DEBUG_BUILD
extern std::unordered_map<uint64_t, GameObject*> GameObjectsById;
#endif

#endif
