from setuptools import setup, Extension

ext_queue = Extension(
    name="_queue",
    sources=["p3118helper/_queue.c"]
)

setup(
    name="P3118Helper",
    version="0.3.0rc0",
    install_requires=["aiogram>=2,<3"],
    packages=["p3118helper"],
    description="Telegram bot on aiogram for group P3118 in ITMO",
    package_data={"p3118helper": ["py.typed", "*.pyi"]},
    url="https://github.com/LandgrafHomyak/P3118Helper-cpython",
    author="Andrew Golovashevich",
    download_url="https://github.com/LandgrafHomyak/P3118Helper-cpython/releases/tag/v0.3.0rc0",
    python_requires=">=3.8, <3.10",
    classifiers = [
        "Development Status :: 4 - Beta",
        "Framework :: AsyncIO",
        "License :: OSI Approved :: MIT License",
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
    ],
    license="MIT",
    license_files=["LICENSE"],
)