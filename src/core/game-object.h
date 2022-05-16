#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <string>
#include <map>
#include <memory>
#include <functional>

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

class GameObject
{
  public:
    GameObject(MapLevelBase* levelOwner = nullptr);
    GameObject(GameObject&) = delete;
    virtual ~GameObject();

    GameObject(MapLevelBase* levelOwner,
               int x, int y,
               int avatar,
               const std::string& htmlColor,
               const std::string& bgColor = Colors::BlackColor);

    void SetLevelOwner(MapLevelBase* levelOwner);

    IR Interact();

    void Init(MapLevelBase* levelOwner,
              int x, int y,
              int avatar,
              const std::string& fgColor,
              const std::string& bgColor = Colors::BlackColor);

    /// Adds \p dx and \p dy to corresponding game object's coordinates.
    /// Use to move actors only.
    bool Move(int dx, int dy);

    /// Sets game object's coordinates to \p x and \p y.
    /// Use to move actors only.
    bool MoveTo(int x, int y, bool force = false);
    bool MoveTo(const Position& pos, bool force = false);

    bool CanMoveTo(const Position& pos);

    void Draw(const std::string& overrideFgColor = std::string(),
              const std::string& overrideBgColor = std::string());

    template <typename T, typename ... Args>
    inline T* AddComponent(Args ... args)
    {
      if (_components.count(typeid(T).hash_code()) == 1)
      {
        DebugLog("[WAR] trying to add existing component %s on game object [0x%X] - returning existing 0x%X",
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
        if (c.second.get() != nullptr
         && c.second.get()->GetComponentHash() == typeid(T).hash_code())
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

    void MakeTile(const GameObjectInfo& t, GameObjectType typeOverride = GameObjectType::GROUND);

    void Update();

    void ApplyBonuses(ItemComponent* itemRef);
    void UnapplyBonuses(ItemComponent* itemRef);

    void ApplyBonus(ItemComponent* itemRef, const ItemBonusStruct& bonus);
    void UnapplyBonus(ItemComponent* itemRef, const ItemBonusStruct& bonus);

    const Position& GetPosition();

    Attribute VisibilityRadius;

    int Money = 0;

    int PosX = 0;
    int PosY = 0;

    // e.g. to disallow walking into a tile for NPC
    bool Special = false;

    bool Blocking    = false;
    bool BlocksSight = false;
    bool Revealed    = false;

    bool Corporeal = true;

    // Determines if objects on the map tile should be drawn
    bool Visible = false;

    // NOTE: set manually for NPCs during creation via GameObject::Move(),
    // because some game objects can be stepped on (e.g. bench, money etc)
    bool Occupied = false;

    bool IsDestroyed = false;

    int Image;
    std::string FgColor = Colors::BlackColor;
    std::string BgColor = Colors::BlackColor;
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

    // Erases only effects that have ItemRef = nullptr
    void DispelEffect(const ItemBonusType& type);

    void RemoveEffect(const ItemBonusStruct& e);

    void RemoveEffectFirstFound(const ItemBonusType& type);
    void RemoveEffectAllOf(const ItemBonusType& type);

    void AttachTrigger(TriggerType type,
                       const std::function<bool()>& condition,
                       const std::function<void()>& handler);

    void CheckPerish();

    bool HasEffect(const ItemBonusType& e);
    bool IsAlive();
    bool IsOnDangerousTile();
    bool IsOnTile(GameObjectType tileType);

    const std::map<uint64_t, std::vector<ItemBonusStruct>>& GetActiveEffects();

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

    bool IsLiving = false;

#ifdef DEBUG_BUILD
    std::vector<std::string> DebugInfo();
    std::string HexAddressString;
#endif

  protected:
    std::map<size_t, std::unique_ptr<Component>> _components;
    std::map<uint64_t, std::vector<ItemBonusStruct>> _activeEffects;

    GameObject* _previousCell = nullptr;
    GameObject* _currentCell = nullptr;

    Position _position;

    //
    // Level this object belongs to.
    // Needed for correct drawing on the screen.
    // (see comments in InventoryState::DropItem() for details)
    //
    MapLevelBase* _levelOwner = nullptr;

    int _healthRegenTurnsCounter = 0;
    int _manaRegenTurnsCounter = 0;
    int _skipTurnsCounter = 0;

    void MoveGameObject(int dx, int dy);
    void ProcessEffects();
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

    // Unique in-game id
    uint64_t _objectId = 1;

    std::map<ItemBonusType, Attribute&> _attributesRefsByBonus =
    {
      { ItemBonusType::STR, Attrs.Str },
      { ItemBonusType::DEF, Attrs.Def },
      { ItemBonusType::MAG, Attrs.Mag },
      { ItemBonusType::RES, Attrs.Res },
      { ItemBonusType::SPD, Attrs.Spd },
      { ItemBonusType::SKL, Attrs.Skl }
    };

    std::map<ItemBonusType, RangedAttribute&> _rangedAttributesRefsByBonus =
    {
      { ItemBonusType::HP, Attrs.HP },
      { ItemBonusType::MP, Attrs.MP }
    };

    friend class GameObjectsFactory;

#ifdef DEBUG_BUILD
    friend class DevConsole;
#endif
};

#endif
