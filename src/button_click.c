#include <pebble.h>
#include <stdlib.h>
#include <stdbool.h>
  
static Window *window;
static TextLayer *text_layer;
static TextLayer *instr_text_layer;
static TextLayer *score_text_layer;
static int counter;
static int round_counter;
static int score;
static AppTimer *app_timer;
static bool stop;
static bool endGame;

static void hard_reset(void){
  counter = 0;
  round_counter = 0;
  score = 0;
  stop = true;
}

char *itoa(int num) {
  static char buff[20] = {};
  int i = 0, temp_num = num, length = 0;
  char *string = buff;
  if(num >= 0) {
    // count how many characters in the number
    while(temp_num) {
      temp_num /= 10;
      length++;
    }
    // assign the number to the buffer starting at the end of the 
    // number and going to the begining since we are doing the
    // integer to character conversion on the last number in the
    // sequence
    for(i = 0; i < length; i++) {
      buff[(length-1)-i] = '0' + (num % 10);
      num /= 10;
    }
    buff[i] = '\0'; // can't forget the null byte to properly end our string
    }
  else{
    return "Unsupported Number";
  }
  return string;
}

char *itoa2(int num) {
  static char buff[20] = {};
  int i = 0, temp_num = num, length = 0;
  char *string = buff;
  if(num >= 0) {
    // count how many characters in the number
    while(temp_num) {
      temp_num /= 10;
      length++;
    }
    // assign the number to the buffer starting at the end of the 
    // number and going to the begining since we are doing the
    // integer to character conversion on the last number in the
    // sequence
    for(i = 0; i < length; i++) {
      buff[(length-1)-i] = '0' + (num % 10);
      num /= 10;
    }
    buff[i] = '\0'; // can't forget the null byte to properly end our string
    }
  else{
    return "Unsupported Number";
  }
  return string;
}

static void appTimerCallback(void *data){
  if(!stop){
    counter++;
    text_layer_set_text(text_layer, itoa(counter));
    app_timer = app_timer_register(1, appTimerCallback, window);
  }
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context){
  if(endGame){
    hard_reset();
    endGame = false;
    text_layer_set_text(text_layer, "Press Down");
    text_layer_set_text(score_text_layer, "0");
  }
  else{
    if(round_counter > 0){
      int tscore = counter - 3333;
      score += (tscore > 0) ? tscore : -tscore;
      text_layer_set_text(score_text_layer, itoa2(score));
      round_counter++;
      if(round_counter < 4){
        counter = 0;
        app_timer = app_timer_register(1, appTimerCallback, window);
      }
      else {
        stop = true;
        text_layer_set_text(text_layer, "Final Score:");
        text_layer_set_text(score_text_layer, itoa2(score));
        endGame = true;
      }
    }
    else {
      stop = false;
      round_counter++;
      app_timer = app_timer_register(1, appTimerCallback, window);
    }
  }
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  GFont titleFont = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  GFont otherFont = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
  
  text_layer = text_layer_create((GRect) { .origin = { 0, 60}, .size = { bounds.size.w, 60} });
  text_layer_set_font(text_layer, otherFont);
  text_layer_set_text(text_layer, "Press Down");
  
  instr_text_layer = text_layer_create((GRect){.origin = {0,10}, .size= {bounds.size.w, 60}});
  text_layer_set_font(instr_text_layer, titleFont);
  text_layer_set_text(instr_text_layer, "Get Close to 3333");
  
  score_text_layer = text_layer_create((GRect){.origin = {0,100}, .size= {bounds.size.w, 30}});
  text_layer_set_font(score_text_layer, otherFont);
  text_layer_set_text(score_text_layer, "0");
  
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_layer));
  
  text_layer_set_text_alignment(instr_text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(instr_text_layer));
  
  text_layer_set_text_alignment(score_text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(score_text_layer));
}

static void window_unload(Window *window) {
  app_timer_cancel(app_timer);
  text_layer_destroy(text_layer);
  text_layer_destroy(instr_text_layer);
  text_layer_destroy(score_text_layer);
}

static void init(void) {
  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
	.load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  hard_reset();
  endGame = false;
  window_stack_push(window, animated);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}




