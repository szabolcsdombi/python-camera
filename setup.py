from setuptools import Extension, setup

ext = Extension(
    name='camera',
    sources=['camera.cpp'],
)

setup(
    name='camera',
    version='0.1.0',
    ext_modules=[ext],
)
