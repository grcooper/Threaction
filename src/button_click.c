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

static void appTimerCallback(void *data){
  counter++;
  char *buffer;
  buffer = itoa(counter/10);
  
  text_layer_set_text(text_layer, buffer);
  if(!stop){
    app_timer = app_timer_register(1, appTimerCallback, window);
  } 
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  if(stop){
    stop = false;
    round_counter++;
    app_timer = app_timer_register(1, appTimerCallback, window);
  }
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  if(!stop){
    if(round_counter != 0){
      score += abs((counter/10) - 333);
      char *s_buffer;
      s_buffer = itoa(score);
      text_layer_set_text(score_text_layer, s_buffer);
    }
    if(round_counter < 4){
      round_counter++;
      counter = 0;
      app_timer = app_timer_register(1, appTimerCallback, window);
    }
    else {
      char *score_buffer;
      score_buffer = itoa(score);
      text_layer_set_text(score_text_layer, score_buffer);
      APP_LOG(APP_LOG_LEVEL_INFO, "Score buffer should be here");
      hard_reset();
    }
  }
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context){
  hard_reset();
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  text_layer = text_layer_create((GRect) { .origin = { 0, 72 }, .size = { bounds.size.w, 20 } });
  text_layer_set_text(text_layer, "Press UP to Start");
  
  instr_text_layer = text_layer_create((GRect){.origin = {0,0}, .size= {bounds.size.w, 20}});
  text_layer_set_text(instr_text_layer, "Get Close to 333");
  
  score_text_layer = text_layer_create((GRect){.origin = {0,100}, .size= {bounds.size.w, 20}});
  text_layer_set_text(score_text_layer, "SCORE HERE");
  
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