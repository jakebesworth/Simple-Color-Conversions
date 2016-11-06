# Simple Color Conversions

## About

* Portable, simple, compact, written in pure C, two file color conversion library between color spaces, RGB to CYM(K) and HSV

* I could not find a c library with all the color conversion functions I wanted, there are snippets here and there on the internet, but nothing licensed and on Github
* If you have anymore conversions you want added such as HSI, XYZ, YUV, YU'V'... SMPTE-C, Kodak YCC... let me know

## How to use

* Include color.c and color.h into your project
* Header file explains everything, it's very basic and simple to use

```c
#include "color.h"

float h, s, v;

convertRGBtoHSV(147, 135, 95, &h, &s, &v);

// h = 46.1538
// s =  0.3537
// v =  0.5765
```

## License

* Licensed under the Apache License, Version 2.0, do what you want with it

## Resources

* [Formulas derived from "Color Space Conversions" by Adrian Ford and Alan Roberts](http://sites.biology.duke.edu/johnsenlab/pdfs/tech/colorconversion.pdf)

## Contributing

* Need help testing the algorithms to see if they produce proper results. Done hundreds of manual tests, something like an O(n^3) to test to and fro between conversions as well

* Need help implementing the other color space conversions (maybe CMY -> HSV through the RGB functions) such as XYZ

* Need help testing portability and how casting and cheap ceiling implementation work on other systems and compilers

## Contributors

* [Jake Besworth](https://github.com/jakebesworth)
