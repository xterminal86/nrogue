#include "gid-generator.h"

// =============================================================================

const uint64_t& GID::GenerateGlobalId()
{
  _globalId++;
  return _globalId;
}

// =============================================================================

const uint64_t& GID::GetCurrentGlobalId()
{
  return _globalId;
}

// =============================================================================

void GID::InitSpecific()
{
}

// =============================================================================

void GID::Reset()
{
  _globalId = kStartingValue;
}
