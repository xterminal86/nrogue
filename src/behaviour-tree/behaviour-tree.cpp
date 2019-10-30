#include "behaviour-tree.h"
#include "game-object.h"

//
// ***** Node *****
//
Node::Node(GameObject* objectToControl)
{
  if (objectToControl != nullptr)
  {
    _objectToControl = objectToControl;
  }
}

Node::~Node()
{
  //printf("0x%X dtor\n", this);
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
  std::string res = Util::StringFormat("[NodeBase]");
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

std::string Root::ToString()
{
  std::string res = Util::StringFormat("[RootNode]");
  return res;
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

  if (_fn.target_type() != typeid(void))
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

std::string Condition::ToString()
{
  std::string res = Util::StringFormat("[Condition]");
  return res;
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

std::string ControlNode::ToString()
{
  std::string res = Util::StringFormat("[ControlNode]");
  return res;
}
//
// ********************
//

//
// ***** Failure *****
//
BTResult Failure::Run()
{
  _objectToControl->CurrentlyExecutingNode = this;

  FirstRun();

  return BTResult::Failure;
}

std::string Failure::ToString()
{
  std::string res = Util::StringFormat("[Failure]");
  return res;
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

std::string IgnoreFailure::ToString()
{
  std::string res = Util::StringFormat("[IgnoreFailure]");
  return res;
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

std::string Sequence::ToString()
{
  std::string res = Util::StringFormat("[Sequence]");
  return res;
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

std::string Selector::ToString()
{
  std::string res = Util::StringFormat("[Selector]");
  return res;
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

    auto res = _children[0]->Run();

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

std::string Repeater::ToString()
{
  std::string res = Util::StringFormat("[Repeater]");
  return res;
}
