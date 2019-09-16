#ifndef BEHAVIOURTREE_H
#define BEHAVIOURTREE_H

#include <memory>
#include <vector>

#include "application.h"
#include "util.h"

class GameObject;

enum class BTResult
{
  Success = 0,
  Running,
  Failure
};

class Node
{
  public:
    Node(GameObject* objectToControl)
    {
      if (objectToControl != nullptr)
      {
        _objectToControl = objectToControl;
      }

      _playerRef = &Application::Instance().PlayerInstance;
    }

    virtual ~Node() = default;
    virtual bool Run() = 0;

  protected:
    GameObject* _objectToControl = nullptr;
    Player* _playerRef = nullptr;
};

class Root : public Node
{
  using Node::Node;

  public:
    bool Run() override
    {
      if (_root)
      {
        _root->Run();
      }

      return true;
    }

    void SetNode(Node* node)
    {
      if (node != nullptr)
      {
        _root.reset(node);
      }
    }

  private:
    std::unique_ptr<Node> _root;
};

class ControlNode : public Node
{
  using Node::Node;

  public:
    void AddNode(Node* node)
    {
      if (node != nullptr)
      {
        _children.push_back(std::unique_ptr<Node>(node));
      }      
    }

  protected:
    std::vector<std::unique_ptr<Node>> _children;
};

class Sequence : public ControlNode
{
  using ControlNode::ControlNode;

  public:
    bool Run() override
    {
      for (auto& i : _children)
      {
        if (!i->Run())
        {
          return false;
        }
      }

      return true;
    }
};

class Selector : public ControlNode
{
  using ControlNode::ControlNode;

  public:
    bool Run() override
    {
      for (auto& i : _children)
      {
        if (i->Run())
        {
          return true;
        }
      }

      return false;
    }
};

class ProbabilisticNode : public Node
{
  public:
    ProbabilisticNode(GameObject* objectToControl, int successChance)
      : Node(objectToControl)
    {
      _successChance = successChance;
    }

    bool Run() override
    {
      return Util::Rolld100(_successChance);
    }

  private:
    int _successChance = 100;
};

#endif
