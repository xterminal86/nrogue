#include <iostream>

#include "util.h"
#include "msg-scroll-buffer.h"

int main(int argc, char* argv[])
{
  MsgScrollBuffer<std::string> b;

  for (size_t i = 0; i < 10; i++)
  {
    b.AddMessage(Util::StringFormat("Message %lu", (i + 1)));
  }

  b.Print();

  const std::string ruler(80, '=');

  while (true)
  {
    char c;

    std::cin >> c;

    switch (c)
    {
      case 'w':
      case 'W':
      {
        printf("%s\n", ruler.data());
        b.ScrollUp();
        b.Print();
      }
      break;

      case 's':
      case 'S':
      {
        printf("%s\n", ruler.data());
        b.ScrollDown();
        b.Print();
      }
      break;

      default:
        break;
    }
  }

  return 0;
}
