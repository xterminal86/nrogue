#include "behaviour-tree.h"

#include "game-object.h"
#include "application.h"
#include "util.h"

//
// ***** Node *****
//
Node::Node(GameObject* objectToControl)
{
  _playerRef = &Application::Instance().PlayerInstance;

  if (objectToControl != nullptr)
  {
    _objectToControl = objectToControl;
  }
}

Node::~Node()
{
  //DebugLog("0x%X dtor\n", this);
}

void Node::FirstRun()
{
  if (!_firstRunFlag)
  {
    FirstRunSpecific();
    _firstRunFlag = true;
  }
}

void Node::FirstRunSpecific()
{
}

void Node::Reset()
{
  ResetSpecific();
}

void Node::ResetSpecific()
{
}

void Node::AddNode(Node* node)
{
}

std::string Node::ToString()
{
  std::string res = Util::Instance().StringFormat("[%s]", typeid(*this).name());
  return res;
}
//
// ********************
//

//
// ***** Root *****
//
BTResult Root::Run()
{
  if (_root)
  {
    _root->FirstRun();

    return _root->Run();
  }

  return BTResult::Success;
}

void Root::ResetSpecific()
{
  if (_root)
  {
    _root->Reset();
  }
}

void Root::AddNode(Node* node)
{
  Set(node);
}

void Root::Set(Node* node)
{
  if (node != nullptr)
  {
    _root.reset(node);
  }
}

//
// ********************
//

//
// ***** Condition *****
//
Condition::Condition(GameObject* objToControl,
                     const std::function<BTResult()>& fn)
  : ControlNode(objToControl)
{
  _fn = fn;
}

void Condition::AddNode(Node* node)
{
  if (node != nullptr)
  {
    if (_children.empty())
    {
      _children.push_back(std::unique_ptr<Node>(node));
    }
    else
    {
      _children[0].reset(node);
    }
  }
}

BTResult Condition::Run()
{
  BTResult res = BTResult::Undefined;

  if (Util::Instance().IsFunctionValid(_fn))
  {
    res = _fn();

    if (res == BTResult::Success)
    {
      if (!_children.empty())
      {
        _children[0]->FirstRun();

        res = _children[0]->Run();
      }
      else
      {
        res = BTResult::Undefined;
      }
    }
  }

  return res;
}

void Condition::ResetSpecific()
{
  if (!_children.empty())
  {
    _children[0]->Reset();
  }
}
//
// ********************
//

//
// ***** ControlNode *****
//
void ControlNode::AddNode(Node* node)
{
  if (node != nullptr)
  {
    _children.push_back(std::unique_ptr<Node>(node));
  }
}

void ControlNode::ResetSpecific()
{
  for (auto& i : _children)
  {
    i->Reset();
  }
}

const std::vector<std::unique_ptr<Node>>& ControlNode::GetChildren()
{
  return _children;
}

//
// ********************
//

//
// ***** Failure *****
//
BTResult Failure::Run()
{
  return BTResult::Failure;
}
//
// ********************
//


//
// ***** IgnoreFailure *****
//
BTResult IgnoreFailure::Run()
{
  for (auto& i : _children)
  {
    i->FirstRun();

    auto status = i->Run();

    if (status == BTResult::Failure)
    {
      status = BTResult::Success;
    }

    return status;
  }

  return BTResult::Success;
}
//
// ********************
//

//
// ***** Sequence *****
//
BTResult Sequence::Run()
{
  for (auto& i : _children)
  {
    i->FirstRun();

    auto status = i->Run();

    if (status != BTResult::Success)
    {
      return status;
    }
  }

  return BTResult::Success;
}
//
// ********************
//

//
// ***** Selector *****
//
BTResult Selector::Run()
{
  for (auto& i : _children)
  {
    i->FirstRun();

    auto status = i->Run();

    if (status != BTResult::Failure)
    {
      return status;
    }
  }

  return BTResult::Failure;
}
//
// ********************
//

//
// ***** Repeater *****
//
Repeater::Repeater(GameObject* objectToControl, int toRepeat) : ControlNode(objectToControl)
{
  _toRepeat = toRepeat;
  _repeatCount = 0;
}

BTResult Repeater::Run()
{
  if (_children.size() == 1)
  {
    _children[0]->FirstRun();

    _children[0]->Run();

    if (_toRepeat > 0)
    {
      _repeatCount++;

      if (_repeatCount > _toRepeat)
      {
        return BTResult::Success;
      }
    }
  }

  return BTResult::Running;
}

void Repeater::ResetSpecific()
{
  _repeatCount = 0;
}
