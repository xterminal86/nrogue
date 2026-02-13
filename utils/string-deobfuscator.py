#!/usr/bin/python3

import sys;

CharsByIndex = {};

def CreateData():
  for i in range(0, 128):
    CharsByIndex[i] = chr(i);

################################################################################

def DecodeNumToChar(n):
  index = n & 0x000000FF;
  return CharsByIndex[index];

################################################################################

def Decode(data : list):
  print("Encoded data:");
  print(data);
  print("");

  tmp = "";
  print("\n");
  print(">"*80);
  for c in data:
    res = DecodeNumToChar(c);
    print(f"{ res } = { ord(res) } ({ c })");
    tmp += res;
  print("<"*80);
  print(tmp);

################################################################################

def main():
  CreateData();

  print("Enter numbers separated by whitespace (CTRL+D on empty line to finish)");

  data = "";

  for line in sys.stdin:
    data += line;

  spl = data.split(' ');

  ar = [];

  for item in spl:
    hasChars = item.strip();
    if not hasChars:
      continue;
    try:
      ar.append(int(item.strip()));
    except Exception as e:
      print(f"{ e }");
      exit(1);

  if not ar:
    print("No input");
    exit(0);

  Decode(ar);

################################################################################

if __name__ == "__main__":
  main();
