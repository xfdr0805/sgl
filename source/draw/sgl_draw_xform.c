/* source/draw/sgl_draw_xform.c
 *
 * MIT License
 *
 * Copyright(c) 2023-present All contributors of SGL  
 * Document reference link: https://sgl-docs.readthedocs.io
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <sgl_core.h>
#include <sgl_log.h>
#include <sgl_draw.h>
#include <sgl_math.h>
#include <string.h>


/**
 * @brief calculate a point color by bilinear interpolate
 * @param buffer point to 2x2 pixel matrix
 * @param w width of buffer
 * @param h height of buffer
 * @param fx x coordinate of point
 * @param fy y coordinate of point
 * @return point color
 */
sgl_color_t sgl_biln(const sgl_color_t *buffer, int16_t w, int16_t h, int32_t fx, int32_t fy)
{
    sgl_color_t ret;
    int32_t max_x = (((int32_t)w) - 1) << SGL_FIXED_SHIFT;
    int32_t max_y = (((int32_t)h) - 1) << SGL_FIXED_SHIFT;
    fx = fx < 0 ? 0 : (fx > max_x ? max_x : fx);
    fy = fy < 0 ? 0 : (fy > max_y ? max_y : fy);

    const int32_t x0 = fx >> SGL_FIXED_SHIFT;
    const int32_t y0 = fy >> SGL_FIXED_SHIFT;
    const int32_t dx = fx & SGL_FIXED_MASK;
    const int32_t dy = fy & SGL_FIXED_MASK;
    const int32_t dx1 = SGL_FIXED_ONE - dx;
    const int32_t dy1 = SGL_FIXED_ONE - dy;
    const int32_t point = (y0 * w) + x0;

    const sgl_color_t p00 = buffer[point];
    const sgl_color_t p01 = buffer[point + 1];
    const sgl_color_t p10 = buffer[point + w];
    const sgl_color_t p11 = buffer[point + w + 1];

    const uint8_t r00 = p00.ch.red;
    const uint8_t r01 = p01.ch.red;
    const uint8_t r10 = p10.ch.red;
    const uint8_t r11 = p11.ch.red;

    const uint8_t g00 = p00.ch.green;
    const uint8_t g01 = p01.ch.green;
    const uint8_t g10 = p10.ch.green;
    const uint8_t g11 = p11.ch.green;

    const uint8_t b00 = p00.ch.blue;
    const uint8_t b01 = p01.ch.blue;
    const uint8_t b10 = p10.ch.blue;
    const uint8_t b11 = p11.ch.blue;

    ret.ch.red = ((r00 * dx1 * dy1) + (r01 * dx * dy1) + (r10 * dx1 * dy) + (r11 * dx * dy)) >> (2 * SGL_FIXED_SHIFT);
    ret.ch.green = ((g00 * dx1 * dy1) + (g01 * dx * dy1) + (g10 * dx1 * dy) + (g11 * dx * dy)) >> (2 * SGL_FIXED_SHIFT);
    ret.ch.blue = ((b00 * dx1 * dy1) + (b01 * dx * dy1) + (b10 * dx1 * dy) + (b11 * dx * dy)) >> (2 * SGL_FIXED_SHIFT);

    return ret;
}
