import imageio
import glob, os

def getnum(f):
    return int(''.join(x for x in f if str.isdigit(x)))

with imageio.get_writer('output/movie.mp4', mode='I', fps=24) as writer:
    for filename in sorted(glob.glob("output/output*.ppm"), key=getnum):
        print(filename)
        image = imageio.imread(filename)
        writer.append_data(image)
