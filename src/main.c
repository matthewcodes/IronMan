#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_time_layer;
static GFont s_time_font;

static BitmapLayer *s_face_layer;
static GBitmap *s_face_bitmap;

static BitmapLayer *s_top_layer_white;
static BitmapLayer *s_top_layer_black;
static GBitmap *s_top_bitmap_white;
static GBitmap *s_top_bitmap_black;

static BitmapLayer *s_eyes_layer_white;
static BitmapLayer *s_eyes_layer_black;
static GBitmap *s_eyes_bitmap_white;
static GBitmap *s_eyes_bitmap_black;

static PropertyAnimation *face_property_animation;

static void open_animation_stopped(Animation *animation, bool finished, void *data) {
  
  if(finished) {
    property_animation_destroy(face_property_animation);
  }
}

static void show_eyes() {
  // Eyes
  s_eyes_bitmap_white = gbitmap_create_with_resource(RESOURCE_ID_EYES_WHITE);
  s_eyes_bitmap_black = gbitmap_create_with_resource(RESOURCE_ID_EYES_BLACK);
  
  s_eyes_layer_white = bitmap_layer_create(GRect(30, 73, 83, 15));
  bitmap_layer_set_bitmap(s_eyes_layer_white, s_eyes_bitmap_white);
  bitmap_layer_set_compositing_mode(s_eyes_layer_white, GCompOpOr);
  layer_add_child(window_get_root_layer(s_main_window), bitmap_layer_get_layer(s_eyes_layer_white));
  
  s_eyes_layer_black = bitmap_layer_create(GRect(30, 73, 83, 15));
  bitmap_layer_set_bitmap(s_eyes_layer_black, s_eyes_bitmap_black);
  bitmap_layer_set_compositing_mode(s_eyes_layer_black, GCompOpClear);
  layer_add_child(window_get_root_layer(s_main_window), bitmap_layer_get_layer(s_eyes_layer_black));
}

static void hide_eyes() {
  
  layer_remove_from_parent(bitmap_layer_get_layer(s_eyes_layer_white));
  layer_remove_from_parent(bitmap_layer_get_layer(s_eyes_layer_black));
  
  bitmap_layer_destroy(s_eyes_layer_black);
  bitmap_layer_destroy(s_eyes_layer_white);

  gbitmap_destroy(s_eyes_bitmap_white);
  gbitmap_destroy(s_eyes_bitmap_black);
  
}

static void open_animation_started(Animation *animation, void *context) {
  hide_eyes();
}

static void open_mask() {

  Layer* mask_layer = bitmap_layer_get_layer(s_face_layer);
  GRect from_frame = layer_get_frame(mask_layer);
  GRect to_frame = GRect(0, -80, 144, 143);
  
  face_property_animation = property_animation_create_layer_frame(mask_layer, &from_frame, &to_frame);
  
  animation_set_delay((Animation*) face_property_animation, 2000);
  
  animation_set_handlers((Animation*) face_property_animation, (AnimationHandlers) {
    .started = (AnimationStartedHandler) open_animation_started,
    .stopped = (AnimationStoppedHandler) open_animation_stopped
  }, NULL);
  
  animation_schedule((Animation*) face_property_animation);
}

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char buffer[] = "00:00";

  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    // Use 24 hour format
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    // Use 12 hour format
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
  }

  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, buffer);
}

static void close_animation_stopped(Animation *animation, bool finished, void *data) {
  
  if(finished) {
    show_eyes();
    property_animation_destroy(face_property_animation);
    update_time();
    open_mask();
  }
}

static void close_mask() {

  Layer* mask_layer = bitmap_layer_get_layer(s_face_layer);
  GRect from_frame = layer_get_frame(mask_layer);
  GRect to_frame = GRect(0, 0, 144, 143);
  
  face_property_animation = property_animation_create_layer_frame(mask_layer, &from_frame, &to_frame);
  
  animation_set_handlers((Animation*) face_property_animation, (AnimationHandlers) {
    .stopped = (AnimationStoppedHandler) close_animation_stopped
  }, NULL);
  
  animation_schedule((Animation*) face_property_animation);
}
  
static void main_window_load(Window *window) {
  
  window_set_background_color(window, GColorBlack);
  
  // Create time TextLayer
  s_time_layer = text_layer_create(GRect(0, 60, 144, 60));
  text_layer_set_background_color(s_time_layer, GColorBlack);
  text_layer_set_text_color(s_time_layer, GColorClear);

  // Improve the layout to be more like a watchface
  // Create GFont
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_IRON_MAN_48));

  // Apply to TextLayer
  text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
  
  // Create GBitmap, then set to created BitmapLayer
  s_face_bitmap = gbitmap_create_with_resource(RESOURCE_ID_FACE);
  s_face_layer = bitmap_layer_create(GRect(0, -80, 144, 143));
  bitmap_layer_set_bitmap(s_face_layer, s_face_bitmap);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_face_layer));
  
  // Lip
  s_top_bitmap_white = gbitmap_create_with_resource(RESOURCE_ID_LIP_WHITE);
  s_top_bitmap_black = gbitmap_create_with_resource(RESOURCE_ID_LIP_BLACK);
  
  s_top_layer_white = bitmap_layer_create(GRect(0, 135, 144, 23));
  bitmap_layer_set_bitmap(s_top_layer_white, s_top_bitmap_white);
  bitmap_layer_set_compositing_mode(s_top_layer_white, GCompOpOr);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_top_layer_white));
  
  s_top_layer_black = bitmap_layer_create(GRect(0, 135, 144, 23));
  bitmap_layer_set_bitmap(s_top_layer_black, s_top_bitmap_black);
  bitmap_layer_set_compositing_mode(s_top_layer_black, GCompOpClear);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_top_layer_black));
    
  // Make sure the time is displayed from the start
  update_time();
}

static void main_window_unload(Window *window) {
  
  bitmap_layer_destroy(s_top_layer_black);
  bitmap_layer_destroy(s_top_layer_white);
  bitmap_layer_destroy(s_face_layer);
  //bitmap_layer_destroy(s_eyes_layer_black);
  //bitmap_layer_destroy(s_eyes_layer_white);

  gbitmap_destroy(s_top_bitmap_black);
  gbitmap_destroy(s_top_bitmap_white);
  gbitmap_destroy(s_face_bitmap);
  //gbitmap_destroy(s_eyes_bitmap_black);
  //gbitmap_destroy(s_eyes_bitmap_white);
  
  // Unload GFont
  fonts_unload_custom_font(s_time_font);
  
  // Destroy TextLayer
  text_layer_destroy(s_time_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  close_mask();
}

static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
}

static void deinit() {
  animation_unschedule_all();
  
  // Destroy Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}