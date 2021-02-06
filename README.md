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

1. Partition the **L x L** square into **N** non-overlapping rectangles where the width **w** and height **h** of each rectangle satisfies **max(w, h)>=2r**. To do this:
	1. Pick one of the remaining rectangels satisfying **w>=2r** and/or **h>=2r**.
	1. Split it along width or height, depending on whether **w>=2r** or **h>=2r**. If both **w>=2r** and **h>=2r**, randomly select the dimension to split along.
	1. Select the splitting point randomly (see below for details).
	1. Split and repeat with steps 1-3.
	
<img src="https://github.com/samurdhilbk/Drone-Placement/raw/master/plots/grid_only.png" width="200"> <img src="https://github.com/samurdhilbk/Drone-Placement/raw/master/plots/grid_plus_circles.png" width="200">

2. If less than **N** rectangles were found, then a valid solution does not exist.
3. If **N** rectangles were found, place a circle in each rectangle randomly. Since we ensured that each rectangle satisfied **max(w, h)>=2r**, this can always be done.

### Selecting a splitting point

Assume that the selected dimension to split a rectangle along has length **p** where **p>=2r**. The splitting point **q** is found as

**q=2r + TruncatedNormal(mean=0.5, variance=sigma^2, min=0, max=1) (p-2r)**

Here **TruncatedNormal** is the Truncated Normal Distribution (https://en.wikipedia.org/wiki/Truncated_normal_distribution), which is a Gaussian restricted to range **[min, max]**. Note that

1. When **sigma=0**, **TruncatedNormal(mean=0.5, variance=0, min=0, max=1)=0.5**, and hence the rectangle will always be split in half.
2. When **sigma -> infinity**, **TruncatedNormal** approaches a uniform distribution and there will be randomness in the splitting location.

**sigma=0** packs the circles tighter and has less randomness while **sigma -> infinity** packs the circles looser and has higher randomness. So to achieve the maximum possible randomness we choose the maximum **sigma** that allows to place all circles inside. This is done by using binary search.

## Using the code

Since standard C++ libraries do not include the capability to calculate , we use a libary developed by researchers at https://people.sc.fsu.edu/~jburkardt/cpp_src/truncated_normal/truncated_normal.html. This is reflected by `truncated_normal.cpp` and `truncated_normal.h`, and is the only dependency other than the standard  C++ libary.

The included CMAKE file could be helpful to compile the program.

### Setting parameters

Values for **N**,  **r** and **L** can be set in the `main()` as indicated function. 

### Parsing output

Output of the program can be printed to the console or be written to files, as controlled by in the main() function. The output files can be parsed by using the included Jupyter notebook `visualize_results.ipynb` to visualize the results.

## Visualization of Results



<img src="https://github.com/samurdhilbk/Drone-Placement/raw/master/plots/grid_plus_circles_200.png" width="200"> <img src="https://github.com/samurdhilbk/Drone-Placement/raw/master/plots/circles_only_200.png" width="200">

**L=200, r=1, N=1000 (run 1)**

<img src="https://github.com/samurdhilbk/Drone-Placement/raw/master/plots/grid_plus_circles_200_1.png" width="200"> <img src="https://github.com/samurdhilbk/Drone-Placement/raw/master/plots/circles_only_200_1.png" width="200">

**L=200, r=1, N=1000 (run 2)**

<img src="https://github.com/samurdhilbk/Drone-Placement/raw/master/plots/grid_only.png" width="200"> <img src="https://github.com/samurdhilbk/Drone-Placement/raw/master/plots/grid_plus_circles.png" width="200">

**L=8, r=2, N=10 (run 1)**

<img src="https://github.com/samurdhilbk/Drone-Placement/raw/master/plots/grid_only_1.png" width="200"> <img src="https://github.com/samurdhilbk/Drone-Placement/raw/master/plots/grid_plus_circles_1.png" width="200">
