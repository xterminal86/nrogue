#include "blackboard.h"

void Blackboard::InitSpecific()
{
}

// =============================================================================

void Blackboard::Set(uint64_t goId, const SSPair& data)
{
  auto& mapVal = _blackboard[goId];
  mapVal[data.first] = data.second;
}

// =============================================================================

std::string Blackboard::Get(uint64_t goId, const std::string& key)
{
  auto& mapVal = _blackboard[goId];
  return mapVal[key];
}

// =============================================================================

void Blackboard::Remove(uint64_t goId)
{
  _blackboard.erase(goId);
}
