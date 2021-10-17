from setuptools import setup, Extension

ext_queue = Extension(
    name="_queue",
    sources=["p3118helper/_queue.c"]
)

setup(
    name="P3118Helper",
    version="0.2.0rc0",
    install_requires=["aiogram"],
    packages=["p3118helper"],
    package_data={"p3118helper": ["py.typed", "*.pyi"]},
    url="https://github.com/LandgrafHomyak/P3118Helper-cpython",
    author="Andrew Golovashevich",
    python_requires=">=3.8",
    classifiers = [
        "Development Status :: 4 - Beta",
        "Framework :: AsyncIO",
        "Natural Language :: Russian",
        "Operating System :: OS Independent",
        "Programming Language :: C",
        "Programming Language :: Python :: 3 :: Only",
        "Programming Language :: Python :: 3.8",
        "Programming Language :: Python :: Implementation :: CPython",
        "Typing :: Typed"
    ],
    ext_package="p3118helper",
    ext_modules=[
        ext_queue
    ]
)