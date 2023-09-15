from moviepy import editor
import numpy as np
import os
from PIL import Image
import re

def extract_blinks(af, cutoff):
    eye_open = False
    saved_frames = []
    for frame in af:
        md = np.median(frame,(0,1))[1]
        green = md > cutoff

        if eye_open and not green:
            eye_open = False
            yield frame
        elif not eye_open and green:
            eye_open = True
            if len(saved_frames) > 0:
                yield saved_frames[len(saved_frames)//2]
                saved_frames = []
            yield frame
        elif not eye_open:
            saved_frames.append(frame)


def main(cutoff: int = 30):
    ptrn = re.compile(r"(.*)\.[^\.]*$")
    for filename in os.listdir("input"):
        i = 0
        for frame in extract_blinks(editor.VideoFileClip(f"input/{filename}").iter_frames(),cutoff):
            im = Image.fromarray(frame)
            base_name = ptrn.match(filename).group(1)
            im.save(f"output/{base_name}_{i}.bmp")
            i += 1
            print(i)

if __name__ == "__main__":
    main()

