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
#ifndef TK_ROTATE_IMAGE_BGR565_H
#define TK_ROTATE_IMAGE_BGR565_H

#include "base/bitmap.h"

ret_t rotate_bgr565_image(bitmap_t* fb, bitmap_t* img, const rect_t* src, lcd_orientation_t o);

#endif /*TK_ROTATE_IMAGE_BGR565_H*/
