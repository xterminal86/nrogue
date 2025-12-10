#include "globals.h"
#include "enumerations.h"

#include "util.h"

#include "shadowcaster.h"

int main(int argc, char* argv[])
{
  /*
  if (!Game::Init(MapType::TEST_LEVEL))
  {
    return 1;
  }

  Game::Run();
  */

  Shadowcaster s;

  printf("%s\n", s.ToString().data());

  return 0;
}
