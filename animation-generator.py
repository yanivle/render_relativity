#!/usr/bin/env python3

import imageio
import glob, os
import sys
import progressbar

def getnum(f):
    return int(''.join(x for x in f if str.isdigit(x)))

with imageio.get_writer('output/movie.mp4', mode='I', fps=24) as writer:
    for filename in progressbar.progressbar(sorted(glob.glob("output/output*.ppm"), key=getnum)):
        image = imageio.imread(filename)
        writer.append_data(image)
