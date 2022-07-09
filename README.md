# cheapest_route

A Dijkstra-based route-planner between two points on a grid.

This repo contains a utility application that can generate the cheapest route between two points A
and B, using a raster image as input. It is possible to write the resulting curve in SVG,
white-space separated values, or JSON format. Below is an example of a generated path on top of a
false-color representation of the input data.

![Example output](example_output.png)

The input is an image in EXR format, which may be a grayscale image, or an RGBA image. In the case
of a grayscale image, the Y component is used as a height-map. In the RGBA case, it the additional
channels gives control over pixel traversal cost, as well as the cost of traveling in a different
direction.

For a grayscale image, the Y component is mapped to $z$, the elevation at the current pixel. The
program will minimize

$$
I = \int_{a}^{^b} \mathrm{d}s(\vec{r}(t))
$$
, with respect to $\vec{r}(t) = \vec{c}\odot(x(t), s_y y(t), z(x(t), y(t)))$. The components of
$\vec{c}$, can be controlled from the command line, and affects the scale in the different
directions.

For a RGBA image, the R component takes the role of describing the elevation at the current pixel.
The G component is used for describing cost of traversing the current pixel. It can be thought of as
a local friction coefficient

$$
I = \int_{a}^{b}\left(c(\vec{r})\\, \mathrm{d}s(x, y, z) + |\vec{v}(\vec{r})\cdot\\, \mathrm{d}\vec{r}|\right)
$$