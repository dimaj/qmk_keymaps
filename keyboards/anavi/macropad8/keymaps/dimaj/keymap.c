/* Copyright 2021 Leon Anavi <leon@anavi.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include QMK_KEYBOARD_H

enum layers {
    _GIT = 0,
    _LEDS,
    _TEAMS,
    _SLACK,
    _FN = 15
};
// #define _GIT 0
// #define _LEDS 1
// #define _TEAMS 2
// #define _FN 10


enum custom_keycodes {
	GITCOMMIT = SAFE_RANGE,
	GITPUSH,
	GITPULL,
	GITSTATUS,
	GITDIFF,
	GITLOG,
	GITADD,
    SLACK_ACTIVE,
    SLACK_AWAY
};


enum teams_keycodes {
    TOGGLE_MUTE = LGUI(LSFT(KC_M)),
    TOGGLE_VIDEO = LGUI(LSFT(KC_O)),
    TOGGLE_RAISE_HAND = LGUI(LSFT(KC_K)),
    END_VIDEO_CALL = LGUI(LSFT(KC_B)),
    ACCEPT_VIDEO_CALL = LGUI(LSFT(KC_A))
};

enum slack_keycodes {
    ALL_UNREAD = LGUI(LSFT(KC_A)),
    THREADS = LGUI(LSFT(KC_T)),
    DMS = LGUI(LSFT(KC_K)),
};

#ifdef RGBLIGHT_LAYERS
// define layers for underglow
const rgblight_segment_t PROGMEM git_layer[] = RGBLIGHT_LAYER_SEGMENTS(
    {0, 4, HSV_RED} // light all LEDs RED
);
const rgblight_segment_t PROGMEM layer_layer[] = RGBLIGHT_LAYER_SEGMENTS(
    {0, 4, HSV_YELLOW} // light all LEDs RED
);
const rgblight_segment_t PROGMEM teams_layer[] = RGBLIGHT_LAYER_SEGMENTS(
    {0, 4, HSV_BLUE} // light all LEDs RED
);
const rgblight_segment_t PROGMEM slack_layer[] = RGBLIGHT_LAYER_SEGMENTS(
    {0, 4, HSV_WHITE} // light all LEDs RED
);
const rgblight_segment_t PROGMEM leds_layer[] = RGBLIGHT_LAYER_SEGMENTS(
    {0, 1, HSV_RED},
    {1, 1, HSV_YELLOW},
    {2, 1, HSV_BLUE},
    {3, 1, HSV_GREEN}
);
// end layer underglow

const rgblight_segment_t* const PROGMEM rgb_layers[] = RGBLIGHT_LAYERS_LIST(
    git_layer,
    leds_layer,
    teams_layer,
    slack_layer,
    layer_layer
);
void keyboard_post_init_user(void) {
    // Enable LED layers
    rgblight_layers = rgb_layers;
}

bool led_update_user(led_t led_state) {
    rgblight_set_layer_state(_GIT, false);
    rgblight_set_layer_state(_LEDS, false);
    rgblight_set_layer_state(_TEAMS, false);
    rgblight_set_layer_state(_SLACK, false);
    rgblight_set_layer_state(_FN, false);

    rgblight_set_layer_state(get_highest_layer(layer_state), true);
    return true;
}
#endif

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    static uint16_t hold_timer;

    bool is_hold;
    if (record->event.pressed) {
        hold_timer = timer_read();
        return true;
    } else {
        is_hold = timer_elapsed(hold_timer) > TAPPING_TERM;
    }
    switch (keycode) {
        case GITCOMMIT:
            SEND_STRING("git commit -s\n");
            break;
        case GITPUSH:
            if (is_hold) {
                SEND_STRING("git pull-request ");
            } else {
                SEND_STRING("git push\n");
            }
            break;
        case GITPULL:
            SEND_STRING("git pull\n");
            break;
        case GITSTATUS:
            SEND_STRING("git status\n");
            break;
        case GITDIFF:
            SEND_STRING("git difftool\n");
            break;
        case GITLOG:
            SEND_STRING("git log\n");
            break;
        case GITADD:
            SEND_STRING("git add ");
            break;
        case SLACK_AWAY:
            SEND_STRING("/away");
            _delay_ms(500);
            SEND_STRING("\n");
            break;
        case SLACK_ACTIVE:
            SEND_STRING("/active");
            _delay_ms(500);
            SEND_STRING("\n");
            break;
    }
	return true;
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  /* Layer 0: Git
  *           Short press                              Long press
  * /--------+--------+--------+--------\   /--------+--------+--------+--------\
  * | status |  log   |  pull  |  push  |   |        |        |        |   pr   |
  * |--------`--------`--------`--------|   |--------`--------`--------`--------|
  * |  diff  |   add  | commit |   FN   |   |        |        |        |        |
  * \--------+--------+--------+--------/   \--------+--------+--------+--------/
  */
  [_GIT] = LAYOUT_ortho_2x4(
     GITSTATUS, GITLOG, GITPULL, GITPUSH,
     GITDIFF, GITADD, GITCOMMIT, MO(_FN)
  ),

  /* Layer 15: Layers
  *           Short press                              Long press
  * /--------+--------+--------+--------\   /--------+--------+--------+--------\
  * |  Git   |  Teams |  Slack |        |   |        |        |        |        |
  * |--------`--------`--------`--------|   |--------`--------`--------`--------|
  * |        |        | LEDs   |   FN   |   |        |        |        |        |
  * \--------+--------+--------+--------/   \--------+--------+--------+--------/
  */
  [_FN] = LAYOUT_ortho_2x4(
    TO(_GIT), TO(_TEAMS), TO(_SLACK), _______,
    _______, _______, TO(_LEDS), _______
  ),

  /* Layer 1: LEDs
  * Top Row    : RGB - Underglow
  * Bottom Row : Backlight - Keys
  *           Short press                              Long press
  * /--------+--------+--------+--------\   /--------+--------+--------+--------\
  * | Toggle |  Mode  | Rainbow| Snake  |   |        |        |        |        |
  * |--------`--------`--------`--------|   |--------`--------`--------`--------|
  * | Toggle | Bright | Breath |   FN   |   |        |        |        |        |
  * \--------+--------+--------+--------/   \--------+--------+--------+--------/
  */
  [_LEDS] = LAYOUT_ortho_2x4(
     RGB_TOG, RGB_MOD, RGB_M_R, RGB_M_SN,
     BL_TOGG, BL_STEP, BL_BRTG, MO(_FN)
  ),

  /* Layer 2: Microsoft Teams
  *           Short press                              Long press
  * /--------+--------+--------+--------\   /--------+--------+--------+--------\
  * | Un/Mute| V. Tog |  Hand  |        |   |        |        |        |        |
  * |--------`--------`--------`--------|   |--------`--------`--------`--------|
  * |  Answ  |  End   |        |   FN   |   |        |        |        |        |
  * \--------+--------+--------+--------/   \--------+--------+--------+--------/
  */
  [_TEAMS] = LAYOUT_ortho_2x4(
     TOGGLE_MUTE, TOGGLE_VIDEO, TOGGLE_RAISE_HAND, _______,
     ACCEPT_VIDEO_CALL, END_VIDEO_CALL, _______, MO(_FN)
  ),

  /* Layer 3: Slack
  *           Short press                              Long press
  * /--------+--------+--------+--------\   /--------+--------+--------+--------\
  * | Active | Away   | Un/Mute| V. Tog |   |        |        |        |        |
  * |--------`--------`--------`--------|   |--------`--------`--------`--------|
  * | Unread |  DMs   | Threads|   FN   |   |        |        |        |        |
  * \--------+--------+--------+--------/   \--------+--------+--------+--------/
  */
  [_SLACK] = LAYOUT_ortho_2x4(
     SLACK_ACTIVE, SLACK_AWAY, KC_M, KC_V,
     ALL_UNREAD, DMS, THREADS, MO(_FN)
  )
};

#ifdef OLED_DRIVER_ENABLE
oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    return OLED_ROTATION_180;  // flips the display 180 degrees if offhand
}

void oled_task_user(void) {
    // oled_scroll_left();
    // max length - 21 chars

  oled_write_P(PSTR("Active layer: "), false);

  switch (get_highest_layer(layer_state)) {
    case _GIT:
      oled_write_ln_P(PSTR("Git"), false);
      oled_write_ln_P(PSTR("ST | LOG | PUL | PUS"), false);
      oled_write_ln_P(PSTR("DF | ADD | COM | FN"), false);
      oled_write_ln_P(PSTR("--------------------"), false);
      oled_write_ln_P(PSTR("__ | __ | __ | PR"), false);
      oled_write_ln_P(PSTR("__ | __ | __ | __"), false);
      break;
    case _FN:
      oled_write_ln_P(PSTR("Layers"), false);
      oled_write_ln_P(PSTR("Git | Team |Slack|"), false);
      oled_write_ln_P(PSTR("__ | __ | LEDs | FN"), false);
      oled_write_ln_P(PSTR(""), false);
      oled_write_ln_P(PSTR(""), false);
      oled_write_ln_P(PSTR(""), false);
      break;
    case _LEDS:
      oled_write_ln_P(PSTR("LEDs"), false);
      oled_write_ln_P(PSTR("TOG | MODE | R | SN"), false);
      oled_write_ln_P(PSTR("TOG | STP | BRT | FN"), false);
    //   oled_write_ln_P(PSTR("--------------------"), false);
    //   oled_write_ln_P(PSTR("__ | __ | __ | PR"), false);
    //   oled_write_ln_P(PSTR("__ | __ | __ | __"), false);
      oled_write_ln_P(PSTR(""), false);
      oled_write_ln_P(PSTR(""), false);
      oled_write_ln_P(PSTR(""), false);
      break;
    case _TEAMS:
      oled_write_ln_P(PSTR("Teams"), false);
      oled_write_ln_P(PSTR("Mic | Cam | Hnd | __"), false);
      oled_write_ln_P(PSTR("Answ | End | __ | FN"), false);
    //   oled_write_ln_P(PSTR("--------------------"), false);
    //   oled_write_ln_P(PSTR("__ | __ | __ | PR"), false);
    //   oled_write_ln_P(PSTR("__ | __ | __ | __"), false);
      oled_write_ln_P(PSTR(""), false);
      oled_write_ln_P(PSTR(""), false);
      oled_write_ln_P(PSTR(""), false);
      break;
    case _SLACK:
      oled_write_ln_P(PSTR("Slack"), false);
      oled_write_ln_P(PSTR("Act | Aw | Mic | Cam"), false);
      oled_write_ln_P(PSTR("All | DMs | Thr | FN"), false);
    //oled_write_ln_P(PSTR("--------------------"), false);
    //   oled_write_ln_P(PSTR("__ | __ | __ | PR"), false);
    //   oled_write_ln_P(PSTR("__ | __ | __ | __"), false);
      oled_write_ln_P(PSTR(""), false);
      oled_write_ln_P(PSTR(""), false);
      oled_write_ln_P(PSTR(""), false);
      break;
    default:
      // Or use the write_ln shortcut over adding '\n' to the end of your string
      oled_write_ln_P(PSTR("N/A"), false);
  }
  oled_write_ln_P(PSTR(" "), false);
  oled_write_P(PSTR("Backlit: "), false);
  oled_write_ln_P(is_backlight_enabled() ? PSTR("On") : PSTR("Off"), false);
// #ifdef RGBLIGHT_ENABLE
//   static char rgbStatusLine1[26] = {0};
//   snprintf(rgbStatusLine1, sizeof(rgbStatusLine1), "RGB Mode: %d", rgblight_get_mode());
//   oled_write_ln(rgbStatusLine1, false);
//   static char rgbStatusLine2[26] = {0};
//   snprintf(rgbStatusLine2, sizeof(rgbStatusLine2), "h:%d s:%d v:%d", rgblight_get_hue(), rgblight_get_sat(), rgblight_get_val());
//   oled_write_ln(rgbStatusLine2, false);
// #endif
}
#endif
