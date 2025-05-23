#ifndef BEHAVIOURTREE_H
#define BEHAVIOURTREE_H

#include <memory>
#include <vector>
#include <map>
#include <functional>
#include <string>

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

///
/// Base class for behaviour tree logic.
///
/// [TASK p1="<TASK_TYPE>"] (behaviour is implemented inside specific classes)
///
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

    Player* _playerRef = nullptr;

    // FirstRun and Reset shouldn't be allowed to be called
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

///
/// Base class for fundamental control nodes (selector and sequence).
///
class ControlNode : public Node
{
  using Node::Node;

  public:
    void AddNode(Node* node) override;

    // For allowing debug print of the behaviour tree
    const std::vector<std::unique_ptr<Node>>& GetChildren();

  protected:
    void ResetSpecific() override;

    std::vector<std::unique_ptr<Node>> _children;
};

///
/// Starting node of the behaviour tree.
///
/// [TREE]
///
class Root : public Node
{
  using Node::Node;

  public:
    BTResult Run() override;

    void AddNode(Node* node) override;

  protected:
    void Set(Node* node);

    void ResetSpecific() override;

    std::unique_ptr<Node> _root;

    friend class AIModelBase;
};

///
/// Calls underlying condition function and returns
/// result of the child node if condition function is a success,
/// BTResult::Failure otherwise.
///
/// If no condition function is specified,
/// the result is BTResult::Undefined.
///
/// [COND p1="..." p2="..." ...]
///
class Condition : public ControlNode
{
  using ControlNode::ControlNode;

  public:
    Condition(GameObject* objToControl, const std::function<BTResult()>& fn);

    void AddNode(Node* node) override;

    BTResult Run() override;

  protected:
    void ResetSpecific() override;

    std::function<BTResult()> _fn;
};

///
/// Always returns BTResult::Failure.
///
/// Can be used as an "end state" node -
/// e.g. if root node receives BTResult::Failure,
/// Reset() gets called which calls Reset()
/// on the whole tree nodes.
///
/// You can override reset behaviour for tasks
/// by overriding ResetSpecific() method,
/// as well as "call once" functionality by overriding
/// FirstRunSpecific().
/// Don't forget to reset _firstRunFlag as well if you
/// need to repeat FirstRun() after Reset().
///
/// [FAIL]
///
class Failure : public Node
{
  using Node::Node;

  public:
    BTResult Run() override;
};

///
/// If child node returns BTResult::Failure,
/// returns BTResult::Success.
///
/// [SUCC]
///
class IgnoreFailure : public ControlNode
{
  using ControlNode::ControlNode;

  public:
    BTResult Run() override;
};

///
/// A sequence runs each task in order until one fails,
/// at which point it returns FAILURE. If all tasks succeed, a SUCCESS
/// status is returned.  If a subtask is still RUNNING, then a RUNNING
/// status is returned and processing continues until either SUCCESS
/// or FAILURE is returned from the subtask.
///
/// [SEQ]
///
class Sequence : public ControlNode
{
  using ControlNode::ControlNode;

  public:
    BTResult Run() override;
};

///
/// A selector runs each task in order until one succeeds,
/// at which point it returns SUCCESS. If all tasks fail, a FAILURE
/// status is returned.  If a subtask is still RUNNING, then a RUNNING
/// status is returned and processing continues until either SUCCESS
/// or FAILURE is returned from the subtask.
///
/// [SEL]
///
class Selector : public ControlNode
{
  using ControlNode::ControlNode;

  public:
    BTResult Run() override;
};

///
/// Executes child node 'toRepeat' number of times
/// or inifinitely if 'toRepeat' is -1.
///
class Repeater : public ControlNode
{
  public:
    Repeater(GameObject* objectToControl, int toRepeat = -1);

    BTResult Run() override;

  protected:
    void ResetSpecific() override;

  private:
    int _toRepeat    = 0;
    int _repeatCount = 0;
};

#endif
