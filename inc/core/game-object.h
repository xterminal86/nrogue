#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <string>
#include <map>
#include <memory>
#include <functional>

#include <ncurses.h>

#include "component.h"
#include "constants.h"

class MapLevelBase;

class GameObject
{
  public:    
    GameObject(MapLevelBase* levelOwner = nullptr);
    GameObject(GameObject&) = delete;
    virtual ~GameObject() = default;

    GameObject(MapLevelBase* levelOwner, int x, int y, chtype avatar, const std::string& htmlColor, const std::string& bgColor = "#000000")
    {      
      Init(levelOwner, x, y, avatar, htmlColor, bgColor);
    }

    void SetLevelOwner(MapLevelBase* levelOwner)
    {
      _levelOwner = levelOwner;
    }

    bool Interact()
    {
      // http://www.cplusplus.com/reference/functional/function/target_type/
      //
      // Return value
      // The type_info object that corresponds to the type of the target,
      // or typeid(void) if the object is an empty function.

      if (InteractionCallback.target_type() != typeid(void))
      {
        InteractionCallback();

        return true;
      }

      return false;
    }

    void Init(MapLevelBase* levelOwner, int x, int y, chtype avatar, const std::string& fgColor, const std::string& bgColor = "#000000");

    /// Adds \p dx and \p dy to corresponding game object's coordinates.
    /// Use to move actors only.
    bool Move(int dx, int dy);

    /// Sets game object's coordinates to \p x and \p y.
    /// Use to move actors only.
    bool MoveTo(int x, int y);

    void Draw(const std::string& overrideFgColor = std::string(), const std::string& overrideBgColor = std::string());

    template <typename T>
    inline Component* AddComponent()
    {
      // NOTE: No check for component already added.
      // Do we need one?

      auto cp = std::make_unique<T>();

      cp.get()->OwnerGameObject = this;

      // cp is null after std::move
      _components[typeid(T).hash_code()] = std::move(cp);

      return _components[typeid(T).hash_code()].get();
    }

    template <typename T>
    inline Component* GetComponent()
    {
      for (auto& c : _components)
      {
        if (c.second.get()->Hash() == typeid(T).hash_code())
        {
          return c.second.get();
        }
      }

      return nullptr;
    }

    void MakeTile(Tile t);

    void Update();

    int PosX;
    int PosY;

    bool Blocking = false;
    bool BlocksSight = false;
    bool Revealed = false;

    // Determines if objects on map tile should be drawn
    bool Visible = false;

    // NOTE: set manually for NPCs during creation via GameObject::Move(),
    // because some game objects can be stepped on (e.g. bench, money etc)
    bool Occupied = false;

    bool IsDestroyed = false;

    chtype Image;
    std::string FgColor = GlobalConstants::BlackColor;
    std::string BgColor = GlobalConstants::BlackColor;
    std::string ObjectName;
    std::string FogOfWarName;

    std::function<void()> InteractionCallback;

    size_t ComponentsSize()
    {
      return _components.size();
    }

    void ReceiveDamage(GameObject* from, int amount);
    void FinishTurn();
    void WaitForTurn();

    Attributes Attrs;        

    int HealthRegenTurns = 0;

  protected:
    std::map<size_t, std::unique_ptr<Component>> _components;

    GameObject* _previousCell = nullptr;
    GameObject* _currentCell = nullptr;

    // Level this object belongs to
    MapLevelBase* _levelOwner = nullptr;    

    int _healthRegenTurnsCounter = 0;    
};

#endif
