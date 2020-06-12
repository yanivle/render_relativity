#!/usr/bin/env python3

import imageio
import glob, os
import sys
import progressbar

output_movie_file = 'seam_carving/pagoda2_movie.mp4'
input_files_glob = "seam_carving/pagoda2_frame*.ppm"
# output_movie_file = 'output/movie.mp4'
# input_files_glob = "output/output*.ppm"


def getnum(f):
  return int(''.join(x for x in f if str.isdigit(x)))


with imageio.get_writer(output_movie_file, mode='I', fps=24) as writer:
  for filename in progressbar.progressbar(
      sorted(glob.glob(input_files_glob), key=getnum)):
    image = imageio.imread(filename)
    writer.append_data(image)
