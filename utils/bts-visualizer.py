#!/usr/bin/python3

import sys;

Script = "";

ToParse = [];
Order   = [];
NodesById = {};
UmlData = [];

def FindMaxIndent():
  indent = 0;

  for item in ToParse:
    ind = int(item["indent"]);

    if item["visited"] == False and ind > indent:
      indent = ind;

  return indent;

def GetNodeType(data):
  spl = data.split(' ');
  return spl[0];

def GetNodeData(data):
  spl = data.split(' ');

  if len(spl) > 1:
    return spl[1];

  return None;

def GenerateUmlDefinitions():
  output = "";

  # print(UmlData);

  for item in UmlData:
    nodeData = item["nodeData"];
    output += "\n";
    for i in nodeData:
      if i != None:
        output += i;
        output += "\n";

  output += "\n\n";

  for item in Order:
    id = item["data"]["id"];
    data = item["data"]["data"];

    pId = item["parent"]["id"];
    pData = item["parent"]["data"];

    output += "s{} ---> s{}\n".format(pId, id);

    # print(id, ") ", data, " <------ ", pId, ") ", pData);

  print(output);

def main():
  start = False;

  indent = 0;

  line = "";

  id = 0;

  for c in Script:
    if c == ' ' and start == False:
      indent += 1;

    if c == '[':
      start = True;
    elif c == ']':
      start = False;
      data = { "indent" : indent, "data" : line, "visited" : False, "id" : id };
      nodeType = GetNodeType(data["data"]);
      nodeName = "s{}".format(id);
      umlData = "state {} as \"{}\"".format(nodeName, nodeType);
      nodeData = GetNodeData(data["data"]);
      umlDesc = None;
      if nodeData != None:
        umlDesc = "{} : {}".format(nodeName, nodeData);
      UmlData.append({ "id" : id, "nodeData" : [ umlData, umlDesc ] });
      ToParse.append(data);
      NodesById[id] = data;
      line = "";
      indent = 0;
      id += 1;
    else:
      if start == True:
        line += c;

  # print(ToParse);

  indent = FindMaxIndent();

  parent = None;

  while indent != 0:
    for item in ToParse:
      if item["visited"] == True:
        continue;

      if int(item["indent"]) == indent:
        Order.append({ "data" : item, "parent" : parent });
        # print(item, " || ", parent);
        item["visited"] = True;
        break;

      parent = item;

    indent = FindMaxIndent();

  GenerateUmlDefinitions();

if __name__ == "__main__":
  for line in sys.stdin:
    Script += line;

  main();