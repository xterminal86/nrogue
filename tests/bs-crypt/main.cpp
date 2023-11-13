#include "rng.h"
#include "util.h"

int main(int argc, char* argv[])
{
  RNG::Instance().Init();

  std::string original = "This is a test";

  std::string encrypted = Util::Encrypt(original);
  std::string decrypted = Util::Encrypt(encrypted);

  printf("Original string:\n");
  printf("%s\n\n", original.data());

  printf("Encrypted:\n");

  for (size_t i = 0; i < encrypted.length(); i++)
  {
    printf(".%X.", encrypted[i]);
  }

  printf("\n\n");

  printf("Decrypted:\n");
  printf("%s\n\n", decrypted.data());

  printf("%s\n\n", (original == decrypted) ? "OK" : "FAIL!");

  const size_t bigSize = 1000000;

  std::string decor(80, '-');

  printf("Trying to encrypt string of length %llu...\n\n", bigSize);

  std::string bigString;
  bigString.resize(bigSize);

  for (size_t i = 0; i < bigSize; i++)
  {
    bigString[i] = char(Util::Rolld100() + 32);
  }

  printf("BEFORE\n");
  printf("%s\n", decor.data());

  for (size_t i = 0; i < 30; i++)
  {
    printf("%c", bigString[i]);
  }

  printf(" ...\n");

  printf("%s\n", decor.data());

  encrypted = Util::Encrypt(bigString);

  printf("AFTER\n");
  printf("%s\n", decor.data());

  for (size_t i = 0; i < 30; i++)
  {
    printf("%c", encrypted[i]);
  }

  printf(" ...\n");

  printf("%s\n", decor.data());

  decrypted = Util::Encrypt(encrypted);

  printf("DECRYPTED\n");
  printf("%s\n", decor.data());

  for (size_t i = 0; i < 30; i++)
  {
    printf("%c", decrypted[i]);
  }

  printf(" ...\n");

  printf("%s\n", decor.data());

  printf("%s\n", (bigString == decrypted) ? "OK" : "FAIL!");

  return 0;
}
