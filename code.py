from PIL import Image, ImageSequence

with Image.open("apng_test.png") as im:
    index = 1
    for frame in ImageSequence.Iterator(im):
        frame.save(f"frame{index}.png")
        index += 1