#include "globals.h"

// =============================================================================
#ifdef DEBUG_BUILD
#include "game-object.h"

std::unordered_map<std::string, void*> AnyObjectByAddr;
std::unordered_map<uint64_t, GameObject*> GameObjectsById;
#endif
// =============================================================================
