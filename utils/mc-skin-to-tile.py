#!/usr/bin/python3

import argparse;

from pathlib import Path;

from PIL import Image, ImageOps;

def main():
  parser = argparse.ArgumentParser();

  parser.add_argument(
    "SKINFILE",
    type=str,
    help="Minecraft character skin as png"
  );

  parser.add_argument("--head-acc", action="store_true");

  args = parser.parse_args();

  fname   = args.SKINFILE;
  headAcc = args.head_acc;

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

  headAccessory = img.crop((40, 8, 48, 16));

  resultImage.paste(head, (12, 0));

  if headAcc:
    resultImage.paste(headAccessory, (12, 0), mask=headAccessory);

  resultImage.paste(torso, (12, 8));
  resultImage.paste(armLeft, (8, 8));
  resultImage.paste(armRight, (20, 8));
  resultImage.paste(legLeft, (12, 20));
  resultImage.paste(legRight, (16, 20));

  resultImage.save(f"{ fnameResult }.png");

  img.close();

  print("OK!");

if __name__ == "__main__":
  main();
