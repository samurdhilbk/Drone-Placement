# Drone Placement

## Objective

Design an algorithm to place 1000 2-dimensional drones (looking like a disc) randomly and without collision within a **L x L** square meter area.

## Input
- **N** - The number of drones (equi-radius circles)
- **L** - Length of each side of the square
- **r** - Radius of each drone (circle)

## Output

If a valid placement can be found, the coordinates of the center of each drone.  If not, denote that a valid placement cannot be found for the given input.

## Solution

1. Partition the ** L x L ** square into **N** non-overlapping rectangles where the width **w** and height **h** of each rectangle satisfies **max(w, h)>=2r**. To do this:
	1. Pick a one of the remaining rectangels satisfying **w>=2r** and/or **h>=2r**
	1. Split it along width or height depending on whether **w>=2r** or **h>=2r**. If both **w>=2r** and **h>=2r**, randomly select the dimension to split along.
	1. Select the splitting point randomly (see below for details)
	1. Splt and repeat with steps 1-3
