import itertools

import pytest

import camera


def test_projected_point():
    cam = camera.camera((4.0, 3.0, 2.0), (0.0, 0.0, 0.0), fov=45.0)

    viewport = (0, 0, 200, 200)

    for point in itertools.product([-1.0, 1.0], repeat=3):
        projected = cam.project(point, viewport)
        assert 0 < projected[0] < 200 and 0 < projected[1] < 200


def test_projected_point_after_turn():
    cam = camera.camera((4.0, 3.0, 2.0), (0.0, 0.0, 0.0), fov=45.0)
    cam.turn(30.0, 0.0)

    viewport = (0, 0, 200, 200)

    in_viewport = 0
    for point in itertools.product([-1.0, 1.0], repeat=3):
        projected = cam.project(point, viewport)
        if 0 < projected[0] < 200 and 0 < projected[1] < 200:
            in_viewport += 1

    assert in_viewport == 2


def test_projected_point_after_move():
    cam = camera.camera((4.0, 3.0, 2.0), (0.0, 0.0, 0.0), fov=45.0)
    cam.move(0.0, 0.0, 10.0)

    viewport = (0, 0, 200, 200)

    in_viewport = 0
    for point in itertools.product([-1.0, 1.0], repeat=3):
        projected = cam.project(point, viewport)
        if 0 < projected[0] < 200 and 0 < projected[1] < 200:
            in_viewport += 1

    assert in_viewport == 0


if __name__ == '__main__':
    pytest.main([__file__])
