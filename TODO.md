# General cleanups
* Give scenes names
* Clean Makefiles
* Add option to control rendering_params from the CLI

# Optimizations
* Cluster stars into bounded galaxies (using kd-tree or ball-tree)
* Change SDF to SDF + intersect (return inf if no intersection)
* Optimize sphere SDF with approximation removing the sqrt when possible
* Templatize SDFs (instead of using slow virutal functions)
* Profile and optimize
* Check if my short-circuit optimization even helping? (evaluating less SDFs but marching lower distances)
* Consider optimizing the return value from sdf() (e.g. make it into an argument, but remember * can't make this into SDF* and dist, as some materials are created as part of the sdf e.g. in smoothing)

# Features
* Simulate moving scenes (and consider lorentz transformations)
* Simulate scattering
* Add progress bar for post processing
* Add rotation sdf
* Experiment with variants on perlin noise
    * Add octaves
    * Change easing function (e.g. x^3 glued to itself rotated)
