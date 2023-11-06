#include "replay-start-state.h"

#include "application.h"
#include "printer.h"

#include <filesystem>

void ReplayStartState::Prepare()
{
  using DI   = std::filesystem::directory_iterator;
  using Path = std::filesystem::path;

  _files.clear();

  _cursorIndex = 0;

  std::string curDir = std::filesystem::current_path().string();

  for (auto& i : DI(curDir))
  {
    const Path& p = i.path();

    if (p.extension().string() == Strings::ReplayFileExtension)
    {
      std::string fname = p.filename().string();
      _files[fname] = ReadReplayFileData(fname);
    }
  }
}

// =============================================================================

void ReplayStartState::HandleInput()
{
  _keyPressed = GetKeyDown();

  switch (_keyPressed)
  {
    #ifdef USE_SDL
    case KEY_DOWN:
    #endif
    case NUMPAD_2:
      _cursorIndex++;
      break;

    #ifdef USE_SDL
    case KEY_UP:
    #endif
    case NUMPAD_8:
      _cursorIndex--;
      break;

    case '<':
      _replaySpeed--;
      break;

    case '>':
      _replaySpeed++;
      break;

    case VK_CANCEL:
      Application::Instance().ChangeState(GameStates::MENU_STATE);
      break;

    case VK_ENTER:
      auto it = _files.begin();
      std::advance(it, _cursorIndex);
      Application::Instance().LoadReplay(it->second.Filename);
      Application::Instance().ReplaySpeed_ = (ReplaySpeed)_replaySpeed;
      Application::Instance().ReplayMode = true;
      Application::Instance().ChangeState(GameStates::START_GAME_STATE);
      break;
  }

  _cursorIndex = Util::Clamp(_cursorIndex, 0, _files.size() - 1);
  _replaySpeed = Util::Clamp(_replaySpeed, 0, (int)ReplaySpeed::INSTANT);
}

// =============================================================================

void ReplayStartState::Update(bool forceUpdate)
{
  if (_keyPressed != -1 || forceUpdate)
  {
    Printer::Instance().Clear();

    Printer::Instance().DrawWindow({ 0, 0 },
                                   { _tw - 1, _th - 1 },
                                   "SELECT FILE TO REPLAY",
                                   Colors::WhiteColor,
                                   Colors::MessageBoxHeaderBgColor,
                                   Colors::WhiteColor,
                                   Colors::BlackColor,
                                   Colors::None);

    if (_files.empty())
    {
      Printer::Instance().PrintFB(_twHalf,
                                  _thHalf,
                                  "No replay files found",
                                  Printer::kAlignCenter,
                                  Colors::WhiteColor);
    }
    else
    {
      int yOffset = 0;

      for (int i = _scrollOffset; i < (int)_files.size(); i++)
      {
        if (yOffset > _maxFilesToDisplay)
        {
          break;
        }

        auto it = _files.begin();
        std::advance(it, i);

        const std::string& fname  = it->first;
        const ReplayFileData& rfd = it->second;

        Printer::Instance().PrintFB(2, 2 + yOffset,
                                    rfd.FileNameTruncated.empty()
                                    ? fname
                                    : rfd.FileNameTruncated,
                                    Printer::kAlignLeft,
                                    Colors::WhiteColor,
                                    (i == _cursorIndex)
                                    ? Colors::ShadesOfGrey::Four
                                    : Colors::BlackColor);

        if (i == _cursorIndex)
        {
          Printer::Instance().PrintFB(_tw - 22, 2, "World seed: ",
                                      Printer::kAlignRight, Colors::WhiteColor);
          std::string toPrint = Util::StringFormat("0x%llX", rfd.WorldSeed);
          Printer::Instance().PrintFB(_tw - 2, 2, toPrint,
                                      Printer::kAlignRight, Colors::WhiteColor);
          //
          Printer::Instance().PrintFB(_tw - 22, 3, "Name: ",
                                      Printer::kAlignRight, Colors::WhiteColor);
          toPrint = Util::StringFormat("%s", rfd.PlayerName.data());
          Printer::Instance().PrintFB(_tw - 2, 3, toPrint,
                                      Printer::kAlignRight, Colors::WhiteColor);
          //
          Printer::Instance().PrintFB(_tw - 22, 4, "Class: ",
                                      Printer::kAlignRight, Colors::WhiteColor);
          toPrint = Util::StringFormat("%s",
                                       GlobalConstants::PlayerClassNameByType.at(rfd.Class).data());
          Printer::Instance().PrintFB(_tw - 2, 4, toPrint,
                                      Printer::kAlignRight, Colors::WhiteColor);
          //
          Printer::Instance().PrintFB(_tw - 22, 5, "Timestamp: ",
                                      Printer::kAlignRight, Colors::WhiteColor);
          toPrint = Util::StringFormat("%s", rfd.Timestamp.data());
          Printer::Instance().PrintFB(_tw - 2, 5, toPrint,
                                      Printer::kAlignRight, Colors::WhiteColor);
        }

        yOffset++;
      }
    }

    std::string replaySpeedText;
    uint32_t textColor = Colors::WhiteColor;

    ReplaySpeed rs = (ReplaySpeed)_replaySpeed;

    switch (rs)
    {
      case ReplaySpeed::SLOW:
        replaySpeedText = "SLOW";
        textColor = 0xAA0000;
        break;

      case ReplaySpeed::NORMAL:
        replaySpeedText = "NORMAL";
        textColor = Colors::WhiteColor;
        break;

      case ReplaySpeed::FAST:
        replaySpeedText = "FAST";
        textColor = Colors::GreenColor;
        break;

      default:
        replaySpeedText = "INSTANT";
        textColor = Colors::CyanColor;
        break;
    }

    Printer::Instance().PrintFB(_twHalf, _th - 2,
                                "Replay speed:",
                                Printer::kAlignRight,
                                Colors::WhiteColor);

    Printer::Instance().PrintFB(_twHalf + 1, _th - 2,
                                replaySpeedText,
                                Printer::kAlignLeft,
                                textColor);

    Printer::Instance().PrintFB(2, _th - 2,
                                "'q' - go back",
                                Printer::kAlignLeft,
                                Colors::WhiteColor);

    Printer::Instance().PrintFB(_tw - 2, _th - 2,
                                "'ENTER' - start replay",
                                Printer::kAlignRight,
                                Colors::WhiteColor);

    Printer::Instance().Render();
  }
}

// =============================================================================

const ReplayStartState::ReplayFileData&
ReplayStartState::ReadReplayFileData(const std::string& fname)
{
  std::ifstream file(fname.data(), std::ifstream::binary);

  _replayFileData.Filename = fname;

  _replayFileData.PlayerName.resize(GlobalConstants::MaxNameLength);

  file.read((char*)&_replayFileData.WorldSeed, sizeof(_replayFileData.WorldSeed));
  file.read(_replayFileData.PlayerName.data(), GlobalConstants::MaxNameLength);

  _replayFileData.PlayerName.erase(_replayFileData.PlayerName.find('\0'));
  _replayFileData.PlayerName.append(1, '\0');

  uint8_t class_ = 255;

  file.read((char*)&class_, sizeof(class_));

  _replayFileData.Class = (PlayerClass)class_;

  size_t tsLength = 0;
  file.read((char*)&tsLength, sizeof(tsLength));

  _replayFileData.Timestamp.resize(tsLength);

  file.read(_replayFileData.Timestamp.data(), tsLength);

  file.close();

  std::string& fnt = _replayFileData.FileNameTruncated;
  fnt = fname;
  fnt.resize(fnt.length() - 4);

  if (fnt.length() > _replayNameCharsLimit)
  {
    fnt.resize(_replayNameCharsLimit);
    _replayFileData.FileNameTruncated = Util::StringFormat("%s...", fnt.data());
  }

  return _replayFileData;
}
