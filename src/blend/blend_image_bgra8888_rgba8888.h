﻿/**
 * File:   blend_image_bgra8888_rgba8888.c
 * Author: AWTK Develop Team
 * Brief:  blend rgba8888 on bgra8888
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
 * 2022-02-24 Generated by gen.sh(DONT MODIFY IT)
 *
 */
#ifndef TK_BLEND_IMAGE_BGRA8888_RGBA8888_H
#define TK_BLEND_IMAGE_BGRA8888_RGBA8888_H

#include "base/bitmap.h"

ret_t blend_image_bgra8888_rgba8888(bitmap_t* dst, bitmap_t* src, const rectf_t* dst_r, const rectf_t* src_r, uint8_t a);

ret_t blend_image_rotate_bgra8888_rgba8888(bitmap_t* dst, bitmap_t* src, const rectf_t* dst_r, const rectf_t* src_r, uint8_t a, lcd_orientation_t o);

#endif /*TK_BLEND_IMAGE_BGRA8888_RGBA8888_H*/
