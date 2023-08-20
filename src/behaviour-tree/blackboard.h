#ifndef BLACKBOARD_H
#define BLACKBOARD_H

#include <string>
#include <unordered_map>

#ifdef __MINGW64__
#include <stdint.h>
#endif

#include "singleton.h"

class GameObject;

using SSMap = std::unordered_map<std::string, std::string>;
using SSPair = std::pair<std::string, std::string>;

///
/// Helper class for global data access
/// and manipulation for AI controlled objects
///
class Blackboard : public Singleton<Blackboard>
{
  public:
    void Set(uint64_t goId, const SSPair& data);
    std::string Get(uint64_t goId, const std::string& key);
    void Remove(uint64_t goId);

  protected:
    void InitSpecific() override;

  private:
    std::unordered_map<uint64_t, SSMap> _blackboard;
};

#endif // BLACKBOARD_H
