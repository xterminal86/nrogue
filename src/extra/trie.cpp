#include "trie.h"

// =============================================================================

Trie::Trie()
{
}

// =============================================================================

Trie::Trie(const std::unordered_set<std::string>& words)
{
  for (auto& word : words)
  {
    Add(word);
  }
}

// =============================================================================

bool Trie::Find(const std::string& word)
{
  Trie* head = this;

  for (char c : word)
  {
    if (head->_root.count(c) == 0)
    {
      return false;
    }

    head = &head->_root[c];
  }

  //
  // If we scanned the whole word and got here, it is in the trie,
  // whether formally (as part of supplied dictionary) or not.
  //
  return true;
}

// =============================================================================

const std::set<std::string>&
Trie::FindAll(const std::string& prefix)
{
  _hints.clear();

  Trie* head = this;

  //
  // Return everything if prefix is an empty string (hit 'TAB' in CLI with
  // nothing typed).
  //
  if (prefix.empty())
  {
    for (auto& word : _dict)
    {
      _hints.insert(word);
    }
  }
  else
  {
    bool prefixFound = true;

    for (char c : prefix)
    {
      if (head->_root.count(c) == 0)
      {
        prefixFound = false;
        break;
      }

      head = &head->_root[c];
    }

    //
    // Make sure that specified prefix is actually in the trie.
    // Otherwise ther's nothing to search for.
    //
    if (prefixFound)
    {
      std::string start = prefix;
      TraverseHead(head, prefix, start, _hints);
    }
  }

  return _hints;
}

// =============================================================================

void Trie::Add(const std::string& word)
{
  _dict.insert(word);

  Trie* head = this;
  for (char c : word)
  {
    head = &head->_root[c];
  }

  //
  // To show in printout that a word from the dictionary is actually in the
  // trie by printing out special "terminator" character.
  // Output still looks kinda shit though.
  //
  head->_root[_terminator] = Trie();

  //
  // Implementation with references doesn't work because in C++ you can't
  // reassign references.
  // It's obvious in trivial examples, like:
  //
  // int a = 42, b = 33;
  // int& r = a;
  // r = b;
  //
  // Here this will modify a through r via assignment of b, so this is
  // basically the same as saying a = b.
  //
  // But I still don't understand why this doesn't work here. It looks good
  // semantically. It doesn't even crash, because we're assigning to the
  // 'this' value, but I guess that's UB or whatever.
  // I suppose I could work around this by implementing helper search
  // methods that will find and return proper references, but solution with
  // pointers is actually much simpler and straightforward, so I'll just
  // leave this as a reminder of C++ bullshit.
  //
  /*
  Trie& head = *this;

  printf("this = 0x%llX, head._root = 0x%llX\n", this, &head._root);

  for (char c : word)
  {
    head = head._root[c];
    printf("head: 0x%llX root: 0x%llX\n", &head, &_root[c]);
  }

  head._root['\0'] = Trie();
  */
}

void Trie::TraverseHead(Trie* head,
                        const std::string prefix,
                        std::string& soFar,
                        std::set<std::string>& hints)
{
  //
  // Here's an example of how algorithm works:
  //
  // Suppose we have words 'ban', 'band' and 'bandit'.
  // The trie will look something like this:
  //
  //     B
  //     |
  //     A
  //     |
  //     N--<0>
  //     |
  //     D---I---T--<0>
  //     |
  //    <0>
  //
  // We start the search with prefix 'ba'.
  //
  //   +-B
  //   | |
  //   +>A
  //     |
  //     N--<0>
  //     |
  //     D---I---T--<0>
  //     |
  //    <0>
  //
  // There's only one item in '_root' right now - it's 'n'.
  // Set new prefix to 'ban' and proceed into recursive call.
  //
  //   +-B
  //   | |
  //   +-A
  //   | |
  //   +>N--<0>
  //     |
  //     D---I---T--<0>
  //     |
  //    <0>
  //
  // Now there will be two items in '_root': terminator and 'd'.
  // Suppose we hit 'd' first.
  // Prefix is now 'band' and we must go deeper into recursion.
  //
  //   +-B
  //   | |
  //   +-A
  //   | |
  //   +-N--<0>
  //   | |
  //   +>D---I---T--<0>
  //     |
  //    <0>
  //
  // But let's not go inside it this time and suppose we returned from
  // there.
  // Now our 'soFar' string is still set to 'band', which is incorrect.
  // So we reassign it to be 'ban' again so that next iteration of for loop
  // works correctly.
  //
  //   +-B
  //   | |
  //   +-A
  //   | |
  //   +>N--<0>
  //     |
  //     D---I---T--<0>
  //     |
  //    <0>
  //
  // On this next iteration we hit '\0' and add the word 'ban' into the
  // list of hints, as intended.
  //
  // Everything else works by analogy.
  //
  for (auto& kvp : head->_root)
  {
    if (kvp.first == _terminator)
    {
      hints.insert(soFar);
    }
    else
    {
      soFar.append(1, kvp.first);

      //
      // We cannot pass 'soFar' as new prefix, because it is passed by
      // reference in third argument and is being modified, so we need to
      // make a copy of it.
      //
      TraverseHead(&kvp.second, soFar, soFar, hints);

      //
      // After we hit terminator, we need to restore so far accumulated
      // string to old prefix for next iteration.
      //
      soFar = prefix;
    }
  }
}

// =============================================================================

void Trie::Print()
{
  Trie& start = (*this);
  PrintIntl(start, 0);
}

// =============================================================================

void Trie::PrintIntl(Trie& start, uint64_t indent)
{
  std::string spaces(indent, ' ');
  const std::string terminator = "<0>";

  for (auto& kvp : start._root)
  {
    if (kvp.first == _terminator)
    {
      printf("%s%s\n", spaces.data(), terminator.data());
    }
    else
    {
      printf("%s%c\n", spaces.data(), kvp.first);
      PrintIntl(start._root[kvp.first], indent + 4);
    }
  }
}
