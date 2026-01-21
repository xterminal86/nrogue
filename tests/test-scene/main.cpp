#include "globals.h"

int main(int argc, char* argv[])
{
  if (!Game::Init(MapType::TEST_LEVEL))
  {
    return 1;
  }

  Game::Run();

  return 0;
}
