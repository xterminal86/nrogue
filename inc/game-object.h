#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <string>
#include <map>
#include <memory>
#include <functional>

#include <ncurses.h>

#include "component.h"
#include "constants.h"

/// Helper struct to reduce the writing when creating objects
struct Tile
{
  void Set(bool isBlocking,
           bool blocksSight,
           chtype image,
           const std::string& fgColor,
           const std::string& bgColor,
           const std::string& objectName,
           const std::string& fowName = "")
  {
    IsBlocking = isBlocking;
    BlocksSight = blocksSight;
    Image = image;
    FgColor = fgColor;
    BgColor = bgColor;
    ObjectName = objectName;
    FogOfWarName = fowName;
  }

  bool IsBlocking;
  bool BlocksSight;
  chtype Image;

  // set this to "" if you want only background color
  std::string FgColor;

  std::string BgColor;

  std::string ObjectName;
  std::string FogOfWarName;
};

class GameObject
{
  public:
    GameObject() {}
    GameObject(GameObject&) = delete;
    virtual ~GameObject() = default;

    GameObject(int x, int y, chtype avatar, const std::string& htmlColor, const std::string& bgColor = "#000000")
    {      
      Init(x, y, avatar, htmlColor, bgColor);
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

    void Init(int x, int y, chtype avatar, const std::string& fgColor, const std::string& bgColor = "#000000");

    bool Move(int dx, int dy);
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

    Attributes Attrs;

    std::vector<Attribute*> MainAttributes =
    {
      &Attrs.Str,
      &Attrs.Def,
      &Attrs.Mag,
      &Attrs.Res,
      &Attrs.Skl,
      &Attrs.Spd
    };

  private:

    std::map<size_t, std::unique_ptr<Component>> _components;

    GameObject* _previousCell = nullptr;
    GameObject* _currentCell = nullptr;        
};

#endif
