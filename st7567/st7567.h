#pragma once

#include "esphome/core/component.h"
#include "esphome/components/display/display_buffer.h"
#include "esphome/components/spi/spi.h"

namespace esphome {
namespace st7567 {

class ST7567;

using st7567_writer_t = std::function<void(ST7567 &)>;

class ST7567 : public PollingComponent,
               public display::DisplayBuffer,
               public spi::SPIDevice<spi::BIT_ORDER_MSB_FIRST, spi::CLOCK_POLARITY_HIGH, spi::CLOCK_PHASE_TRAILING,
                                     spi::DATA_RATE_1MHZ> {
 public:
  void set_writer(st7567_writer_t &&writer) { this->writer_local_ = writer; }
  void set_reset_pin(GPIOPin *value) { this->reset_pin_ = value; }
  void set_dc_pin(GPIOPin *value) { this->dc_pin_ = value; }
  void set_height(uint16_t height) { this->height_ = height; }
  void set_width(uint16_t width) { this->width_ = width; }

  // ========== INTERNAL METHODS ==========
  void setup() override;
  void dump_config() override;
  void update() override;
  void fill(Color color) override;

  float get_setup_priority() const override { return setup_priority::PROCESSOR; }
  display::DisplayType get_display_type() override { return display::DisplayType::DISPLAY_TYPE_BINARY; }

 protected:
  // overrides that must be implemented in DisplayBuffer.
  int get_height_internal() override { return this->width_; }
  int get_width_internal() override { return this->height_; }

  void draw_absolute_pixel_internal(int x, int y, Color color) override;

  // custom methods.
  inline int get_buffer_index_(int x, int y) { return y / 8 * get_width_internal() + x; }
  inline int get_byte_index_(int x, int y) { return y % 8; }
  size_t get_buffer_length_() { return size_t(this->get_width_internal()) * size_t(this->get_height_internal()) / 8u; }

  void init_reset_();
  void display_init_();
  void command_(uint8_t value);
  void data_(uint8_t value);
  void write_display_data_();

  int16_t width_ = 128, height_ = 64;
  GPIOPin *reset_pin_{nullptr};
  GPIOPin *dc_pin_{nullptr};
  optional<st7567_writer_t> writer_local_{};
};

}  // namespace st7567
}  // namespace esphome