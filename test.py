import itertools

from PIL import Image, ImageDraw

import camera

camera = camera.camera((4.0, 3.0, 2.0), (0.0, 0.0, 0.0), fov=45.0)

img = Image.new('RGB', (200, 200), '#fff')
draw = ImageDraw.Draw(img)

vp = (0, 0, 200, 200)

for a, b in itertools.product(itertools.product([-1, 1], repeat=3), itertools.product([-1, 1], repeat=3)):
    if sum(x == y for x, y in zip(a, b)) == 2:
        draw.line([camera.project(a, vp)[:2], camera.project(b, vp)[:2]], '#000')

img.transpose(Image.FLIP_TOP_BOTTOM).show()
