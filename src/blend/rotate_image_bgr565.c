﻿/**
 * File:   rotate_image_bgr565.c
 * Author: AWTK Develop Team
 * Brief:  rotate on bgr565
 *
 * Copyright (c) 2018 - 2022  Guangzhou ZHIYUAN Electronics Co.,Ltd.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * License file for more details.
 *
 */

/**
 * History:
 * ================================================================
 * 2018-10-10 Generated by gen.sh(DONT MODIFY IT)
 *
 */
#include "tkc/rect.h"
#include "base/pixel.h"
#include "base/bitmap.h"
#include "base/pixel_pack_unpack.h"

#define pixel_dst_t pixel_bgr565_t
#define pixel_dst_format pixel_bgr565_format
#define pixel_dst_to_rgba pixel_bgr565_to_rgba
#define pixel_dst_from_rgb pixel_bgr565_from_rgb

#include "rotate_image.inc"

ret_t rotate_bgr565_image(bitmap_t* fb, bitmap_t* img, const rect_t* src, lcd_orientation_t o) {
  return rotate_image(fb, img, src, o);
}
