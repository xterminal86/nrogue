#ifndef BLACKBOARD_H
#define BLACKBOARD_H

#include <string>
#include <unordered_map>

#include "singleton.h"

class GameObject;

using ssmap = std::unordered_map<std::string, std::string>;
using sspair = std::pair<std::string, std::string>;

///
/// Helper class for global data access
/// and manipulation for AI controlled objects
///
class Blackboard : public Singleton<Blackboard>
{
  public:
    void Init() override;

    void Set(uint64_t goId, const sspair& data);
    std::string Get(uint64_t goId, const std::string& key);
    void Remove(uint64_t goId);

  private:
    std::unordered_map<uint64_t, ssmap> _blackboard;
};

#endif // BLACKBOARD_H
