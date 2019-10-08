# General cleanups
* Clean Makefiles
* Add animator object
* Give scenes names
* Clean up scenes
* Add more c files?
* Add option to control rendering_params from the CLI
* Clean up main
* Clean up star scenes (split to files)
* Fix progress bar for animations

# Optimizations
* Change SDF to SDF + intersect (return inf if no intersection)
* Optimize sphere SDF with approximation removing the sqrt when possible
* Templatize SDFs (instead of using slow virutal functions)
* Profile and optimize
* Check if my short-circuit optimization even helping? (evaluating less SDFs but marching lower distances)
* Consider optimizing the return value from sdf() (e.g. make it into an argument, but remember * can't make this into SDF* and dist, as some materials are created as part of the sdf e.g. in smoothing)

# Features
* Simulate moving scenes (and consider lorentz transformations)
* Simulate scattering
* Add colors to light sources
* Add progress bar for post processing
* Add progress bar for frames when rendering animations
* Add rotation sdf
* Experiment with variants on perlin noise
    * Add octaves
    * Change easing function (e.g. x^3 glued to itself rotated)
* Add counter groups

