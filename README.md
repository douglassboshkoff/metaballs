# Metaballs

##DISCLAIMER
I did not write some of this code. It was provided as boilerplate code in my graphics class taught at Swarthmore. The main files that
my partner and I created/modified were circle.h, circle.cpp, mypanelopengl.h, mypanelopengl.cpp, and the fragment and vertex shaders in the shaders directory. I do 
not claim to know how cmake works, and some of the helper classes such as the printmatrix and mainwindow files were provided by our instructor. 

##Description
The aim of this project was to create amorphous two dimensional bodies that fluidly combine and separate. The bodies will bounce off walls and continuously collide with each other. To model this we will use the square marching algorithm. Square marching relies on a thresholding function to calculate the contributions of each circle in a two dimensional scalar field. We will use linear interpolation along with our thresholding function to ensure that each metaball has a smooth boundary.

Check out our final writeup and the metaballs demo video for more information. 

[![Metaballs Thumbnail](https://img.youtube.com/vi/9YuR5wZ8RPI/0.jpg)](https://www.youtube.com/watch?v=9YuR5wZ8RPI)

##References
We used [this blog]( http://jamie-wong.com/2014/08/19/metaballs-and-marching-squares/) as a guide to developing our program. 
