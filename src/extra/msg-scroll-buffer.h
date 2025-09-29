#ifndef MSGSCROLLBUFFER_H
#define MSGSCROLLBUFFER_H

#include <cstdint>
#include <string>

#include "util.h"

class MsgScrollBuffer
{
  public:
    MsgScrollBuffer();
    MsgScrollBuffer(uint8_t screensCount);

    void AddMessage(const std::string& msg);

    void ScrollUp();
    void ScrollDown();
    void ResetScroll();

    void Clear();
    void Print();

    const std::vector<std::string*>& GetMessages();

  private:
    void Init(size_t bufSize);

    const size_t kScreenSize = 23;

    size_t _bufferSize = 0;

    std::vector<std::string>  _buffer;
    std::vector<std::string*> _output;

    std::vector<int> _msgIndices;

    size_t _msgsCount = 0;

    int _bufferIndex = 0;
    int _scrollIndex = 0;
};

#endif // MSGSCROLLBUFFER_H
