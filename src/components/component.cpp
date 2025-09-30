#include "component.h"

Component::Component()
{
#ifdef DEBUG_BUILD
  HexAddressString = Util::StringFormat("0x%X", this);
#endif
}

// =============================================================================

void Component::Prepare(GameObject* owner)
{
  OwnerGameObject = owner;

  PrepareAdditional();
}

// =============================================================================

void Component::PrepareAdditional()
{
}

#ifdef DEBUG_BUILD
StringV Component::Dump(size_t indent)
{
  const std::string spaces(indent, ' ');

  StringV res;

  res.push_back(
    Util::StringFormat("%s'0x%X': {", spaces.data(), this)
  );

  res.push_back(
    Util::StringFormat("%s  'typeid': '%s',",
                       spaces.data(),
                       typeid(*this).name())
  );

  res.push_back(
    Util::StringFormat("%s  'OwnerGameObject': 0x%X,",
                       spaces.data(),
                       OwnerGameObject)
  );

  res.push_back(
    Util::StringFormat("%s  'IsEnabled': '%s',",
                       spaces.data(),
                       IsEnabled ? "Y" : "N")
  );

  res.push_back(
    Util::StringFormat("%s}", spaces.data())
  );

  return res;
}
#endif
