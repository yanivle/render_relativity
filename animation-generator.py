import imageio
import glob, os

with imageio.get_writer('output/movie.mp4', mode='I') as writer:
    for filename in sorted(glob.glob("output/output*.ppm")):
        image = imageio.imread(filename)
        writer.append_data(image)
