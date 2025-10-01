#ifndef GLOBALS_H
#define GLOBALS_H

// =============================================================================
#ifdef DEBUG_BUILD
#include <unordered_map>
#include <string>
#include <cstdint>

class GameObject;

extern std::unordered_map<std::string, void*> AnyObjectByAddr;
extern std::unordered_map<uint64_t, GameObject*> GameObjectsById;
#endif
// =============================================================================

#endif // GLOBALS_H
