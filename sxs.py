from PIL import Image, ImageDraw, ImageFont
import sys

import argparse


def centerText(draw, txt, container_width, font):
  txt_width, _ = draw.textsize(txt, font=font)
  return (container_width - txt_width) // 2


def square_iterator(x, y, width):
  for dy in range(-width, width + 1):
    for dx in range(-width, width + 1):
      yield x + dx, y + dy


def drawText(img,
             txt,
             x,
             container_width,
             font,
             padding_bottom,
             outline_width,
             outlineColor='black',
             fillColor='white'):
  draw = ImageDraw.Draw(img)
  center_x = x + centerText(draw, txt, container_width, font)
  center_y = img.size[1] - padding_bottom

  for x, y in square_iterator(center_x, center_y, outline_width):
    draw.text((x, y), txt, font=font, fill=outlineColor)
  draw.text((center_x, center_y), txt, font=font, fill=fillColor)


parser = argparse.ArgumentParser()
parser.add_argument('images', nargs='+', help='the images')
parser.add_argument('--labels',
                    help='the comma-separated labels for the images')
parser.add_argument('--padding', type=int, default=30)
parser.add_argument('--padding_bottom', type=int, default=20)
parser.add_argument('--font_size', type=int, default=32)
parser.add_argument('--outline_width', type=int, default=1)
parser.add_argument('--out', default='sxs.png')

args = parser.parse_args()

labels = [''] * len(args.images)
if args.labels:
  labels = args.labels.split(',')

assert len(args.images) >= 2
assert len(args.images) == len(labels)

images = []
width = 0
height = 0
for image_name in args.images:
  image = Image.open(image_name)
  width += image.size[0]
  height = max(height, image.size[1])
  images.append(image)

sxs = Image.new('RGB', (width + args.padding * (len(args.images) - 1), height),
                (255, 255, 255))

pos = 0
for i, image in enumerate(images):
  sxs.paste(image, (pos, 0))
  drawText(sxs, labels[i], pos, image.size[0],
           ImageFont.truetype('/Library/Fonts/Arial.ttf', args.font_size),
           args.padding_bottom, args.outline_width)
  pos += image.size[0] + args.padding

sxs.save(args.out)
sxs.show()
