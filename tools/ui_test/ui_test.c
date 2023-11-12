/**
 * File:   ui_test.c
 * Author: AWTK Develop Team
 * Brief:  ui test
 *
 * Copyright (c) 2023 - 2023  Guangzhou ZHIYUAN Electronics Co.,Ltd.
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
 * 2023-11-04 Li XianJing <lixianjing@zlg.cn> created
 *
 */

#include "tkc.h"
#include "base/enums.h"
#include "base/events.h"
#include "base/widget_consts.h"

#include "conf_io/conf_ini.h"
#include "conf_io/conf_node.h"
#include "remote_ui/client/remote_ui.h"
#include "streams/stream_factory.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

static const char* fix_str(const char* str) {
  return str != NULL ? str : "";
}

static void check_return_code(ret_t ret, const char* expected_ret, const char* name,
                              const char* arg1, const char* arg2, const char* arg3) {
  if (expected_ret != NULL) {
    if (ret != ret_code_from_name(expected_ret)) {
      log_debug("%s failed(%s != %s)\n", name, ret_code_to_name(ret), expected_ret);
    } else {
      log_debug("%s (%s %s %s) ok\n", name, fix_str(arg1), fix_str(arg2), fix_str(arg3));
    }
  } else {
    if (ret == RET_OK) {
      log_debug("%s (%s %s %s) ok\n", name, fix_str(arg1), fix_str(arg2), fix_str(arg3));
    } else {
      log_debug("%s (%s %s %s) fail\n", name, fix_str(arg1), fix_str(arg2), fix_str(arg3));
    }
  }
}

static char* read_text_file(const char* name) {
  char filename[MAX_PATH + 1] = {0};
  path_prepend_app_root(filename, name);
  return (char*)file_read(filename, NULL);
}

static bool_t image_equal(const char* filename1, const char* filename2) {
  int x1 = 0;
  int y1 = 0;
  int n1 = 0;
  int x2 = 0;
  int y2 = 0;
  int n2 = 0;
  bool_t ret = FALSE;
  float* data1 = stbi_loadf(filename1, &x1, &y1, &n1, 0);
  float* data2 = stbi_loadf(filename2, &x2, &y2, &n2, 0);

  return_value_if_fail(data1 != NULL && data2 != NULL, FALSE);
  if (x1 != x2 || y1 != y2 || n1 != n2) {
    goto error;
  }

  if (memcmp(data1, data2, x1 * y1 * n1) != 0) {
    goto error;
  }

  ret = TRUE;
error:
  STBI_FREE(data1);
  STBI_FREE(data2);

  return ret;
}

static event_t* key_event_init_with_symbol(key_event_t* e, uint32_t type, const char* symbol) {
  const key_type_value_t* kv = keys_type_find(symbol);
  return_value_if_fail(kv != NULL, NULL);
  return key_event_init(e, type, NULL, kv->value);
}
static void run_script(conf_doc_t* doc, uint32_t times) {
  ret_t ret = RET_OK;
  remote_ui_t* ui = NULL;
  const char* expected_ret = NULL;
  conf_node_t* iter = conf_node_get_first_child(doc->root);

  while (iter != NULL) {
    const char* name = conf_node_get_name(iter);

    if (tk_str_eq(name, "create")) {
      const char* url = conf_node_get_child_value_str(iter, "url", "tcp://localhost:2233");
      tk_iostream_t* io = tk_stream_factory_create_iostream(url);

      break_if_fail(io != NULL);
      if (ui != NULL) {
        remote_ui_destroy(ui);
      }
      ui = remote_ui_create(io);
      iter = iter->next;
      continue;
    }

    if (ui == NULL) {
      log_debug("ui is null.\n");
      break;
    }

    expected_ret = conf_node_get_child_value_str(iter, "ret", NULL);
    if (tk_str_eq(name, "login")) {
      const char* user = conf_node_get_child_value_str(iter, "user", "admin");
      const char* password = conf_node_get_child_value_str(iter, "password", "admin");
      ret = remote_ui_login(ui, user, password);
      check_return_code(ret, expected_ret, name, user, NULL, NULL);
    } else if (tk_str_eq(name, "create_dir")) {
      const char* path = conf_node_get_child_value_str(iter, "path", NULL);
      ret = remote_ui_create_dir(ui, path);
      check_return_code(ret, expected_ret, name, path, NULL, NULL);
    } else if (tk_str_eq(name, "remove_dir")) {
      const char* path = conf_node_get_child_value_str(iter, "path", NULL);
      ret = remote_ui_remove_dir(ui, path);
      check_return_code(ret, expected_ret, name, path, NULL, NULL);
    } else if (tk_str_eq(name, "remove_file")) {
      const char* filename = conf_node_get_child_value_str(iter, "filename", NULL);
      ret = remote_ui_remove_file(ui, filename);
      check_return_code(ret, expected_ret, name, filename, NULL, NULL);
    } else if (tk_str_eq(name, "upload")) {
      const char* remote = conf_node_get_child_value_str(iter, "remote", NULL);
      const char* local = conf_node_get_child_value_str(iter, "local", NULL);
      ret = remote_ui_upload_file(ui, remote, local);
      check_return_code(ret, expected_ret, name, remote, local, NULL);
    } else if (tk_str_eq(name, "download")) {
      const char* remote = conf_node_get_child_value_str(iter, "remote", NULL);
      const char* local = conf_node_get_child_value_str(iter, "local", NULL);
      ret = remote_ui_download_file(ui, remote, local);
      check_return_code(ret, expected_ret, name, remote, local, NULL);
    } else if (tk_str_eq(name, "get_dev_info")) {
      remote_ui_dev_info_t info;
      ret = remote_ui_get_dev_info(ui, &info);
      check_return_code(ret, expected_ret, name, info.name, info.os, info.arch);
      log_debug("width=%d height=%d\n", info.screen_width, info.screen_height);
    } else if (tk_str_eq(name, "take_snapshot")) {
      char temp_file[MAX_PATH + 1] = {0};
      const char* target = conf_node_get_child_value_str(iter, "target", "");
      const char* filename = conf_node_get_child_value_str(iter, "filename", NULL);

      path_prepend_temp_path(temp_file, "snapshot.png");
      ret = remote_ui_take_snapshot(ui, target, temp_file);
      if (ret == RET_OK) {
        if (file_exist(filename)) {
          if (image_equal(temp_file, filename)) {
            ret = RET_OK;
            fs_remove_file(os_fs(), temp_file);
          } else {
            ret = RET_FAIL;
            log_debug("screen shot not equal:%s %s\n", temp_file, filename);
          }
        } else {
          log_debug("copy %s to %s\n", temp_file, filename);
          fs_copy_file(os_fs(), temp_file, filename);
          fs_remove_file(os_fs(), temp_file);
        }
      }
      check_return_code(ret, expected_ret, name, filename, NULL, NULL);
    } else if (tk_str_eq(name, "get_source")) {
      char temp_file[MAX_PATH + 1] = {0};
      const char* target = conf_node_get_child_value_str(iter, "target", "");
      const char* filename = conf_node_get_child_value_str(iter, "filename", NULL);

      path_prepend_temp_path(temp_file, "source.xml");
      ret = remote_ui_get_xml_source(ui, target, temp_file);
      if (ret == RET_OK) {
        if (file_exist(filename)) {
          if (fs_file_equal(os_fs(), temp_file, filename)) {
            ret = RET_OK;
            fs_remove_file(os_fs(), temp_file);
          } else {
            ret = RET_FAIL;
            log_debug("source not equal:%s %s\n", temp_file, filename);
          }
        } else {
          log_debug("copy %s to %s\n", temp_file, filename);
          fs_copy_file(os_fs(), temp_file, filename);
          fs_remove_file(os_fs(), temp_file);
        }
      }
      check_return_code(ret, expected_ret, name, filename, NULL, NULL);
    } else if (tk_str_eq(name, "get_manifest")) {
      const char* filename = conf_node_get_child_value_str(iter, "filename", "manifest.txt");
      ret = remote_ui_get_manifest(ui, filename);
      check_return_code(ret, expected_ret, name, filename, NULL, NULL);
    } else if (tk_str_eq(name, "get_loaded_images_info")) {
      const char* filename = conf_node_get_child_value_str(iter, "filename", NULL);
      ret = remote_ui_get_loaded_images_info(ui, filename);
      check_return_code(ret, expected_ret, name, filename, NULL, NULL);
    } else if (tk_str_eq(name, "get_loaded_assets_info")) {
      const char* filename = conf_node_get_child_value_str(iter, "filename", NULL);
      ret = remote_ui_get_loaded_assets_info(ui, filename);
      check_return_code(ret, expected_ret, name, filename, NULL, NULL);
    } else if (tk_str_eq(name, "open_window")) {
      const char* wname = conf_node_get_child_value_str(iter, "name", NULL);
      const char* xml = conf_node_get_child_value_str(iter, "xml", NULL);
      const char* init = conf_node_get_child_value_str(iter, "init", NULL);
      char* xml_content = NULL;
      char* init_content = NULL;
      if (xml != NULL && *xml == '@') {
        xml_content = read_text_file(xml + 1);
        xml = xml_content;
      }
      if (init != NULL && *init == '@') {
        init_content = read_text_file(init + 1);
        init = init_content;
      }
      ret = remote_ui_open_window(ui, wname, xml, init);
      check_return_code(ret, expected_ret, name, wname, NULL, NULL);
      if (xml_content != NULL) {
        TKMEM_FREE(xml_content);
      }
      if (init_content != NULL) {
        TKMEM_FREE(init_content);
      }
      sleep_ms(1000);
    } else if (tk_str_eq(name, "close_window")) {
      const char* wname = conf_node_get_child_value_str(iter, "name", NULL);
      ret = remote_ui_close_window(ui, wname);
      check_return_code(ret, expected_ret, name, wname, NULL, NULL);
      sleep_ms(1000);
    } else if (tk_str_eq(name, "create_widget")) {
      const char* target = conf_node_get_child_value_str(iter, "target", NULL);
      const char* xml = conf_node_get_child_value_str(iter, "xml", NULL);
      char* xml_content = NULL;
      if (xml != NULL && *xml == '@') {
        xml_content = read_text_file(xml + 1);
        xml = xml_content;
      }
      ret = remote_ui_create_widget(ui, target, xml);
      check_return_code(ret, expected_ret, name, target, NULL, NULL);
      if (xml_content != NULL) {
        TKMEM_FREE(xml_content);
      }
    } else if (tk_str_eq(name, "destroy_widget")) {
      const char* target = conf_node_get_child_value_str(iter, "target", NULL);
      ret = remote_ui_destroy_widget(ui, target);
      check_return_code(ret, expected_ret, name, target, NULL, NULL);
    } else if (tk_str_eq(name, "move_widget")) {
      const char* target = conf_node_get_child_value_str(iter, "target", NULL);
      int32_t x = conf_node_get_child_value_int32(iter, "x", 0);
      int32_t y = conf_node_get_child_value_int32(iter, "y", 0);
      ret = remote_ui_move_widget(ui, target, x, y);
      check_return_code(ret, expected_ret, name, target, NULL, NULL);
    } else if (tk_str_eq(name, "resize_widget")) {
      const char* target = conf_node_get_child_value_str(iter, "target", NULL);
      int32_t w = conf_node_get_child_value_int32(iter, "w", 0);
      int32_t h = conf_node_get_child_value_int32(iter, "h", 0);
      ret = remote_ui_resize_widget(ui, target, w, h);
      check_return_code(ret, expected_ret, name, target, NULL, NULL);
    } else if (tk_str_eq(name, "back")) {
      ret = remote_ui_back_to_prev(ui);
      check_return_code(ret, expected_ret, name, NULL, NULL, NULL);
      sleep_ms(1000);
    } else if (tk_str_eq(name, "home")) {
      ret = remote_ui_back_to_home(ui);
      check_return_code(ret, expected_ret, name, NULL, NULL, NULL);
      sleep_ms(1000);
    } else if (tk_str_eq(name, "logout")) {
      ret = remote_ui_logout(ui);
      check_return_code(ret, expected_ret, name, NULL, NULL, NULL);
    } else if (tk_str_eq(name, "set_prop")) {
      value_t v;
      const char* target = conf_node_get_child_value_str(iter, "target", NULL);
      const char* prop = conf_node_get_child_value_str(iter, "name", NULL);
      const char* value = conf_node_get_child_value_str(iter, "value", NULL);

      value_set_str(&v, value);
      ret = remote_ui_set_prop(ui, target, prop, &v);
      check_return_code(ret, expected_ret, name, target, prop, value);
    } else if (tk_str_eq(name, "get_prop")) {
      value_t v;
      const char* target = conf_node_get_child_value_str(iter, "target", NULL);
      const char* prop = conf_node_get_child_value_str(iter, "name", NULL);
      const char* value = conf_node_get_child_value_str(iter, "value", NULL);
      value_set_str(&v, NULL);
      ret = remote_ui_get_prop(ui, target, prop, &v);
      if (value != NULL) {
        if (!tk_str_eq(value, value_str(&v))) {
          ret = RET_FAIL;
        }
      }
      check_return_code(ret, expected_ret, name, target, prop, value_str(&v));
    } else if (tk_str_eq(name, "set_theme")) {
      const char* theme = conf_node_get_child_value_str(iter, "theme", NULL);
      ret = remote_ui_set_theme(ui, theme);
      check_return_code(ret, expected_ret, name, theme, NULL, NULL);
    } else if (tk_str_eq(name, "set_language")) {
      const char* language = conf_node_get_child_value_str(iter, "language", NULL);
      ret = remote_ui_set_language(ui, language);
      check_return_code(ret, expected_ret, name, language, NULL, NULL);
    } else if (tk_str_eq(name, "exec_fscript")) {
      str_t str;
      const char* fscript = conf_node_get_child_value_str(iter, "fscript", NULL);
      char* fscript_content = NULL;
      if (fscript != NULL && *fscript == '@') {
        fscript_content = read_text_file(fscript + 1);
        fscript = fscript_content;
      }
      str_init(&str, 1000);
      ret = remote_ui_exec_fscript(ui, fscript, &str);
      check_return_code(ret, expected_ret, name, fscript, str.str, NULL);
      str_reset(&str);
      if (fscript_content != NULL) {
        TKMEM_FREE(fscript_content);
      }
    } else if (tk_str_eq(name, "click")) {
      const char* target = conf_node_get_child_value_str(iter, "target", NULL);
      ret = remote_ui_click(ui, target);
      check_return_code(ret, expected_ret, name, target, NULL, NULL);
    } else if (tk_str_eq(name, "key")) {
      key_event_t event;
      const char* target = conf_node_get_child_value_str(iter, "target", NULL);
      const char* key = conf_node_get_child_value_str(iter, "key", NULL);
      key_event_init_with_symbol(&event, EVT_KEY_DOWN, key);
      ret = remote_ui_key(ui, target, event.key);
      check_return_code(ret, expected_ret, name, target, key, NULL);
    } else if (tk_str_eq(name, "send_event")) {
      event_t* e = NULL;
      const char* target = conf_node_get_child_value_str(iter, "target", NULL);
      const char* type = conf_node_get_child_value_str(iter, "type", NULL);
      const char* key = conf_node_get_child_value_str(iter, "key", NULL);
      int x = conf_node_get_child_value_int32(iter, "x", 0);
      int y = conf_node_get_child_value_int32(iter, "y", 0);
      break_if_fail(type != NULL);

      if (target == NULL) {
        target = STR_PROP_WINDOW;
      }

      if (strstr(type, "pointer") != NULL || strstr(type, "click") != NULL) {
        pointer_event_t event;
        if (strstr(type, "down") != NULL) {
          e = pointer_event_init(&event, EVT_POINTER_DOWN, NULL, x, y);
        } else if (strstr(type, "up") != NULL) {
          e = pointer_event_init(&event, EVT_POINTER_UP, NULL, x, y);
        } else if (strstr(type, "click") != NULL) {
          e = pointer_event_init(&event, EVT_CLICK, NULL, x, y);
        } else {
          e = pointer_event_init(&event, EVT_POINTER_MOVE, NULL, x, y);
        }
        ret = remote_ui_send_event(ui, target, e);
      } else if (strstr(type, "key") != NULL) {
        key_event_t event;
        if (strstr(type, "down") != NULL) {
          e = key_event_init_with_symbol(&event, EVT_KEY_DOWN, key);
        } else {
          e = key_event_init_with_symbol(&event, EVT_KEY_UP, key);
        }
        ret = remote_ui_send_event(ui, target, e);
      }
      check_return_code(ret, expected_ret, name, target, type, NULL);

      sleep_ms(100);
    } else if (tk_str_eq(name, "confirm")) {
      const char* title = conf_node_get_child_value_str(iter, "title", NULL);
      const char* content = conf_node_get_child_value_str(iter, "content", NULL);
      ret = remote_ui_show_confirm(ui, title, content);
      check_return_code(ret, expected_ret, name, title, content, NULL);
    } else if (tk_str_eq(name, "warn")) {
      const char* title = conf_node_get_child_value_str(iter, "title", NULL);
      const char* content = conf_node_get_child_value_str(iter, "content", NULL);
      ret = remote_ui_show_warn(ui, title, content);
      check_return_code(ret, expected_ret, name, title, content, NULL);
    } else if (tk_str_eq(name, "info")) {
      const char* title = conf_node_get_child_value_str(iter, "title", NULL);
      const char* content = conf_node_get_child_value_str(iter, "content", NULL);
      ret = remote_ui_show_info(ui, title, content);
      check_return_code(ret, expected_ret, name, title, content, NULL);
    } else if (tk_str_eq(name, "toast")) {
      const char* content = conf_node_get_child_value_str(iter, "content", NULL);
      int32_t duration = conf_node_get_child_value_int32(iter, "duration", 3000);
      ret = remote_ui_show_toast(ui, duration, content);
      check_return_code(ret, expected_ret, name, content, NULL, NULL);
    } else if (tk_str_eq(name, "sleep")) {
      int32_t time_ms = conf_node_get_child_value_int32(iter, "time", 1000);
      sleep_ms(time_ms);
    } else if (tk_str_eq(name, "close")) {
      remote_ui_destroy(ui);
      ui = NULL;
    }

    iter = iter->next;

    if (iter == NULL) {
      iter = conf_node_get_first_child(doc->root);
      times--;
      log_debug("=============%u===============\n", times);
    }

    if (times == 0) {
      break;
    }
  }

  if (ui != NULL) {
    remote_ui_destroy(ui);
    ui = NULL;
  }
}

#include "tkc/data_reader_factory.h"
#include "tkc/data_writer_factory.h"
#include "tkc/data_writer_file.h"
#include "tkc/data_writer_wbuffer.h"
#include "tkc/data_reader_file.h"
#include "tkc/data_reader_mem.h"

int main(int argc, char* argv[]) {
  char* data = NULL;
  conf_doc_t* doc = NULL;
  const char* input = argc > 1 ? argv[1] : "data/fs_default.ini";
  uint32_t times = argc > 2 ? tk_atoi(argv[2]) : 1;

  platform_prepare();
  data_writer_factory_set(data_writer_factory_create());
  data_reader_factory_set(data_reader_factory_create());
  data_writer_factory_register(data_writer_factory(), "file", data_writer_file_create);
  data_reader_factory_register(data_reader_factory(), "file", data_reader_file_create);
  data_reader_factory_register(data_reader_factory(), "mem", data_reader_mem_create);
  data_writer_factory_register(data_writer_factory(), "wbuffer", data_writer_wbuffer_create);

  if (argc < 2) {
    log_debug("Usage: %s config times\n", argv[0]);
    log_debug(" ex: %s data/tcp.ini\n", argv[0]);
    log_debug(" ex: %s data/tcp.ini 10\n", argv[0]);
    return 0;
  }

  tk_socket_init();

  data = (char*)file_read(input, NULL);
  if (data != NULL) {
    doc = conf_doc_load_ini(data);
    if (doc != NULL) {
      run_script(doc, times);
      conf_doc_destroy(doc);
    }
    TKMEM_FREE(data);
  }

  tk_socket_deinit();

  return 0;
}
