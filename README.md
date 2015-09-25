# Prismoid

2d physics engine with fixed-orientation rigid bodies intended for side-scrolling platformers.

The primary goal of this physics engine is correctness: never let objects pass through walls or other objects.
For example, firing two bullets directly toward each other should cause the bullets to collide rather than pass through each other,
and a stack of boxes should never occupy less height than the sum of the heights of the boxes.
This physics engine approaches the issue by plotting the intersection of moving 2d objects as 3d objects in spacetime.
With this model collisions are intersections of prismoid polyhedra.

## Status

Not cool yet.

## Build

TODO
