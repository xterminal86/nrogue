#!/usr/bin/python3

import argparse;

from pathlib import Path;

from PIL import Image;

def main():
  parser = argparse.ArgumentParser();

  parser.add_argument(
    "SKINFILE",
    type=str,
    help="Minecraft character skin as png"
  );

  args = parser.parse_args();

  fname = args.SKINFILE;

  fnameResult = Path(fname).stem;

  try:
    img = Image.open(fname);
  except Exception as e:
    print(f"{ e }");
    exit(1);

  resultImageFace = Image.new("RGBA", (32, 32));
  resultImage = Image.new("RGBA", (32, 32));

  head     = img.crop((8, 8, 16, 16));
  torso    = img.crop((20, 20, 28, 32));
  armLeft  = img.crop((44, 22, 48, 32));
  armRight = armLeft.transpose(Image.Transpose.FLIP_LEFT_RIGHT);
  legLeft  = img.crop((4, 20, 8, 32));
  legRight = legLeft.transpose(Image.Transpose.FLIP_LEFT_RIGHT);

  resultImage.paste(head, (12, 0));
  resultImage.paste(torso, (12, 8));
  resultImage.paste(armLeft, (8, 8));
  resultImage.paste(armRight, (20, 8));
  resultImage.paste(legLeft, (12, 20));
  resultImage.paste(legRight, (16, 20));

  resultImage.save(f"{ fnameResult }.png");

  headFull = head.resize((32, 32), Image.Resampling.NEAREST);
  resultImageFace.paste(headFull);
  resultImageFace.save(f"{ fnameResult }-head.png");

  img.close();

  print("OK!");

if __name__ == "__main__":
  main();
