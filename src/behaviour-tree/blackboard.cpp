#include "blackboard.h"

void Blackboard::InitSpecific()
{
}

void Blackboard::Set(uint64_t goId, const SSPair& data)
{
  if (_blackboard.count(goId) == 0)
  {
    SSMap placeholder;
    _blackboard.emplace(goId, placeholder);
  }

  auto& mapVal = _blackboard[goId];

  mapVal[data.first] = data.second;
}

std::string Blackboard::Get(uint64_t goId, const std::string& key)
{
  std::string res;

  if (_blackboard.count(goId) == 1)
  {
    auto& mapVal = _blackboard[goId];

    if (mapVal.count(key) == 1)
    {
      res = mapVal[key];
    }
  }

  return res;
}

void Blackboard::Remove(uint64_t goId)
{
  _blackboard.erase(goId);
}
