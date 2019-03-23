import itertools

import numpy as np
import pytest

import camera


def test_move():
    cam = camera.camera((4.0, 3.0, 2.0), (0.0, 0.0, 0.0), fov=45.0)
    np.testing.assert_almost_equal(cam.position, (4.0, 3.0, 2.0))

    cam.move(0.0, 0.0, 3.0)
    np.testing.assert_almost_equal(cam.position, (4.0, 3.0, 5.0))

    cam.move(1.0, 0.0, 0.0)
    np.testing.assert_almost_equal(cam.position, (3.25, 2.44, 4.62), decimal=1)


if __name__ == '__main__':
    pytest.main([__file__])
