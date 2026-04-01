#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <linux/input.h>

#define SDL_MAIN_HANDLED
#include "lvgl.h"
#include "examples/lv_examples.h"
#include "demos/lv_demos.h"


static int keyboard_fd;
enum {KEY_RELEASE, KEY_PRESS, KEY_REPEAT};
char char_keys[2][49] = {
    "1234567890-=qwertyuiop[]asdfghjkl;'`\\zxcvbnm,./<",
    "!@#$%^&*()_+QWERTYUIOP{}ASDFGHJKL:\"~|ZXCVBNM<>?>"
};
const char char_or_func[] =  // c = character key,
  "_fccccccccccccff"         // f = function key
  "ccccccccccccffcc"         // _ = blank/error ('_' is used, don't change)
  "ccccccccccfccccc"
  "ccccccffffffffff"
  "ffffffffffffffff"
  "ffff__cff_______"
  "ffffffffffffffff"
  "_______f_____fff";

int to_char_keys_index(unsigned int keycode)
{
    if (keycode >= KEY_1 && keycode <= KEY_EQUAL)  // keycodes 2-13: US keyboard: 1, 2, ..., 0, -, =
        return keycode - 2;
    if (keycode >= KEY_Q && keycode <= KEY_RIGHTBRACE)  // keycodes 16-27: q, w, ..., [, ]
        return keycode - 4;
    if (keycode >= KEY_A && keycode <= KEY_GRAVE)  // keycodes 30-41: a, s, ..., ', `
        return keycode - 6;
    if (keycode >= KEY_BACKSLASH && keycode <= KEY_SLASH)  // keycodes 43-53: \, z, ..., ., /
        return keycode - 7;
    if (keycode == KEY_102ND) return 47;  // key right to the left of 'Z' on US layout
    return -1;  // not character keycode
}

void keyboard_callback (lv_indev_t * indev, lv_indev_data_t * data)
{
    int char_index;
    struct input_event ev;
    static int last_state = LV_INDEV_STATE_PRESSED;

    if (last_state != LV_INDEV_STATE_RELEASED)
    {
        last_state = LV_INDEV_STATE_RELEASED;
        data->state = last_state;
        return;
    }

    if (keyboard_fd < 0) return;
    if (read(keyboard_fd, &ev, sizeof(ev)) != sizeof(ev)) return;
    if (ev.type != EV_KEY) return;

    static int shift = 0;
    if (ev.code == 42 || ev.code == 54)
    {
        if (shift == 0 && ev.value != KEY_RELEASE)
            shift = 1;

        if (shift == 1 && ev.value == KEY_RELEASE)
            shift = 0;
    }

    if (ev.value == KEY_PRESS) return;

    // if      (ev.code == 6) data->key = LV_KEY_NEXT;
    // else if (ev.code == 11) data->key = LV_KEY_PREV;
    // else if (ev.code == 2) data->key = '1';
    // else if (ev.code == 7) data->key = '2';
    // else if (ev.code == 12) data->key = '3';
    // else if (ev.code == 8) data->key = '4';
    // else if (ev.code == 13) data->key = '5';
    // else if (ev.code == 18) data->key = '6';
    // else if (ev.code == 9) data->key = '7';
    // else if (ev.code == 14) data->key = '8';
    // else if (ev.code == 19) data->key = '9';
    // else if (ev.code == 10) data->key = '0';
    // else if (ev.code == 5) data->key = ' ';
    // else if (ev.code == 20) data->key = LV_KEY_ENTER;
    // else if (ev.code == 4) data->key = LV_KEY_BACKSPACE;
    // else if (ev.code == 3) data->key = LV_KEY_ESC;
    // else return;

    if (ev.code >= sizeof(char_or_func)) return;
    // printf("type: %d, code: %d, value: %d\n", ev.type, ev.code, ev.value);
    if (char_or_func[ev.code] == 'c' && (char_index = to_char_keys_index(ev.code)) != -1)
      data->key = char_keys[shift][char_index];
    else if (ev.code == 1) data->key = LV_KEY_ESC;
    else if (ev.code == 14) data->key = LV_KEY_BACKSPACE;
    else if (ev.code == 28) data->key = LV_KEY_ENTER;
    else if (ev.code == 57) data->key = ' ';
    else if (ev.code == 102) data->key = LV_KEY_HOME;
    else if (ev.code == 103) data->key = LV_KEY_UP;
    else if (ev.code == 104) data->key = LV_KEY_PREV; // page up
    else if (ev.code == 105) data->key = LV_KEY_LEFT;
    else if (ev.code == 106) data->key = LV_KEY_RIGHT;
    else if (ev.code == 107) data->key = LV_KEY_END;
    else if (ev.code == 108) data->key = LV_KEY_DOWN;
    else if (ev.code == 109) data->key = LV_KEY_NEXT; // page down
    else if (ev.code == 111) data->key = LV_KEY_DEL;
    else return;

    last_state = LV_INDEV_STATE_PRESSED;
    data->state = last_state;
}

void ta_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);
    lv_obj_t * kb = lv_event_get_user_data(e);

    if (code == LV_EVENT_FOCUSED)
    {
        lv_keyboard_set_textarea(kb, ta);
        lv_obj_remove_flag(kb, LV_OBJ_FLAG_HIDDEN);
    }

    if (code == LV_EVENT_DEFOCUSED)
    {
        lv_keyboard_set_textarea(kb, NULL);
        lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
    }
}

int main(int argc, char * argv[], char * env[])
{
    // sleep(3);

    // keyboard_fd = open("/dev/input/event0", O_RDONLY | O_CLOEXEC);
    // if (keyboard_fd > 0)
    // {
    //     int fl = fcntl(keyboard_fd, F_GETFL, NULL);
    //     if (fl >= 0)
    //         fcntl(keyboard_fd, F_SETFL, fl | O_NONBLOCK);
    // }

    lv_init();

    lv_display_t * display = lv_sdl_window_create(800, 600);
    lv_indev_t * mouse = lv_sdl_mouse_create();
    lv_indev_t * mouse_wheel = lv_sdl_mousewheel_create();
    lv_indev_t * keyboard = lv_sdl_keyboard_create();

    // lv_font_t * font = lv_tiny_ttf_create_file("F:PYekan.ttf", 24);
    lv_font_t * font = lv_tiny_ttf_create_file("F:vazirmatn-fa.ttf", 24);

    lv_group_t * default_group = lv_group_create();
    lv_group_set_default(default_group);

    // lv_disp_t * display = lv_linux_fbdev_create();
    // lv_linux_fbdev_set_file(display, "/dev/fb0");
    // lv_display_set_default(display);
    // lv_display_set_rotation(display, LV_DISPLAY_ROTATION_90);
    // printf("w: %d, h:%d\n", lv_display_get_horizontal_resolution(display), lv_display_get_vertical_resolution(display));

    // lv_indev_t * keypad = lv_indev_create();
    // lv_indev_set_type(keypad, LV_INDEV_TYPE_KEYPAD);
    // lv_indev_set_read_cb(keypad, keyboard_callback);
    // lv_indev_set_group(keypad, lv_group_get_default());

    // lv_demo_benchmark();

    // lv_demo_music();
    // lv_demo_widgets();

    // lv_demo_keypad_encoder();

    lv_obj_t * ta = lv_textarea_create(lv_screen_active());
    lv_textarea_set_placeholder_text(ta, "سلام");
    lv_textarea_set_text(ta, "گچ پژ آلف");
    lv_obj_align(ta, LV_ALIGN_TOP_MID, 0, 10);
    lv_obj_set_size(ta, lv_pct(90), lv_pct(20));
    lv_textarea_set_align(ta, LV_TEXT_ALIGN_RIGHT);
    lv_obj_set_style_base_dir(ta, LV_BASE_DIR_RTL, 0);
    lv_obj_set_style_text_font(ta, font, 0);

    lv_obj_t * kb = lv_keyboard_create(lv_screen_active());
    lv_obj_set_style_text_font(kb, &lv_font_dejavu_16_persian_hebrew, 0);

    lv_obj_add_event_cb(ta, ta_event_cb, LV_EVENT_ALL, kb);
    lv_keyboard_set_textarea(kb, ta);

    while (1)
    {
        uint32_t time_till_next = lv_timer_handler();
        if(time_till_next == LV_NO_TIMER_READY)
            time_till_next = LV_DEF_REFR_PERIOD;
        lv_sleep_ms(time_till_next);
    }

    // lv_indev_delete(keypad);

    // lv_display_delete(display);

    lv_group_delete(default_group);

    lv_tiny_ttf_destroy(font);

    lv_sdl_quit();

    lv_deinit();

    if (keyboard_fd > 0) close(keyboard_fd);

    return 0;
}