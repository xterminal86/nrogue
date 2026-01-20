#!/usr/bin/python3

import os;

from pathlib import Path;
from PIL     import Image;

def main():
  p = Path("../resources/graphic-tiles").glob("*.png");

  images = [];

  for item in p:
    images.append(str(item));

  images.sort();

  if not images:
    print("No images found!");
    exit(1);

  cppEnum = "enum class GraphicTiles\n{\n";

  first = True;
  for item in images:
    spl = item.split("/");
    ln = len(spl);
    fname = spl[ln - 1].replace(".png", "");
    enumName = fname.upper().replace("-", "_");

    if first:
      cppEnum += f"  { enumName } = 256,\n";
      first = False;
    else:
      cppEnum += f"  { enumName },\n";

  cppEnum += "};\n";

  asciiTiles = Image.open("../resources/tileset-32x32.bmp");

  emptyCell = Image.new("RGB", (32, 32), color="magenta");

  totalImages = len(images);

  print(f"Found { totalImages } files");

  atlasWidth  = asciiTiles.width;
  atlasHeight = asciiTiles.height + (
    32 if (totalImages <= 15) else ( 32 * ( (totalImages // 16) + 1 ) )
  );

  print(f"Atlas size: { atlasWidth } x { atlasHeight }");

  graphics = Image.new("RGB", (atlasWidth, atlasHeight), color='magenta');

  print("Pasting ASCII tiles...");

  graphics.paste(asciiTiles, (0, 0));

  print("Pasting graphics...");

  imgPosX = 0;
  imgPosY = asciiTiles.height;

  counter = 1;

  for path in images:
    print(f"Pasting file no. { counter } / { totalImages } ('{ path }')...");
    img = Image.open(path);
    graphics.paste(img, (imgPosX, imgPosY), img);
    imgPosX += 32;

    if (imgPosX >= atlasWidth):
      imgPosX = 0;
      imgPosY += 32;

    counter += 1;

  print("Pasting blank...");

  graphics.paste(emptyCell, (imgPosX, imgPosY));

  print("Saving...");

  graphics.save("graphic-tiles.bmp");

  print("Done!");

  print("");
  print("Replace corresponding enumeration in source code with this:");
  print("");
  print(cppEnum);

################################################################################

if __name__ == "__main__":
  main();
