#ifndef MSGSCROLLBUFFER_H
#define MSGSCROLLBUFFER_H

#include <cstdint>
#include <string>

#include "util.h"
#include "enumerations.h"

template <typename T>
class MsgScrollBuffer
{
  public:
    MsgScrollBuffer()
    {
      Init(_screenSize * 2);
    }

    // =========================================================================

    MsgScrollBuffer(size_t screenSize, uint8_t screensCount)
    {
      _screenSize = screenSize;

      uint8_t corrected = (screensCount <= 1) ? 2 : screensCount;
      Init(_screenSize * corrected);
    }

    // =========================================================================

    void AddMessage(const T& msg)
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

    // =========================================================================

    T* LastMessage()
    {
      return (_bufferIndex == -1) ? nullptr : &_buffer[_bufferIndex];
    }

    // =========================================================================

    bool IsEmpty()
    {
      return (_msgsCount == 0);
    }

    // =========================================================================

    void ScrollUp()
    {
      if (_msgsCount > _screenSize)
      {
        size_t limit = _msgsCount - _screenSize;
        if ((size_t)_scrollIndex != limit)
        {
          _scrollIndex++;
        }
      }
    }

    // =========================================================================

    void ScrollDown()
    {
      if (_msgsCount > _screenSize)
      {
        if (_scrollIndex != 0)
        {
          _scrollIndex--;
        }
      }
    }

    // =========================================================================

    void PageUp()
    {
      if (_msgsCount > _screenSize)
      {
        _scrollIndex += _screenSize;

        size_t limit = _msgsCount - _screenSize;
        if ((size_t)_scrollIndex > limit)
        {
          _scrollIndex = limit;
        }
      }
    }

    // =========================================================================

    void PageDown()
    {
      if (_msgsCount > _screenSize)
      {
        _scrollIndex -= _screenSize;

        if (_scrollIndex < 0)
        {
          _scrollIndex = 0;
        }
      }
    }

    // =========================================================================

    void ResetScroll()
    {
      _scrollIndex = 0;
    }

    // =========================================================================

    void Clear()
    {
      _msgsCount   = 0;
      _scrollIndex = 0;
      _bufferIndex = -1;

      for (size_t i = 0; i < _screenSize; i++)
      {
        _msgIndices[i] = -1;
        _output[i] = nullptr;
        _lastMsgs[i] = nullptr;
      }
    }

#ifdef BUILD_TESTS
    // =========================================================================

    void Print()
    {
      //
      // If there are no messages don't do anything [stupid].
      //
      if (_msgsCount == 0)
      {
        return;
      }

      int from = (_bufferIndex - _screenSize - _scrollIndex + 1);
      if (from < 0)
      {
        //
        // If we haven't yet accumulated enough messages for scrolling to
        // occur or we haven't filled the buffer, do not perform wrap
        // around calculations to find out proper starting index.
        //
        from = (_msgsCount < _screenSize || _msgsCount != _bufferSize)
               ? 0
               : (_bufferSize + from);
      }

      size_t msgCount = 0;
      while (true)
      {
        printf("%s\n", _buffer[from].data());

        from++;

        if ((size_t)from == _bufferSize)
        {
          from = 0;
        }

        msgCount++;

        if (msgCount == _msgsCount || msgCount == _screenSize)
        {
          break;
        }
      }
    }
#endif

    // =========================================================================

    const std::vector<T*>& GetMessages()
    {
      if (_msgsCount != 0)
      {
        int from = (_bufferIndex - _screenSize - _scrollIndex + 1);
        if (from < 0)
        {
          from = (_msgsCount < _screenSize || _msgsCount != _bufferSize)
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

          if (msgCount == _msgsCount || msgCount == _screenSize)
          {
            break;
          }
        }
      }

      for (size_t i = 0; i < _screenSize; i++)
      {
        int ind = _msgIndices[i];
        if (ind == -1)
        {
          _output[i] = nullptr;
          break;
        }

        _output[i] = &_buffer[ind];
      }

      return _output;
    }

    // =========================================================================

    const std::vector<T*>& GetLastMessages(size_t windowSize)
    {
      if (windowSize == 0 || windowSize > _bufferSize)
      {
        return _empty;
      }

      if (_msgsCount != 0)
      {
        int from = (_bufferIndex - windowSize + 1);
        if (from < 0)
        {
          from = (_msgsCount < _screenSize || _msgsCount != _bufferSize)
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

          if (msgCount == _msgsCount || msgCount == windowSize)
          {
            break;
          }
        }
      }

      for (size_t i = 0; i < windowSize; i++)
      {
        int ind = _msgIndices[i];
        if (ind == -1)
        {
          _lastMsgs[i] = nullptr;
          break;
        }

        _lastMsgs[i] = &_buffer[ind];
      }

      return _lastMsgs;
    }

    // =========================================================================

    T* GetLastMessage()
    {
      return (_bufferIndex < 0) ? nullptr : &_buffer[_bufferIndex];
    }

    // =========================================================================

    void SetScrollState(MessageBufferScrollState setTo)
    {
      if (_msgsCount <= _screenSize)
      {
        return;
      }

      switch (setTo)
      {
        case MessageBufferScrollState::TOP:
          _scrollIndex = (_msgsCount - _screenSize);
          break;

        case MessageBufferScrollState::MIDDLE:
          _scrollIndex = (_msgsCount - _screenSize) / 2;
          break;

        case MessageBufferScrollState::BOTTOM:
          _scrollIndex = 0;
          break;

        default:
          break;
      }
    }

    // =========================================================================

    MessageBufferScrollState GetScrollState() const
    {
      if (_msgsCount <= _screenSize)
      {
        return MessageBufferScrollState::NONE;
      }

      size_t limit = _msgsCount - _screenSize;

      if (_scrollIndex == 0)
      {
        return MessageBufferScrollState::BOTTOM;
      }
      else if ((size_t)_scrollIndex == limit)
      {
        return MessageBufferScrollState::TOP;
      }

      return MessageBufferScrollState::MIDDLE;
    }

    // =========================================================================

    double GetScrollProgress() const
    {
      if (_msgsCount > _screenSize)
      {
        return (double)_scrollIndex / (double)(_msgsCount - _screenSize);
      }

      return 0.0;
    }

  private:
    void Init(size_t bufSize)
    {
      _bufferSize = bufSize;

      _buffer.resize(_bufferSize);

      _bufferIndex = -1;

      _scrollIndex = 0;
      _msgsCount   = 0;

      _msgIndices.resize(_screenSize);
      _output.resize(_screenSize);
      _lastMsgs.resize(_screenSize);

      for (size_t i = 0; i < _screenSize; i++)
      {
        _msgIndices[i] = -1;
        _output[i]     = nullptr;
        _lastMsgs[i]   = nullptr;
      }
    }

    size_t _screenSize = 23;

    size_t _bufferSize = 0;

    std::vector<T>  _buffer;
    std::vector<T*> _output;
    std::vector<T*> _lastMsgs;

    std::vector<T*> _empty;

    std::vector<int> _msgIndices;

    size_t _msgsCount = 0;

    int _bufferIndex = 0;
    int _scrollIndex = 0;
};

#endif // MSGSCROLLBUFFER_H
