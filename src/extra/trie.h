#ifndef TRIE_H
#define TRIE_H

#include <cstdio>
#include <cstdint>
#include <vector>
#include <string>
#include <set>
#include <unordered_set>
#include <unordered_map>

//
// Partially ported Python implementation from here:
// https://github.com/xterminal86/yandex-algo/blob/main/sprint8/final/cheat-sheet/main.py
//
class Trie
{
  public:
    Trie();
    Trie(const std::unordered_set<std::string>& words);

    void Add(const std::string& word);

    bool Find(const std::string& word);

    const std::set<std::string>&
    FindAll(const std::string& prefix);

    void Print();

  private:
    void TraverseHead(Trie* head,
                      // This is not a typo, see comments in cpp file.
                      const std::string prefix,
                      std::string& soFar,
                      std::set<std::string>& hints);

    void PrintIntl(Trie& start, uint64_t indent);

    std::unordered_map<char, Trie> _root;

    std::set<std::string> _hints;

    std::unordered_set<std::string> _dict;

    //
    // Cannot be const because implicit copy ctor cannot copy const value
    // (which is logical I guess).
    //
    char _terminator = '\0';
};

#endif // TRIE_H
