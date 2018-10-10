#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <string>
#include <map>
#include <memory>
#include <functional>

#include <ncurses.h>

#include "component.h"

class GameObject
{
  public:
    GameObject() = default;
    GameObject(GameObject&) = delete;
    virtual ~GameObject() = default;

    GameObject(int x, int y, chtype avatar, const std::string& htmlColor)
    {
      Init(x, y, avatar, htmlColor);
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

    void Init(int x, int y, chtype avatar, const std::string& htmlColor);

    bool Move(int dx, int dy);

    void Draw(const std::string& overrideColor = std::string());

    template <typename T>
    inline Component* AddComponent()
    {
      // No check for component already added. Do we need one?

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

    void CreateWall();
    void CreateFloor();
    void CreateTree();
    void CreateMountain();

    void Update();

    int PosX;
    int PosY;

    bool Blocking = false;
    bool BlockSight = true;
    bool Revealed = false;
    bool Visible = false;
    bool Occupied = false;

    chtype Image;
    std::string HtmlColor;
    std::string ObjectName;

    std::function<void()> InteractionCallback;

  private:

    std::map<size_t, std::unique_ptr<Component>> _components;

    GameObject* _previousCell = nullptr;
    GameObject* _currentCell = nullptr;    
};

#endif
