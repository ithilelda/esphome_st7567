#include "st7567.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"
#include "esphome/components/display/display_buffer.h"

namespace esphome {
namespace st7567 {

static const char *const TAG = "st7567";

// ST7567 COMMANDS
// Commands starting with SET leaves the varying bit as 1, so you should use the OR(|) operator to get the actual command.
static const uint8_t LCD_SETDISPLAY = 0xAE;
static const uint8_t LCD_SETSTARTLINE = 0x40;
static const uint8_t LCD_SETPAGEADDR = 0xB0;
static const uint8_t LCD_SETCOLADDRMSB = 0x10;
static const uint8_t LCD_SETCOLADDRLSB = 0x00;
static const uint8_t LCD_SETREVSEGDIR = 0xA0;
static const uint8_t LCD_SETINVERT = 0xA6;
static const uint8_t LCD_SETALLON = 0xA4;
static const uint8_t LCD_SETBIAS = 0xA2;
static const uint8_t LCD_STARTRMW = 0xE0;
static const uint8_t LCD_ENDRMW = 0xEE;
static const uint8_t LCD_SOFTRST = 0xE2;
static const uint8_t LCD_SETREVCOMDIR = 0xC0;
static const uint8_t LCD_SETPWRCTRL = 0x28;
static const uint8_t LCD_SETREGRATIO = 0x20;
static const uint8_t LCD_EVSETSTART = 0x81;
static const uint8_t LCD_BOOSTERSETSTART = 0xF8;
static const uint8_t LCD_NOP = 0xE3;


// overrides.
void ST7567::setup() {
  ESP_LOGCONFIG(TAG, "Setting up ST7567...");
  this->dump_config();
  this->spi_setup();

  this->dc_pin_->setup();  // setup dc pin.
  this->dc_pin_->digital_write(true);

  this->init_reset_();
  this->init_internal_(this->get_buffer_length_());
  this->display_init_();
}

void ST7567::dump_config() {
  LOG_DISPLAY("", "ST7567", this);
  LOG_PIN("  CS Pin: ", this->cs_);
  LOG_PIN("  Reset Pin: ", this->reset_pin_);
  LOG_PIN("  DC Pin: ", this->dc_pin_);
  ESP_LOGCONFIG(TAG, "  Width: %d", this->width_);
  ESP_LOGCONFIG(TAG, "  Height: %d", this->height_);
}

void ST7567::update() {
  this->clear();
  if (this->writer_local_.has_value())  // call lambda function if available
    (*this->writer_local_)(*this);
  this->write_display_data_();
}

void ST7567::fill(Color color) { memset(this->buffer_, color.is_on() ? 0xFF : 0x00, this->get_buffer_length_()); }


//protected overrides.
void HOT ST7567::draw_absolute_pixel_internal(int x, int y, Color color) {
  if (x >= this->get_width_internal() || x < 0 || y >= this->get_height_internal() || y < 0) {
    ESP_LOGW(TAG, "Position out of area: %dx%d", x, y);
    return;
  }
  if (color.is_on()) {
    this->buffer_[get_buffer_index_(x, y)] |= 0x01 << get_byte_index_(x, y);
  } else {
    this->buffer_[get_buffer_index_(x, y)] &= ~(0x01 << get_byte_index_(x, y));
  }
}


// custom methods.
void ST7567::init_reset_() {
  if (this->reset_pin_ != nullptr) {
    this->reset_pin_->setup();
    this->reset_pin_->digital_write(true);
    delay(50);
    // Trigger Reset
    this->reset_pin_->digital_write(false);
    delay(500);
    // Wake up
    this->reset_pin_->digital_write(true);
    delay(10);
  }
}

void ST7567::display_init_() {
  ESP_LOGD(TAG, "Initializing display...");
  this->command_(LCD_SETDISPLAY|0x00);              // set display off.
  this->command_(LCD_SETINVERT|this->inverted_);    // set display mode. (inverted means black/white invert.)
  this->command_(LCD_SETREVSEGDIR|this->flip_x_);   // set segment direction. (flipped left and right.)
  this->command_(LCD_SETREVCOMDIR|this->flip_y_);   // set COM direction.
  this->command_(LCD_SETSTARTLINE|0x00);            // set startline to 0.
  this->command_(LCD_SETBIAS|this->bias_);          // set bias.
  this->command_(LCD_SETREGRATIO|this->reg_ratio_); // set regulation ratio.
  this->command_(LCD_EVSETSTART);                   // start setting EV.
  this->command_(this->ev_);                        // set ev.
  this->command_(LCD_BOOSTERSETSTART);              // start setting booster.
  this->command_(this->booster_);                   // set booster.
  this->command_(LCD_SETPWRCTRL|0x07);              // turn on all three built in power controls.
  this->command_(LCD_SETDISPLAY|0x01);              // set display on.
}

void HOT ST7567::command_(uint8_t value) {
  this->enable();
  this->dc_pin_->digital_write(false);
  this->write_byte(value);
  this->disable();
}

void HOT ST7567::write_display_data_() {
  for (int page = 0; page < this->get_page_size(); page++) {
    uint8_t* b_start = this->buffer_ + (page * this->get_width_internal());
    this->command_(LCD_SETPAGEADDR|page);
    this->command_(LCD_SETCOLADDRMSB|0);
    this->command_(LCD_SETCOLADDRLSB|0);
    this->enable();
    this->dc_pin_->digital_write(true);
    this->write_array(b_start, this->get_width_internal());
    this->disable();
  }
}


}  // namespace st7567
}  // namespace esphome