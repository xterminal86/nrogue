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
#include "attribute.h"
#include "item-data.h"

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
               const std::string& bgColor = "#000000");

    void SetLevelOwner(MapLevelBase* levelOwner);

    bool Interact();

    void Init(MapLevelBase* levelOwner,
              int x, int y,
              int avatar,
              const std::string& fgColor,
              const std::string& bgColor = "#000000");

    /// Adds \p dx and \p dy to corresponding game object's coordinates.
    /// Use to move actors only.
    bool Move(int dx, int dy);

    /// Sets game object's coordinates to \p x and \p y.
    /// Use to move actors only.
    bool MoveTo(int x, int y, bool force = false);
    bool MoveTo(const Position& pos, bool force = false);

    void Draw(const std::string& overrideFgColor = std::string(),
              const std::string& overrideBgColor = std::string());

    template <typename T>
    inline T* AddComponent()
    {
      // NOTE: No check for component already added.
      // Do we need one?

      std::unique_ptr<T> cp = std::make_unique<T>();

      cp.get()->OwnerGameObject = this;

      // cp is null after std::move
      _components[typeid(T).hash_code()] = std::move(cp);

      auto pointer = _components[typeid(T).hash_code()].get();

      return static_cast<T*>(pointer);
    }

    template <typename T>
    inline T* GetComponent()
    {
      for (auto& c : _components)
      {
        if (c.second.get()->GetComponentHash() == typeid(T).hash_code())
        {
          auto pointer = c.second.get();
          return static_cast<T*>(pointer);
        }
      }

      return nullptr;
    }

    void MakeTile(const GameObjectInfo& t, GameObjectType typeOverride = GameObjectType::GROUND);

    void Update();

    Attribute VisibilityRadius;

    int PosX = 0;
    int PosY = 0;

    // e.g. to disallow walking into a tile for NPC
    bool Special = false;

    bool Blocking = false;
    bool BlocksSight = false;
    bool Revealed = false;

    // Determines if objects on the map tile should be drawn
    bool Visible = false;

    // NOTE: set manually for NPCs during creation via GameObject::Move(),
    // because some game objects can be stepped on (e.g. bench, money etc)
    bool Occupied = false;

    bool IsDestroyed = false;

    int Image;
    std::string FgColor = GlobalConstants::BlackColor;
    std::string BgColor = GlobalConstants::BlackColor;
    std::string ObjectName;
    std::string FogOfWarName;

    std::function<void()> InteractionCallback;

    size_t ComponentsSize();

    bool ReceiveDamage(GameObject* from, int amount, bool isMagical, const std::string& logMsgOverride = std::string());
    bool CanMove();

    void FinishTurn();
    void WaitForTurn();

    virtual void LevelUp(int baseHpOverride = -1);

    void AddEffect(const ItemBonusStruct& effectToAdd);

    // Erases only effects that have ItemRef = nullptr
    void DispelEffect(const ItemBonusType& e);

    void RemoveEffect(const ItemBonusStruct& e);

    bool HasEffect(const ItemBonusType& e);
    bool IsAlive();

    const std::map<uint64_t, std::vector<ItemBonusStruct>>& Effects();

    uint64_t ObjectId();

    Attributes Attrs;

    int HealthRegenTurns = 0;

    int GetActionIncrement();

    GameObjectType Type = GameObjectType::HARMLESS;
    bool IsLiving = false;

  protected:
    std::map<size_t, std::unique_ptr<Component>> _components;
    std::map<uint64_t, std::vector<ItemBonusStruct>> _activeEffects;

    GameObject* _previousCell = nullptr;
    GameObject* _currentCell = nullptr;

    // Level this object belongs to.
    // Needed for correct drawing on the screen.
    // (see comments in InventoryState::DropItem() for details)
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

    bool CanRaiseAttribute(Attribute& attr);
    bool ShouldSkipTurn();

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
};

#endif
