#include "component.h"

Component::Component()
{
#ifdef DEBUG_BUILD
  HexAddressString = Util::StringFormat("0x%lX", this);
  AnyObjectByAddr[HexAddressString] = this;
#endif
}

Component::~Component()
{
#ifdef DEBUG_BUILD
  AnyObjectByAddr.erase(HexAddressString);
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

  res.push_back( I_OBJ_START_NAMED(spaces, typeid(*this).name()) );

  res.push_back( I_PTR_NAMED(spaces, "addr", this) );
  res.push_back( I_BOOL(spaces, IsEnabled) );
  res.push_back( I_PTR(spaces, OwnerGameObject) );

  res.push_back( I_OBJ_END(spaces) );

  return res;
}
#endif
