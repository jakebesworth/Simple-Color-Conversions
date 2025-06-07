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
    /* Perform a cheap ceiling because e.g. it returns 9.99~ which truncates to 9 */
    *r = ((1.0 - c) * 255.0) + 0.5;
    *g = ((1.0 - m) * 255.0) + 0.5;
    *b = ((1.0 - y) * 255.0) + 0.5;
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
    *c = ((int) *k == 1) ? 1 : (*c - *k) / (1.0 - *k);
    *m = ((int) *k == 1) ? 1 : (*m - *k) / (1.0 - *k);
    *y = ((int) *k == 1) ? 1 : (*y - *k) / (1.0 - *k);

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

    /* 360 hue (same as 0 hue) is not accounted for in the original paper.
     * Other implementations of HSV to RGB use modulo instead of a division
     * where 0 and 6 both result in primary == 0. I've fixed it here. */
    if(primary == 0 || primary == 6)
    {
        /* 0: R = v, G = c, B = a */
        *r = (v * 255.0) + 0.5;
        *g = (z * 255.0) + 0.5;
        *b = (x * 255.0) + 0.5;
    }
    else if(primary == 1)
    {
        /* 1: R = b, G = v, B = a */
        *r = (y * 255.0) + 0.5;
        *g = (v * 255.0) + 0.5;
        *b = (x * 255.0) + 0.5;
    }
    else if(primary == 2)
    {
        /* 2: R = a, G = v, B = c */
        *r = (x * 255.0) + 0.5;
        *g = (v * 255.0) + 0.5;
        *b = (z * 255.0) + 0.5;
    }
    else if(primary == 3)
    {
        /* 3: R = a, G = b, B = v */
        *r = (x * 255.0) + 0.5;
        *g = (y * 255.0) + 0.5;
        *b = (v * 255.0) + 0.5;
    }
    else if(primary == 4)
    {
        /* 4: R = c, G = a, B = v */
        *r = (z * 255.0) + 0.5;
        *g = (x * 255.0) + 0.5;
        *b = (v * 255.0) + 0.5;
    }
    else if(primary == 5)
    {
        /* 5: R = v, G = a, B = b */
        *r = (v * 255.0) + 0.5;
        *g = (x * 255.0) + 0.5;
        *b = (y * 255.0) + 0.5;
    }
}

void convertRGBtoHSV(const unsigned char r, const unsigned char g, const unsigned char b, float * h, float * s, float * v)
{
    const unsigned char max = MAX3(r, g, b);
    const float max2 = max / 255.0;
    const unsigned char min = MIN3(r, g, b);
    const float min2 = min / 255.0;

    *s = (max2 < 0.0001) ? 0 : (max2 - min2) / max2;
    *v = max2;

    /* Saturation is 0 */
    if((*s * 100.0) < 0.1)
    {
        /* Hue is undefined, monochrome */
        *h = 0;
        return;
    }
    else if(r == max)
    {
        if(g == min)
        {
            /* H = 5 + B' */
            *h = 5 + ((max2 - (b / 255.0)) / (max2 - min2));
        }
        else
        {
            /* H = 1 - G' */
            *h = 1 - ((max2 - (g / 255.0)) / (max2 - min2));
        }
    }
    else if(g == max)
    {
        if(b == min)
        {
            /* H = R' + 1 */
            *h = ((max2 - (r / 255.0)) / (max2 - min2)) + 1;
        }
        else
        {
            /* H = 3 - B' */
            *h = 3 - ((max2 - (b / 255.0)) / (max2 - min2));
        }
    }
    /* This is actually a problem with the original paper, I've fixed it here */
    else if(b == max && r == min)
    {
        /* H = 3 + G' */
        *h = 3 + ((max2 - (g / 255.0)) / (max2 - min2));
    }
    else
    {
        /* H = 5 - R' */
        *h = 5 - ((max2 - (r / 255.0)) / (max2 - min2));
    }

    /* Hue is then converted to degrees by multiplying by 60 */
    *h = MIN(*h * 60, 360);
}
