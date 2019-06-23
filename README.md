# NanoPT: A tiny c++ path tracer

![fireplace-room](./gallery/fireplace-room.png)

Nanopt is a tiny c++ path tracer written for learning Matt Pharr's book *" Physically Based Rendering: From Theory to Implementation"*. Most of ideas and code comes from PBRT source code, but with a lot simplification. Currently it only compiles and works correctly on linux system.

## Building NanoPT

To check out pbrt together with all dependencies, be sure to use the
`--recursive` flag when cloning the repository, i.e.
```bash
$ git clone --recursive https://github.com/guijiangheng/nanopt.git
```
If you accidentally already cloned pbrt without this flag (or to update an
pbrt source tree after a new submodule has been added, run the following
command to also fetch the dependencies:
```bash
$ git submodule update --init --recursive
```

NanoPT uses [cmake](http://www.cmake.org/) for its build system. After clone the source code, use below commnads to start compling:
```bash
mkdir buld && cd build
cmake ..
make -j4
```

## Demo Scene

Example scenes can be obtained from [Morgan McGuire's Computer Graphics Archive](http://casual-effects.com/data/).

## Author

GuiJiangHeng [Twitter](https://twitter.com/guijiangheng)
