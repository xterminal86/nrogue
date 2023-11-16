#include "serializer.h"

#include "util.h"

void NRS::SetString(const std::string& s, size_t index)
{
  if (_content.size() <= index)
  {
    _content.resize(index + 1);
  }

  _content[index] = s;
}

// =============================================================================

const std::string& NRS::GetString(size_t index) const
{
  if (index >= _content.size())
  {
    return kEmptyString;
  }

  return _content[index];
}

// =============================================================================

void NRS::SetInt(int64_t value, size_t index)
{
  SetString(std::to_string(value), index);
}

// =============================================================================

int64_t NRS::GetInt(size_t index) const
{
  return std::stoll(GetString(index).data());
}

// =============================================================================

void NRS::SetUInt(uint64_t value, size_t index)
{
  SetString(std::to_string(value), index);
}

// =============================================================================

uint64_t NRS::GetUInt(size_t index) const
{
  return std::stoull(GetString(index).data());
}

// =============================================================================

void NRS::Clear()
{
  _content.clear();
  _children.clear();
  _childIndexByName.clear();
  _currentIndent = 0;
  _parsingScopeCount = 0;
  _parsingState = ParsingState::UNDEFINED;
}

// =============================================================================

size_t NRS::ValuesCount() const
{
  return _content.size();
}

// =============================================================================

size_t NRS::ChildrenCount() const
{
  return _children.size();
}

// =============================================================================

bool NRS::Has(const std::string& nodeName)
{
  return (_childIndexByName.count(nodeName) == 1);
}

// =============================================================================

NRS& NRS::operator[](const std::string& nodeName)
{
  if (_childIndexByName.count(nodeName) == 0)
  {
    _childIndexByName[nodeName] = _children.size();

    _children.push_back({ nodeName, NRS() });
  }

  return _children[_childIndexByName[nodeName]].second;
}

// =============================================================================

NRS& NRS::GetNode(const std::string& path)
{
  size_t pos = path.find_first_of('.');
  if (pos != std::string::npos)
  {
    std::string nodeName = path.substr(0, pos);
    return (*this)[nodeName].GetNode(path.substr(pos + 1, path.length()));
  }
  else
  {
    return (*this)[path];
  }
}

// =============================================================================

bool NRS::Save(const std::string& fileName, bool encrypt)
{
  bool ok = false;

  std::ofstream file(fileName, std::ios::binary);
  if (file.is_open())
  {
    std::string serialized = ToStringObject();

    if (encrypt)
    {
      serialized = Util::Encrypt(serialized);
    }

    //
    // Will use write() here as well, because we need to store values exactly,
    // as operator << has some constraints. At least from what I read on the
    // Internet, default format flags are skipws | dec, which means it will skip
    // whitespaces (which may suddenly appear after encryption). It seems these
    // can be circumvented using so-called manipulators, but fuck that.
    //
    file.write(serialized.data(), serialized.length());
    file.close();
    ok = true;
  }

  return ok;
}

// =============================================================================

NRS::LoadResult NRS::Load(const std::string& fname, bool encrypted)
{
  LoadResult res = LoadResult::LOAD_OK;

  std::string loaded;

  std::stringstream ss;

  //
  // If we're going to use encryption, we cannot use std::getline for reading
  // back, because it reads until it meets a delimiter, which can suddenly
  // appear in encrypted file and thus fuck up our reading.
  //
  std::ifstream file(fname, std::ios::binary | std::ios::ate);
  if (!file.is_open())
  {
    res = LoadResult::ERROR;
    return res;
  }


  file.seekg(0, std::ios::end);
  auto fsize = file.tellg();
  file.seekg(0, std::ios::beg);

  std::string buf(fsize, '\0');

  while(file.read(&buf[0], fsize))
  {
    ss << buf;
  }

  file.close();

  loaded = ss.str();

  if (encrypted)
  {
    loaded = Util::Encrypt(loaded);
  }

  if (CheckSyntax(loaded))
  {
    FromStringObject(loaded);
  }
  else
  {
    res = LoadResult::INVALID_FORMAT;
  }

  return res;
}

// =============================================================================

std::string NRS::MakeOneliner(const std::string& stringObject)
{
  std::stringstream ss;

  bool inQuotes = false;

  for (auto& c : stringObject)
  {
    bool unwantedFound = std::find(_unwantedCharacters.begin(),
                                   _unwantedCharacters.end(),
                                   c) != _unwantedCharacters.end();

    if (c == '\"')
    {
      inQuotes = !inQuotes;
    }

    if (inQuotes || !unwantedFound)
    {
      ss << c;
    }
  }

  return ss.str();
}

// =============================================================================

std::string NRS::ToStringObject()
{
  _currentIndent = 0;
  std::stringstream ss;
  WriteIntl(*this, ss);
  return ss.str();
}

// =============================================================================

void NRS::WriteIntl(const NRS& d, std::stringstream& ss)
{
  for (auto& item : d._children)
  {
    if (item.second._children.empty())
    {
      ss << item.first << ":";

      size_t nItems = item.second.ValuesCount();
      for (size_t i = 0; i < nItems; i++)
      {
        bool itemsLeft = ((nItems - i) > 1);

        size_t x = item.second.GetString(i).find_first_of("/ ,");
        if (x != std::string::npos)
        {
          ss << "\"" << item.second.GetString(i) << "\""
            << (itemsLeft ? "/" : "");
        }
        else
        {
          ss << item.second.GetString(i)
            << (itemsLeft ? "/" : "");
        }
      }

      ss << ",";
    }
    else
    {
      ss << item.first << ":{";

      _currentIndent++;

      WriteIntl(item.second, ss);

      ss << "},";
    }
  }

  if (_currentIndent > 0)
  {
    _currentIndent--;
  }
}

// =============================================================================

void NRS::FromStringObject(const std::string& so)
{
  //
  // Just in case.
  //
  Clear();

  //
  // And this one too.
  //
  std::string oneliner = MakeOneliner(so);

  std::string key;
  std::string value;

  std::stack<NRS*> tree;
  tree.push(this);

  bool inQuotesTop = false;

  std::stringstream ss;
  for (auto& c : oneliner)
  {
    switch(c)
    {
      case '\"':
        inQuotesTop = !inQuotesTop;
        ss << c;
        break;

      case ':':
      {
        key = ss.str();
        ss.str(std::string());
      }
      break;

      case ',':
      {
        if (inQuotesTop)
        {
          ss << c;
        }
        else
        {
          value = ss.str();

          bool inQuotes = false;

          std::string valueItem;

          size_t valueIndex = 0;

          //
          // Check if it's a list.
          //
          for (auto& ch : value)
          {
            switch (ch)
            {
              case '\"':
                inQuotes = !inQuotes;
                break;

              default:
              {
                if (inQuotes)
                {
                  valueItem.append(1, ch);
                }
                else
                {
                  if (ch == '/')
                  {
                    (*tree.top())[key].SetString(valueItem, valueIndex);
                    valueIndex++;
                    valueItem.clear();
                  }
                  else
                  {
                    valueItem.append(1, ch);
                  }
                }
              }
              break;
            }
          }

          if (!valueItem.empty())
          {
            (*tree.top())[key].SetString(valueItem, valueIndex);
          }

          ss.str(std::string());
        }
      }
      break;

      case '{':
      {
        tree.push(&(*tree.top())[key]);
        ss.str(std::string());
      }
      break;

      case '}':
      {
        if (!tree.empty())
        {
          tree.pop();
        }

        ss.str(std::string());
      }
      break;

      default:
        ss << c;
        break;
    }
  }
}

// =============================================================================

bool NRS::CheckSyntax(const std::string& so)
{
  std::string oneliner = MakeOneliner(so);

  _parsingState = ParsingState::UNDEFINED;

  std::stringstream ss;

  size_t index = 0;

  for (auto& c : oneliner)
  {
    DriveStateMachine(c);

    if (_parsingState == ParsingState::ERROR)
    {
      break;
    }

    index++;
  }

  //
  // To handle negative case:
  //
  // obj : { key : value,
  //
  bool success = (_parsingState == ParsingState::VALUE_DONE
               && _parsingScopeCount == 0);

  _parsingState = success ? ParsingState::PARSING_OK : ParsingState::ERROR;

  return (_parsingState == ParsingState::PARSING_OK);
}

// =============================================================================

void NRS::DriveStateMachine(const char currentChar, bool debug)
{
  if (debug)
  {
    printf("%c\n", currentChar);
  }

  auto IsValidChar = [this](const char c)
  {
    return (c > 32 && c < 127 && _transitionChars.count(c) == 0);
  };

  switch (_parsingState)
  {
    // -------------------------------------------------------------------------

    case ParsingState::UNDEFINED:
    {
      if (IsValidChar(currentChar))
      {
        _parsingState = ParsingState::READING_KEY;

        if (debug)
        {
          printf("UNDEFINED -> READING_KEY\n");
        }
      }
      else
      {
        _parsingState = ParsingState::ERROR;

      }
    }
    break;

    // -------------------------------------------------------------------------

    case ParsingState::READING_KEY:
    {
      if (currentChar == ':')
      {
        _parsingState = ParsingState::KEY_DONE;

        if (debug)
        {
          printf("READING_KEY -> KEY_DONE\n");
        }
      }
      else
      {
        if (!IsValidChar(currentChar))
        {
          _parsingState = ParsingState::ERROR;
        }
      }
    }
    break;

    // -------------------------------------------------------------------------

    case ParsingState::KEY_DONE:
    {
      if (currentChar == '\"')
      {
        _parsingState = ParsingState::READING_VALUE_Q;

        if (debug)
        {
          printf("KEY_DONE -> READING_VALUE_Q\n");
        }
      }
      else if (currentChar == '{')
      {
        _parsingScopeCount++;
        _parsingState = ParsingState::READING_OBJECT;

        if (debug)
        {
          printf("KEY_DONE -> READING_OBJECT\n");
        }
      }
      else
      {
        if (IsValidChar(currentChar))
        {
          _parsingState = ParsingState::READING_VALUE;

          if (debug)
          {
            printf("KEY_DONE -> READING_VALUE\n");
          }
        }
        else
        {
          _parsingState = ParsingState::ERROR;
        }
      }
    }
    break;

    // -------------------------------------------------------------------------

    case ParsingState::READING_VALUE:
    {
      switch (currentChar)
      {
        case ',':
        {
          _parsingState = ParsingState::VALUE_DONE;

          if (debug)
          {
            printf("READING_VALUE -> VALUE_DONE\n");
          }
        }
        break;

        case '/':
        {
          _parsingState = ParsingState::READING_LIST;

          if (debug)
          {
            printf("READING_VALUE -> READING_LIST\n");
          }
        }
        break;

        default:
        {
          if (!IsValidChar(currentChar))
          {
            _parsingState = ParsingState::ERROR;
          }
        }
        break;
      }
    }
    break;

    // -------------------------------------------------------------------------

    case ParsingState::VALUE_DONE:
    {
      switch (currentChar)
      {
        case '}':
        {
          if (_parsingScopeCount > 0)
          {
            _parsingScopeCount--;
            _parsingState = ParsingState::OBJECT_DONE;

            if (debug)
            {
              printf("VALUE_DONE -> OBJECT_DONE\n");
            }
          }
          else
          {
            _parsingState = ParsingState::ERROR;
          }
        }
        break;

        default:
        {
          if (IsValidChar(currentChar))
          {
            _parsingState = ParsingState::READING_KEY;

            if (debug)
            {
              printf("VALUE_DONE -> READING_KEY\n");
            }
          }
          else
          {
            _parsingState = ParsingState::ERROR;
          }
        }
        break;
      }
    }
    break;

    // -------------------------------------------------------------------------

    case ParsingState::READING_OBJECT:
    {
      if (IsValidChar(currentChar))
      {
        _parsingState = ParsingState::READING_KEY;

        if (debug)
        {
          printf("READING_OBJECT -> READING_KEY\n");
        }
      }
      else
      {
        _parsingState = ParsingState::ERROR;
      }
    }
    break;

    // -------------------------------------------------------------------------

    case ParsingState::OBJECT_DONE:
    {
      switch (currentChar)
      {
        case ',':
        {
          _parsingState = ParsingState::VALUE_DONE;

          if (debug)
          {
            printf("OBJECT_DONE -> VALUE_DONE\n");
          }
        }
        break;

        default:
          _parsingState = ParsingState::ERROR;
          break;
      }
    }
    break;

    // -------------------------------------------------------------------------

    case ParsingState::READING_VALUE_Q:
    {
      if (currentChar == '\"')
      {
        _parsingState = ParsingState::VALUE_Q_DONE;

        if (debug)
        {
          printf("READING_VALUE_Q -> VALUE_Q_DONE\n");
        }
      }
    }
    break;

    // -------------------------------------------------------------------------

    case ParsingState::VALUE_Q_DONE:
    {
      switch(currentChar)
      {
        case ',':
        {
          _parsingState = ParsingState::VALUE_DONE;

          if (debug)
          {
            printf("VALUE_Q_DONE -> VALUE_DONE\n");
          }
        }
        break;

        case '/':
        {
          _parsingState = ParsingState::READING_LIST;

          if (debug)
          {
            printf("VALUE_Q_DONE -> READING_LIST\n");
          }
        }
        break;

        default:
          _parsingState = ParsingState::ERROR;
          break;
      }
    }
    break;

    // -------------------------------------------------------------------------

    case ParsingState::READING_LIST:
    {
      switch(currentChar)
      {
        case '\"':
        {
          _parsingState = ParsingState::READING_VALUE_Q;

          if (debug)
          {
            printf("READING_LIST -> READING_VALUE_Q\n");
          }
        }
        break;

        default:
        {
          if (IsValidChar(currentChar))
          {
            _parsingState = ParsingState::READING_VALUE;

            if (debug)
            {
              printf("READING_LIST -> READING_VALUE\n");
            }
          }
          else
          {
            _parsingState = ParsingState::ERROR;
          }
        }
        break;
      }
    }
    break;
  }
}

// =============================================================================

std::string NRS::ToPrettyString()
{
  std::vector<std::string> lines;

  std::stringstream ss;

  size_t indent = 0;

  char prevChar = '\0';

  std::string oneliner = ToStringObject();

  bool inQuotes = false;

  for (auto& c : oneliner)
  {
    switch(c)
    {
      case '\"':
        inQuotes = !inQuotes;
        break;

      case ':':
      {
        ss << " " << c << " ";
      }
      break;

      case '{':
      {
        ss << c << "\n";
        std::string line = ss.str();
        line.insert(0, indent, ' ');
        lines.push_back(line);
        ss.str(std::string());
        indent += 2;
      }
      break;

      case ',':
      {
        if (inQuotes)
        {
          ss << c;
        }
        else
        {
          ss << c << "\n";

          if (prevChar == '}')
          {
            indent -= 2;
          }

          std::string line = ss.str();
          line.insert(0, indent, ' ');
          lines.push_back(line);
          ss.str(std::string());
        }
      }
      break;

      default:
        ss << c;
        break;
    }

    prevChar = c;
  }

  ss.str(std::string());

  for (auto& line : lines)
  {
    ss << line;
  }

  return ss.str();
}

// =============================================================================

std::string NRS::DumpObjectStructureToString()
{
  std::stringstream ss;

  std::function<void(NRS*,std::stringstream&,size_t)>
  DumpIntl = [&](NRS* node, std::stringstream& ss, size_t indent)
  {
    if (node->ChildrenCount() == 0)
    {
      const std::string indentation(indent, '.');

      char buf[32];

      for (auto& item : node->_content)
      {
        sprintf(buf, "0x%X", &item);
        ss << indentation << item << " (" << buf << ")\n";
      }
    }
    else
    {
      const std::string indentation(indent, '.');

      char buf[32];

      for (auto& p : node->_children)
      {
        sprintf(buf, "0x%X", &p.second);
        ss << indentation << "'" << p.first << "'" << " (" << buf << ")" << "\n";
        DumpIntl(&p.second, ss, indent + 2);
      }
    }
  };

  char buf[32];
  sprintf(buf, "0x%X", this);
  ss << "--- start of [NRS] (" << buf << ") ---\n";

  DumpIntl(this, ss, 0);

  ss << "--- end of [NRS] (" << buf << ") ---\n";

  return ss.str();
}
