# General cleanups
* give scenes names
* clean Makefiles
* Add option to render number of iterations per ray (instead of color)
* make colors 0-1 instead of 0-255

# Optimizations
* Cluster stars into bounded galaxies
* Optimize sphere SDF with approximation removing the sqrt when possible
* templatize SDFs (instead of using slow virutal functions)
* profile and optimize
* if my short-circuit optimization even helping? (evaluating less SDFs but marching lower distances)
* consider optimizing the return value from sdf() (e.g. make it into an argument, but remember * can't make this into SDF* and dist, as some materials are created as part of the sdf e.g. in smoothing)

# Features
* simulate moving scenes (and consider lorentz transformations)
* simulate scattering
* add progress bar for post processing
* add rotation sdf
* experiment with variants on perlin noise
    * add octaves
    * change easing function (e.g. x^3 glued to itself rotated)
