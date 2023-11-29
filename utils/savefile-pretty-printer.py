#!/usr/bin/python3

import argparse;

################################################################################

def PrettyPrint(contents : str):
  buffer_ = [];

  line = "";

  indent = 0;

  closingBraceFound = False;

  for c in contents:
    line += c;

    if c == '{':
      spaces = " " * indent;
      buffer_.append(f"{ spaces }{ line }");
      indent += 2;
      line = "";
    elif c == '}':
      closingBraceFound = True;
    elif c == ',':
      if closingBraceFound:
        indent += -2;
        closingBraceFound = False;
      spaces = " " * indent;
      buffer_.append(f"{ spaces }{ line }");
      line = "";

  for item in buffer_:
    print(item);

################################################################################

def main():
  parser = argparse.ArgumentParser();

  parser.add_argument("SAVEFILE");

  args = parser.parse_args();

  fname = args.SAVEFILE;

  contents = None;

  with open(fname) as f:
    contents = f.read();

  PrettyPrint(contents);

################################################################################

if __name__ == "__main__":
  main();
