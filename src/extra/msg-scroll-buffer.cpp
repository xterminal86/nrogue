#include "msg-scroll-buffer.h"

// =============================================================================

MsgScrollBuffer::MsgScrollBuffer()
{
  Init(kScreenSize * 4);
}

// =============================================================================

MsgScrollBuffer::MsgScrollBuffer(size_t bufSize)
{
  Init(bufSize);
}

// =============================================================================

void MsgScrollBuffer::AddMessage(const std::string& msg)
{
  _bufferIndex++;

  if ((size_t)_bufferIndex == _bufferSize)
  {
    _bufferIndex = 0;
  }

  _buffer[_bufferIndex] = msg;

  if (_msgsCount != _bufferSize)
  {
    _msgsCount++;
  }
}

// =============================================================================

void MsgScrollBuffer::ScrollUp()
{
  if ((size_t)_scrollIndex != _scrollLimit && _msgsCount > kScreenSize)
  {
    _scrollIndex++;
  }
}

// =============================================================================

void MsgScrollBuffer::ScrollDown()
{
  if (_scrollIndex != 0 && _msgsCount > kScreenSize)
  {
    _scrollIndex--;
  }
}

// =============================================================================

void MsgScrollBuffer::ResetScroll()
{
  _scrollIndex = 0;
}

// =============================================================================

void MsgScrollBuffer::Clear()
{
  _msgsCount   = 0;
  _scrollIndex = 0;
  _bufferIndex = -1;

  for (size_t i = 0; i < kScreenSize; i++)
  {
    _msgIndices[i] = -1;
    _output[i] = nullptr;
  }
}

// =============================================================================

void MsgScrollBuffer::Print()
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
    DebugLog("%s\n", _buffer[from].data());

    from++;

    if ((size_t)from == _bufferSize)
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

// =============================================================================

const std::vector<std::string*>& MsgScrollBuffer::GetMessages()
{
  if (_msgsCount != 0)
  {
    int from = (_bufferIndex - kScreenSize - _scrollIndex + 1);
    if (from < 0)
    {
      from = (_msgsCount < kScreenSize || _msgsCount != _bufferSize)
             ? 0
             : (_bufferSize + from);
    }

    int ind = 0;

    size_t msgCount = 0;
    while (true)
    {
      _msgIndices[ind] = from;

      ind++;
      from++;

      if ((size_t)from == _bufferSize)
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

  for (size_t i = 0; i < kScreenSize; i++)
  {
    int ind = _msgIndices[i];
    DebugLog("%d", ind);

    if (ind == -1)
    {
      _output[i] = nullptr;
      break;
    }

    _output[i] = &_buffer[ind];
  }

  DebugLog("==========");

  return _output;
}

// =============================================================================

void MsgScrollBuffer::Init(size_t bufSize)
{
  _bufferSize = bufSize;

  _buffer.resize(_bufferSize);

  _bufferIndex = -1;

  _scrollIndex = 0;
  _msgsCount   = 0;

  _scrollLimit = (_bufferSize > kScreenSize)
                 ? (_bufferSize - kScreenSize)
                 : 0;

  _msgIndices.resize(kScreenSize);
  _output.resize(kScreenSize);
  for (size_t i = 0; i < kScreenSize; i++)
  {
    _msgIndices[i] = -1;
    _output[i] = nullptr;
  }
}

