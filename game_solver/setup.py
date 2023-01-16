from setuptools import Extension, setup

setup(
    name="hardcoresolver",
    version="1",
    ext_modules=[
        Extension("_hardcoresolver", sources=[
            "hardcoresolver.c",
            "testhardcoresolver.c",
            "pytypes.c",
            "solvep.c"
        ], extra_compile_args=[ "-O3" ])
    ]
)
