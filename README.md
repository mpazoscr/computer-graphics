# computer-graphics
This repository contains a very basic set of libraries for math, rendering, physics and demo app making together with some demos implemented using those libraries.

Libraries: 
* mk-demofw: small framework to help create applications with a GL context.
* mk-math: set of useful math functions.
* mk-renderer: minimal OpenGL rendering library.
* mk-gpgpu: provides with a set of classes that assist in performing general purpose GPU programming using GLSL.
* mk-physics: library that provides with some physical simulation tools to simluate fluids.

##Ocean demo
Implementation of famous Jerry Tessendorf's paper for simulation of ocean surfaces. Thanks to [Themaister](https://github.com/Themaister/GLFFT) for his GLFFT implementation that makes it possible to perform an FFT in a GPU in a platform independent way. 

A video of the result can be seen [here](https://www.youtube.com/watch?v=SfT4pk3UfPE).

##Fluid demo
Toy app showing a two dimensional PIC\FLIP fluid solver implemented while working on my Master's thesis that is based on Robert Bridson's [excellent book](https://www.amazon.com/Simulation-Computer-Graphics-Robert-Bridson/dp/1568813260) on the subject.

The code for the [fluid solver](https://github.com/mpazoscr/computer-graphics/tree/master/mk-physics/src/physics/fluids) is old, it still needs some refactoring as well as parallelisation/optimisation.

A video of the result in its current state can be seen [here](https://www.youtube.com/watch?v=vBjliNlR5RE). Also, [here](https://github.com/mpazoscr/computer-graphics/blob/master/fluid-demo/doc/FluidSimulationThesis.pdf) is a link to my Master's thesis, in which the numerical methods used are explained in detail.

##Fluid benchmarks
Google benchmarks used while investigating optimisation options for the fluid solver mentioned above.

##License
The code in this repository is licensed under the [permissive MIT license](https://github.com/mpazoscr/computer-graphics/blob/master/LICENSE). Additionally, the following libraries are used (linked to their respective licensing models):
* [GLFFT](https://github.com/mpazoscr/computer-graphics/blob/master/mk-gpgpu/src/gpgpu/gl/GLFFT/LICENSE) 
* [GLFW](http://www.glfw.org/license.html)
* [Boost](http://www.boost.org/users/license.html)
* [GLEW](https://github.com/nigels-com/glew#copyright-and-licensing)
