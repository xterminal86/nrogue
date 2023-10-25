#include "util.h"
#include "rng.h"

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    printf("Usage: %s <ALLOW_DOUBLE_VOWELS>\n", argv[0]);
    return 0;
  }

  RNG::Instance().Init();

  bool allowDoubleVowels = argv[1];

  for (size_t i = 0; i < 20; i++)
  {
    std::string name = Util::GenerateName(allowDoubleVowels, false);
    printf("%s\n", name.data());
  }

  return 0;
}
