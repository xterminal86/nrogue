#include "serializer.h"

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

bool NRS::Save(const std::string& fileName)
{
  bool ok = false;

  std::ofstream file(fileName);
  if (file.is_open())
  {
    std::string serialized = ToStringObject();
    file << serialized;
    file.close();
    ok = true;
  }

  return ok;
}

// =============================================================================

bool NRS::Load(const std::string& fname)
{
  bool ok = false;

  std::ifstream file(fname);
  if (file.is_open())
  {
    std::stringstream ss;
    while (!file.eof())
    {
      std::string line;
      std::getline(file, line);
      ss << line << "\n";
    }

    file.close();

    FromStringObject(ss.str());

    ok = true;
  }

  return ok;
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

  bool res = true;

  size_t index = 0;

  for (auto& c : oneliner)
  {
    DriveStateMachine(c);

    if (_parsingState == ParsingState::ERROR)
    {
      res = false;
      break;
    }

    index++;
  }

  return res;
}

// =============================================================================

void NRS::DriveStateMachine(const char currentChar)
{
  //printf("%c\n", currentChar);

  auto IsValidChar = [this](const char c)
  {
    return (c > 32 && c < 127 && _transitionsChars.count(c) == 0);
  };

  switch (_parsingState)
  {
    // -------------------------------------------------------------------------

    case ParsingState::UNDEFINED:
    {
      _parsingState = IsValidChar(currentChar)
                    ? ParsingState::READING_KEY
                    : ParsingState::ERROR;

      //printf("UNDEFINED -> READING_KEY\n");
    }
    break;

    // -------------------------------------------------------------------------

    case ParsingState::READING_KEY:
    {
      if (currentChar == ':')
      {
        _parsingState = ParsingState::KEY_DONE;
        //printf("READING_KEY -> KEY_DONE\n");
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
        _parsingQuotesFlag = !_parsingQuotesFlag;
        _parsingState = ParsingState::READING_VALUE;
        //printf("KEY_DONE -> READING_VALUE (q)\n");
      }
      else if (currentChar == '{')
      {
        _parsingScopeCount++;
        _parsingState = ParsingState::READING_KEY;
        //printf("KEY_DONE -> READING_KEY\n");
      }
      else
      {
        if (IsValidChar(currentChar))
        {
          _parsingState = ParsingState::READING_VALUE;
          //printf("KEY_DONE -> READING_VALUE\n");
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
      if (_parsingQuotesFlag)
      {
        if (currentChar == '\"')
        {
          _parsingQuotesFlag = !_parsingQuotesFlag;
          //printf("READING_VALUE (q) -> READING_VALUE\n");
        }
      }
      else
      {
        if (currentChar == '\"')
        {
          _parsingQuotesFlag = !_parsingQuotesFlag;
          //printf("READING_VALUE -> READING_VALUE (q)\n");
        }
        else if (currentChar == ',')
        {
          _parsingState = ParsingState::VALUE_DONE;
          //printf("READING_VALUE -> VALUE_DONE\n");
        }
        else if (currentChar != '/' && !IsValidChar(currentChar))
        {
          _parsingState = ParsingState::ERROR;
        }
      }
    }
    break;

    // -------------------------------------------------------------------------

    case ParsingState::VALUE_DONE:
    {
      if (currentChar == '}')
      {
        if (_parsingScopeCount > 0)
        {
          _parsingScopeCount--;
          _parsingState = ParsingState::READING_VALUE;
        }
        else
        {
          _parsingState = ParsingState::ERROR;
        }
      }
      else
      {
        _parsingState = IsValidChar(currentChar)
                      ? ParsingState::READING_KEY
                      : ParsingState::ERROR;

        //printf("VALUE_DONE -> READING_KEY\n");
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

      for (auto& kvp : node->_childIndexByName)
      {
        sprintf(buf, "0x%X", &(node->_children[kvp.second].second));
        ss << indentation << kvp.first << " (" << buf << ")" << "\n";
        DumpIntl(&(node->_children[kvp.second].second), ss, indent + 2);
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
