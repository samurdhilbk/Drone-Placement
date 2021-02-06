# Drone Placement

#Objective

Design an algorithm to place 1000 2-dimensional drones (looking like a disc) randomly and without collision within a $$L \times L$$ square meter area.

#Input
$$N$$ - The number of drones (equi-radius circles)
$$L$$ - Length of each side of the square
$$r$$ - Radius of each drone (circle)

#Output

If a valid placement can be found, the coordinates of the center of each drone.  If not, denote that a valid placement cannot be found for the given input.

#Solution

1. Partition the $$ L \times L $$ square into N non-overlapping rectangles where the width $$w$$ and height $$h$$ of each rectangle satisfies $$max(w, h)>=2\times r$$. To do this:
	1. Pick a one of the remaining rectangels $$w>=2\times r$$ and/or $$h>=2\times r$$
	1. sadasd
