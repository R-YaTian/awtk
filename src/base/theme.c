﻿/**
 * File:   theme.c
 * Author: AWTK Develop Team
 * Brief:  theme manager
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
 * 2018-01-19 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "tkc/mem.h"
#include "tkc/utils.h"
#include "base/theme.h"
#include "tkc/buffer.h"

static theme_t* s_theme = NULL;

const uint8_t* theme_find_style(theme_t* theme, const char* widget_type, const char* name,
                                const char* widget_state) {
  return_value_if_fail(theme != NULL, NULL);
  if (theme->find_style != NULL) {
    return theme->find_style(theme, widget_type, name, widget_state);
  } else {
    return NULL;
  }
}

const char* theme_get_style_type(theme_t* theme) {
  return_value_if_fail(theme != NULL, NULL);
  if (theme->get_style_type != NULL) {
    return theme->get_style_type(theme);
  }

  return THEME_DEFAULT_STYLE_TYPE;
}

ret_t theme_set_theme_data(theme_t* theme, const uint8_t* data) {
  return_value_if_fail(theme != NULL, RET_BAD_PARAMS);

  if (theme->set_style_data != NULL) {
    return theme->set_style_data(theme, data);
  } else {
    if (theme->data != NULL && theme->need_free_data) {
      TKMEM_FREE(theme->data);
    }

    theme->data = data;
    theme->need_free_data = FALSE;
  }

  return RET_FAIL;
}

ret_t theme_destroy(theme_t* theme) {
  return_value_if_fail(theme != NULL, RET_BAD_PARAMS);
  
  if (theme == s_theme) {
    s_theme = NULL;
  }

  if (theme->need_free_data) {
    TKMEM_FREE(theme->data);
  }

  if (theme->theme_destroy != NULL) {
    theme->theme_destroy(theme);
  } else {
    TKMEM_FREE(theme);
  }

  return RET_OK;
}

/*global*/

theme_t* theme(void) {
  return s_theme;
}

ret_t theme_set(theme_t* theme) {
  if (theme == s_theme) {
    return RET_OK;
  }

  if (s_theme != NULL) {
    theme_destroy(s_theme);
  }

  s_theme = theme;

  return RET_OK;
}

#if !defined(AWTK_LITE)    
#include "theme_xml.h"
#endif/*AWTK_LITE*/   

#include "theme_default.h"

theme_t* theme_load_from_data(const char* name, const uint8_t* data, uint32_t size) {
  theme_t* t = NULL;
  theme_header_t* header = (theme_header_t*)data;
  return_value_if_fail(name != NULL && data != NULL, NULL);

  if (header->magic == THEME_MAGIC) {
    return theme_default_create(data);
#if !defined(AWTK_LITE)    
  } else if (*data == '<' || strstr(name, ".xml") != NULL) {
    return theme_xml_create((char*)data);
#endif/*AWTK_LITE*/   
  } else {
    return NULL;
  }

  return t;
}

#if !defined(AWTK_LITE)    
#include "theme_xml.inc"
#endif/*AWTK_LITE*/   

#include "theme_data.inc"
#include "theme_default.inc"

