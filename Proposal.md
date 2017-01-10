# Final Project Proposal

 The aim of this project is to create amorphous three dimensional bodies that fluidly combine and separate. The bodies will bounce off walls and continuously collide with each other. To model this we will use the cube marching algorithm. Cube marching relies on a thresholding function to calculate the contributions of each sphere in a three dimensional scalar field. We will use linear interpolation along with our thresholding function to ensure that each meatball has a smooth boundary. Furthermore, we aim to implement a phong lighting model to illuminate our three dimensional metaballs.

# Course Concepts Used
The core of this lab relies on the usage of opengl and qt to draw complex shapes. We will need to determine what calculations need to take place in the vertex and fragment shaders. Most likely we will use per-vertex lighting since it is less computationally expensive. We will also need to do boundary checking for each sphere.

The biggest new concept that we are tackling is representing shapes that are complex, not just simple polygons.

# Goals
###Short Term
The first goal will be to fully understand how the square marching algorithm works. We will then investigate how we can take this algorithm to three dimensions in the form of cube marching. We then implement the two dimensional version of metaballs.
###Long Term
We want to bring create three dimensional metaballs that smoothly collide and separate. Additionally we would like to use phong lighting on each vertex.

All the spheres will be contained within a cube. We will calculate the collisions of the spheres with the sides of the cube.
###Stretch Goals
Create a lava lamp structure and give the metaballs behavior of lava. That is the metaballs will heat and rise to the top of a container and join to "hotter" metaballs. We would also be able to rotate the camera around to view the lava lamp from different perspectives similar to the solar system lab.

A final stretch goal is to implement a button in our application to toggle between the two dimensional and the three dimensional metaballs for demo purposes.

# References
1. http://jamie-wong.com/2014/08/19/metaballs-and-marching-squares/
2. http://www.siggraph.org/education/materials/HyperGraph/modeling/metaballs/metaballs_mward.html
3. https://en.wikipedia.org/wiki/Metaballs
4. http://users.polytech.unice.fr/~lingrand/MarchingCubes/algo.html

# Assessment

 What would make your project a success?
 We want to have a full understanding of how the marching cubes algorithm works. We will consider our project successful if we are able to generate a three dimensional version of metaballs using cube marching.
