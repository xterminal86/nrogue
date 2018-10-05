#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <string>
#include <map>
#include <memory>

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

    void Init(int x, int y, chtype avatar, const std::string& htmlColor);

    bool Move(int dx, int dy);

    void Draw(const std::string& overrideColor = std::string());

    template <typename T>
    inline Component* AddComponent()
    {
      // TODO: no check for component already added

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

    void SetWall();
    void SetFloor();

    void Update();

    int PosX;
    int PosY;

    bool Blocking;
    bool BlockSight;
    bool Revealed;
    bool Visible = false;

    chtype Image;
    std::string HtmlColor;

  private:
    std::map<size_t, std::unique_ptr<Component>> _components;

    GameObject* _previousCell = nullptr;
    GameObject* _currentCell = nullptr;
};

#endif
