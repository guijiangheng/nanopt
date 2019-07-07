# NanoPT: a tiny c++ path tracer

![fireplace-room](./gallery/fireplace-room.png)
![fireplace-room](./gallery/ao.png)

Nanopt is a tiny c++ path tracer written for learning Matt Pharr's book *" Physically Based Rendering: From Theory to Implementation"*. Most of code and ideas comes from PBRT, but with a lot simplification.

## Building NanoPT

To check out nanopt together with all dependencies, be sure to use the `--recursive` flag when cloning the repository, i.e.

```bash
$ git clone --recursive https://github.com/guijiangheng/nanopt.git
```

If you accidentally already cloned nanopt without this flag (or to update an nanopt source tree after a new submodule has been added, run the following command to also fetch the dependencies:

```bash
$ git submodule update --init --recursive
```

NanoPT uses [cmake](http://www.cmake.org/) for its build system. After clone the source code, use below commnads to start compling:

```bash
mkdir build && cd build
cmake ..
make -j4
```
Or you can open repository root folder directly with visual studio 2019.

## Demo Scene

Example scenes can be obtained from [Morgan McGuire's Computer Graphics Archive](http://casual-effects.com/data/).

## Author

GuiJiangHeng [Twitter](https://twitter.com/guijiangheng)
