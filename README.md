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
2. If less than **N** rectangles were found, then a valid solution does not exist.
3. If **N** rectangles were found, place a circle in each rectangle randomly. Since we ensured that each rectangle satisfied this can always be done.

### Selecting a splitting point

Assume that the selected dimension to split a rectangle along has length **p** where **p>=2r**. The splitting point **q** is found as

**q=2r + TruncatedNormal(mean=0.5, variance=sigma^2, min=0, max=1).**

Here TruncatedNormal is the Truncated Normal Distribution (https://en.wikipedia.org/wiki/Truncated_normal_distribution), which is a Gaussian restricted to range **[min, max]**. Note that

1. When **sigma=0**, **TruncatedNormal(mean=0.5, variance=0, min=0, max=1)=0.5** and hence the rectangle will always be split in half.
2. When **sigma -> infinity**, **TruncatedNormal** approaches a uniform distribution and there will be randomness in the splitting location.

**sigma=0** packs the circles tighter and has less randomness while **sigma -> infinity** packs the circles looser and has higher randomness. So to achieve the maximum possible randomness we choose the maximum that allows to all circles inside. This is done by using binary search.
