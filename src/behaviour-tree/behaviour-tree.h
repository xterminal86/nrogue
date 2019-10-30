#ifndef BEHAVIOURTREE_H
#define BEHAVIOURTREE_H

#include <memory>
#include <vector>
#include <map>
#include <functional>

#include "util.h"

class GameObject;
class Player;

enum class BTResult
{
  Success = 0,
  Running,
  Failure,
  Undefined
};

class Node
{
  public:
    Node(GameObject* objectToControl);
    virtual ~Node();

    virtual void AddNode(Node* node);

    virtual BTResult Run() = 0;

    virtual std::string ToString();

  protected:    
    void FirstRun();    
    void Reset();

    virtual void FirstRunSpecific();
    virtual void ResetSpecific();

    GameObject* _objectToControl = nullptr;

    bool _firstRunFlag = false;

    Player* _playerRef;

    // FirstRun and Reset shouldn't be allowed to call
    // directly on task objects.
    // They should be visible only to
    // fundamental behaviour tree nodes only.

    friend class Root;
    friend class ControlNode;
    friend class IgnoreFailure;
    friend class Sequence;
    friend class Selector;
    friend class Repeater;
    friend class Condition;
};

class ControlNode : public Node
{
  using Node::Node;

  public:
    std::string ToString() override;

    void AddNode(Node* node) override;

  protected:
    void ResetSpecific() override;

    std::vector<std::unique_ptr<Node>> _children;    
};

class Root : public Node
{
  using Node::Node;

  public:
    BTResult Run() override;

    void AddNode(Node* node) override;

    std::string ToString() override;

  protected:
    void Set(Node* node);

    void ResetSpecific() override;

    std::unique_ptr<Node> _root;

    friend class AIModelBase;
};

class Condition : public ControlNode
{
  using ControlNode::ControlNode;

  public:
    Condition(GameObject* objToControl, const std::function<BTResult()>& fn);

    void AddNode(Node* node) override;

    BTResult Run() override;

    std::string ToString() override;

  protected:
    void ResetSpecific() override;

    std::function<BTResult()> _fn;
};

///
/// Can be used as an "end state" node - if root node receives
/// BTResult::Failure, Reset() gets called.
///
class Failure : public Node
{
  using Node::Node;

  public:
    BTResult Run() override;

    std::string ToString() override;    
};

class IgnoreFailure : public ControlNode
{
  using ControlNode::ControlNode;

  public:
    BTResult Run() override;

    std::string ToString() override;
};

///
/// A sequence runs each task in order until one fails,
/// at which point it returns FAILURE. If all tasks succeed, a SUCCESS
/// status is returned.  If a subtask is still RUNNING, then a RUNNING
/// status is returned and processing continues until either SUCCESS
/// or FAILURE is returned from the subtask.
///
class Sequence : public ControlNode
{
  using ControlNode::ControlNode;

  public:
    BTResult Run() override;

    std::string ToString() override;    
};

///
/// A selector runs each task in order until one succeeds,
/// at which point it returns SUCCESS. If all tasks fail, a FAILURE
/// status is returned.  If a subtask is still RUNNING, then a RUNNING
/// status is returned and processing continues until either SUCCESS
/// or FAILURE is returned from the subtask.
///
class Selector : public ControlNode
{
  using ControlNode::ControlNode;

  public:
    BTResult Run() override;

    std::string ToString() override;    
};

class Repeater : public ControlNode
{
  public:
    Repeater(GameObject* objectToControl, int toRepeat = -1);

    BTResult Run() override;

    std::string ToString() override;

  private:
    int _toRepeat = 0;
    int _repeatCount = 0;
};

#endif
