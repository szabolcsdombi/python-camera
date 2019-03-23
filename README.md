# python-camera

A simple camera implementation for python.

## Cheat Sheet

### import

```py
import camera
```

### create a camera

```py
cam = camera.camera(position=(4.0, 3.0, 2.0), target=(0.0, 0.0, 0.0), fov=45.0)
# fov is given in degrees
```

### moving around

```py
cam.move(forward=1.5, left=0.0, up=0.0)

cam.turn(left=0.1, up=0.0)
# left and up are given in radians

cam.focus(target=(0.0, 0.0, 1.0))
```

### rendering compatible matrix

```py
mat = cam.matrix(ratio, near, far)
# mat is a bytes object (a 4x4 matrix of floats)
```

### project

```py
pt = cam.project(point=(0.0, 0.0, 0.0), viewport=(0, 0, 100, 100), near=0.1, far=100.0)
# pt is a point in screen coordinates, the z value is the depth
```

### unproject

```py
pt = cam.unproject(point=(50.0, 50.0, 0.5), viewport=(0, 0, 100, 100), near=0.1, far=100.0)
# pt is a point in world coordinates
```

### interpolation

```py
cam = camera.interpolate(camera1=cam1, camera2=cam2, coeff=0.5)
```
