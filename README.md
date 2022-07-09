# cheapest_route

A Dijkstra-based route-planner between two points on a grid.

This repo contains a utility application that can generate the cheapest route between two points A
and B, using a raster image as input. It is possible to write the resulting curve in SVG,
white-space separated values, or JSON format. Below is an example of a generated path on top of a
false-color representation with level curves of the input data.

![Example output](example_output.png)

The input is an image in EXR format, which may be a grayscale image, or an RGBA image. In the case
of a grayscale image, the Y component is used as a height-map. In the RGBA case, it the additional
channels gives control over pixel traversal cost, as well as the cost of traveling in a different
direction. In both cases, a regular Euclidian norm is used to measure the distance between two
points, that is

$$
\mathrm{d}s(\vec{r}) = \sqrt{(\mathrm{d} x)^2 + (\mathrm{d}y)^2 + (\mathrm{d}z)^2}
$$

For a grayscale image, the Y component is mapped to $z$, the elevation at the current pixel. The
program will minimize

$$
I = \mu_0 \int_{A}^{B} \mathrm{d}s(\vec{r}(t))
$$

with respect to $\vec{r}(t) = \vec{c_s}\odot(x(t), y(t), z(x(t), y(t)))$. The components of
$\vec{c_s}$, can be controlled from the command line, and affects the scale in the different
directions. If the $z$ component of
$\vec{c_s}$ is set to zero, the resulting path will be an
approximation of a straight line.

For a RGBA image, the R component takes the role of describing the elevation at the current pixel.
The G component is used for describing cost of traversing the current pixel. It can be thought of as
a local friction coefficient $\mu(x, y)$, It is possible to adjust its strength by the factor
$\mu_0$. The
components B and A are used to control the cost of traveling in different direction. It can be
thought a head-wind effect and is denoted $\vec{v}$. The effect of
$\vec{v}$ is controlled by a
constant $\vec{c_v}$. With this additional data, the program will minimize

$$
I = \int_{a}^{b}\left(\mu_0 \mu(x(t), y(t))\\, \mathrm{d}s(\vec{r}(t)) + |\vec{c_v}\odot\vec{v}(\vec{r}(t))\cdot\mathrm{d}\vec{r}(t)|\right)
$$

Notice that the vector part of the integral is over the absolute value of the projection between
$\mathrm{d}\vec{r}$ and
$\vec{c_v}\odot\vec{v}(\vec{r}(t))$. This means that it is not possible to simulate tailwind.

## Compiling and running

Install `maike2`, as well as the `OpenEXR` dev libraries. After running maike2, you can launch the
program by running

```
__targets/bin/cheapest_route help=
```