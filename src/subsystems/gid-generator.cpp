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

void GID::Init()
{
  if (_initialized)
  {
    return;
  }

  _globalId = kStartingValue;

  _initialized = true;
}
