/*
 * Simple-color-conversions
 *
 * Copyright 2016 Jake Besworth
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif

#ifndef MAX
#define MAX(a,b) (((a)>(b))?(a):(b))
#endif

#ifndef MAX3
#define MAX3(a,b,c) MAX(MAX(a,b),c)
#endif

#ifndef MIN3
#define MIN3(a,b,c) MIN(MIN(a,b),c)
#endif

void convertCMYtoRGB(const float c, const float m, const float y, unsigned char * r, unsigned char * g, unsigned char * b)
{
    *r = (1.0 - c) * 255.0;
    *g = (1.0 - m) * 255.0;
    *b = (1.0 - y) * 255.0;
}

void convertRGBtoCMY(const unsigned char r, const unsigned char g, const unsigned char b, float * c, float * m, float * y)
{
    *c = 1.0 - (r / 255.0);
    *m = 1.0 - (g / 255.0);
    *y = 1.0 - (b / 255.0);
}

void convertCMYtoCMYK(float * c, float * m, float * y, float * k)
{
    *k = MIN3(*c, *m, *y);
    *c = (*c - *k) / (1.0 - *k);
    *m = (*m - *k) / (1.0 - *k);
    *y = (*y - *k) / (1.0 - *k);

}

void convertCMYKtoCMY(float * c, float * m, float * y, const float k)
{
    *c = MIN(1.0, (*c * (1.0 - k)) + k);
    *m = MIN(1.0, (*m * (1.0 - k)) + k);
    *y = MIN(1.0, (*y * (1.0 - k)) + k);
}

void convertCMYKtoRGB(const float c, const float m, const float y, const float k, unsigned char * r, unsigned char * g, unsigned char * b)
{
    float c2 = c;
    float m2 = m;
    float y2 = y;
    float k2 = k;

    convertCMYKtoCMY(&c2, &m2, &y2, k2);
    convertCMYtoRGB(c2, m2, y2, r, g, b);
}

void convertRGBtoCMYK(const unsigned char r, const unsigned char g, const unsigned char b, float * c, float * m, float * y, float * k)
{
    convertRGBtoCMY(r, g, b, c, m, y);
    convertCMYtoCMYK(c, m, y, k);
}

void convertHSVtoRGB(const float h, const float s, const float v, unsigned char * r, unsigned char * g, unsigned char * b)
{
    /* Convert hue back to 0-6 space, floor */
    const float hex = h / 60.0;
    const unsigned char primary = (int) hex;
    const float secondary = hex - primary;

    /* a, b, c */
    const float x = (1.0 - s) * v;
    const float y = (1.0 - (s * secondary)) * v;
    const float z = (1.0 - (s * (1.0 - secondary))) * v;

    if(primary == 0)
    {
        /* 0: R = v, G = c, B = a */
        *r = v;
        *g = z;
        *b = x;
    }
    else if(primary == 1)
    {
        /* 1: R = b, G = v, B = a */
        *r = y;
        *g = v;
        *b = x;
    }
    else if(primary == 2)
    {
        /* 2: R = a, G = v, B = c */
        *r = x;
        *g = v;
        *b = z;
    }
    else if(primary == 3)
    {
        /* 3: R = a, G = b, B = v */
        *r = x;
        *g = y;
        *b = v;
    }
    else if(primary == 4)
    {
        /* 4: R = c, G = a, B = v */
        *r = z;
        *g = x;
        *b = v;
    }
    else if(primary == 5)
    {
        /* 5: R = v, G = a, B = b */
        *r = v;
        *g = x;
        *b = y;
    }
}

void convertRGBtoHSV(const unsigned char r, const unsigned char g, const unsigned char b, float * h, float * s, float * v)
{
    const unsigned char max = MAX3(r, g, b);
    const unsigned char min = MIN3(r, g, b);

    *s = (max - min) / max;
    *v = max;

    /* Saturation is 0 */
    if((*s * 100.0) < 0.1)
    {
        /* Hue is undefined, monochrome */
        *h = 0;
        return;
    }
    else if(r == max && g == min)
    {
        /* H = 5 + B' */
        *h = 5 + (max - b) / (max - min);
    }
    else if(r == max && g != min)
    {
        /* H = 1 - G' */
        *h = 1 - (max - g) / (max - min);
    }
    else if(g == max && b == min)
    {
        /* H = R' + 1 */
        *h = (max - r) / (max - min) + 1;
    }
    else if(g == max && b != min)
    {
        /* H = 3 - B' */
        *h = 3 - (max - b) / (max - min);
    }
    else if(r == max)
    {
        /* H = 3 + G' */
        *h = 3 + (max - g) / (max - min);
    }
    else
    {
        /* H = 5 - R' */
        *h = 5 - (max - r) / (max - min);
    }

    /* Hue is then converted to degrees by multiplying by 60 */
    *h = MIN(*h * 60, 360);
}
