#include "globals.h"
#include "enumerations.h"

int main(int argc, char* argv[])
{
  if (!Game::Init(MapType::TEST_LEVEL))
  {
    return 1;
  }

  Game::Run();
  Game::Shutdown();

  return 0;
}
