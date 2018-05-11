
from PIL import Image
import numpy
import imageio
import os

filenames = list()

for file in os.listdir("build/frames/"):
    if file.endswith(".png"):
        filenames.append("build/frames/"+file)


filenames.sort()


with imageio.get_writer('movie.gif', mode='I', fps=60, subrectangles=True) as writer:
    for filename in filenames:
        img = Image.open(filename).resize((450,450))
        writer.append_data(numpy.array(img))
