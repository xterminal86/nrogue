#!/usr/bin/python3

import argparse;

################################################################################

def main():
  parser = argparse.ArgumentParser();

  parser.add_argument("FILE", type=str);

  args = parser.parse_args();

  fname = args.FILE;

  bts = None;

  with open(fname, "rb") as f:
    bts = f.read()

  output = (
    "std::vector<uint8_t> bytes = \n"
    "{\n"
    "  "
  );

  counter = 0;
  for b in bts:
    output += f"0x{b:02x},"
    counter += 1;

    if (counter >= 20):
      counter = 0;
      output += "\n  ";

  output += "\n};\n";

  print(output);

################################################################################

if __name__ == "__main__":
  main();
