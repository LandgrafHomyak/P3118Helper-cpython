from setuptools import setup, Extension

ext_queue = Extension(
    name="_queue",
    sources=["p3118helper/_queue.c"]
)

setup(
    name="P3118Helper",
    packages=["p3118helper"],
    version="0.2.0rc0",
    install_requires=["aiogram"],
    url="https://github.com/LandgrafHomyak/P3118Helper-cpython",
    author="Andrew Golovashevich",
    python_requires=">=3.8",
    classifiers = [
        "Development Status :: 5 - Production/Stable",
        "Framework :: AsyncIO",
        "Natural Language :: Russian",
        "Operating System :: OS Independent",
        "Programming Language :: Python :: 3 :: Only",
        "Programming Language :: Python :: 3.8",
        "Typing :: Typed"
    ],
    ext_package="p3118helper",
    ext_modules=[
        ext_queue
    ]
)