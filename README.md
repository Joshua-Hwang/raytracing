This repo walks through the instructions defined in [Ray Tracing in One
Weekend](https://raytracing.github.io/books/RayTracingInOneWeekend.html)
and [Ray Tracing: The Next
Week](https://raytracing.github.io/books/RayTracingTheNextWeek.html)

Instead of the PPM image format used in the instructions I have opted
to use [lodepng](https://github.com/lvandeve/lodepng).

The dependency can be obtained after cloning via
```
git submodule init
git submodule update
```

Enter any directory and run `make` to create the program.
Run `./a.out` to run the program and create the .png file.

Changes in `main` determine when I create a new folder.

The directories are prepended with the book they came from.
