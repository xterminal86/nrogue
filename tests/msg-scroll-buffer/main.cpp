#include <iostream>

#include "util.h"
#include "msg-scroll-buffer.h"

int main(int argc, char* argv[])
{
  MsgScrollBuffer<std::string> b(10, 3);

  for (size_t i = 0; i < 15; i++)
  {
    b.AddMessage(Util::StringFormat("Message %lu", (i + 1)));
  }

  b.SetScrollState(MessageBufferScrollState::TOP);

  printf("Use 'w' 's' to scroll. 'q' to exit.\n\n");

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

      //
      // 'break' from 'case' doesn't seem to work.
      //
      // ***********************************************************************
      // *                                                                     *
      // *                          Achievement unlocked:                      *
      // *                 use goto in production code (kinda).                *
      // *                                                                     *
      // ***********************************************************************
      case 'q':
      case 'Q':
      {
        goto _exit;
      }
      break;

      default:
        break;
    }
  }

_exit:

  return 0;
}
