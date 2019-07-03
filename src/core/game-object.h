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
#include "effect.h"

class GameObjectInfo;
class MapLevelBase;
class Position;

class GameObject
{
  public:    
    GameObject(MapLevelBase* levelOwner = nullptr);
    GameObject(GameObject&) = delete;
    virtual ~GameObject() = default;

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
    bool MoveTo(int x, int y);
    bool MoveTo(const Position& pos);

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

    void MakeTile(const GameObjectInfo& t);

    void Update();

    Attribute VisibilityRadius;

    int PosX = 0;
    int PosY = 0;

    // e.g., to disallow walking into a tile for NPC
    bool Special = false;

    bool Blocking = false;
    bool BlocksSight = false;
    bool Revealed = false;

    // Determines if objects on map tile should be drawn
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

    void ReceiveDamage(GameObject* from, int amount, bool isMagical, const std::string& logMsgOverride = std::string());
    void FinishTurn();
    void WaitForTurn();

    virtual void LevelUp(int baseHpOverride = -1);

    void AddEffect(const Effect& effectToAdd);
    void AddEffect(EffectType type, int power, int duration, bool cumulative, bool ignoresArmor);
    void RemoveEffect(EffectType t);    
    bool HasEffect(EffectType t);
    bool IsAlive();

    const std::map<EffectType, Effect>& Effects();

    Attributes Attrs;        

    int HealthRegenTurns = 0;

    GameObjectType Type = GameObjectType::HARMLESS;

  protected:    
    std::map<size_t, std::unique_ptr<Component>> _components;
    std::map<EffectType, Effect> _activeEffects;

    GameObject* _previousCell = nullptr;
    GameObject* _currentCell = nullptr;

    // Level this object belongs to
    MapLevelBase* _levelOwner = nullptr;    

    int _healthRegenTurnsCounter = 0;    

    void MoveGameObject(int dx, int dy);
    void ProcessEffects();
    void ApplyEffect(const Effect& e);
    void UnapplyEffect(const Effect& e);
    void EffectAction(const Effect& e);
    void MarkAndCreateRemains();

    bool CanRaiseAttribute(Attribute& attr);
};

#endif
