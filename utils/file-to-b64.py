#!/usr/bin/python3

import argparse;
import base64;

################################################################################

def main():
  parser = argparse.ArgumentParser();

  parser.add_argument("FILE", type=str);

  args = parser.parse_args();

  fname = args.FILE;

  spl = fname.split("/");

  varName = spl[-1].split(".");

  if len(varName) > 1:
    varName = f"{ varName[0] }-{ varName[1] }";
  else:
    varName = varName[0];

  data = None;

  try:
    with open(fname, "rb") as f:
      data = f.read();
  except Exception as e:
    print(f"{ e }");
    return;

  output = f"const std::string { varName } = \n";
  output += "\"";

  b64 = base64.encodebytes(data);
  s = b64.decode();
  lineCounter = 0;
  for c in s:
    if c == '\n':
      output += "\"\n\"";
      continue;

    output += c;
    lineCounter += 1;

  output = output[:-2];
  output += ";";

  print(output);

################################################################################

if __name__ == "__main__":
  main();
