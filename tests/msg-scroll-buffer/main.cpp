#include <iostream>

#include "util.h"

class MsgScrollBuffer
{
  public:
    MsgScrollBuffer()
    {
      Init(kScreenSize * 2);
    }

    MsgScrollBuffer(size_t bufSize)
    {
      Init(bufSize);
    }

    void AddMessage(const std::string& msg)
    {
      _bufferIndex++;

      if (_bufferIndex == _bufferSize)
      {
        _bufferIndex = 0;
      }

      _buffer[_bufferIndex] = msg;

      if (_msgsCount != _bufferSize)
      {
        _msgsCount++;
      }
    }

    void ScrollUp()
    {
      if (_scrollIndex != _scrollLimit && _msgsCount > kScreenSize)
      {
        _scrollIndex++;
      }
    }

    void ScrollDown()
    {
      if (_scrollIndex != 0 && _msgsCount > kScreenSize)
      {
        _scrollIndex--;
      }
    }

    void Print()
    {
      //
      // If there are no messages don't do anything [stupid].
      //
      if (_msgsCount == 0)
      {
        return;
      }

      int from = (_bufferIndex - kScreenSize - _scrollIndex + 1);
      if (from < 0)
      {
        //
        // If we haven't yet accumulated enough messages for scrolling to
        // occur or we haven't filled the buffer, do not perform wrap
        // around calculations to find out proper starting index.
        //
        from = (_msgsCount < kScreenSize || _msgsCount != _bufferSize)
               ? 0
               : (_bufferSize + from);
      }

      size_t msgCount = 0;
      while (true)
      {
        printf("%s\n", _buffer[from].data());

        from++;

        if (from == _bufferSize)
        {
          from = 0;
        }

        msgCount++;

        if (msgCount == _msgsCount || msgCount == kScreenSize)
        {
          break;
        }
      }
    }

  private:
    void Init(size_t bufSize)
    {
      _bufferSize = bufSize;

      _buffer.resize(_bufferSize);

      _bufferIndex = -1;

      _scrollIndex = 0;
      _msgsCount   = 0;

      _scrollLimit = (_bufferSize > kScreenSize)
                     ? (_bufferSize - kScreenSize)
                     : 0;
    }

    const int kScreenSize = 25;

    size_t _bufferSize = 0;

    std::vector<std::string> _buffer;

    size_t _msgsCount = 0;

    int _bufferIndex = 0;
    int _scrollIndex = 0;

    size_t _scrollLimit = 0;
};

int main(int argc, char* argv[])
{
  MsgScrollBuffer b;

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
