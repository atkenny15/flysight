/***************************************************************************
**                                                                        **
**  FlySight firmware                                                     **
**  Copyright 2018 Michael Cooper, Tom van Dijck                          **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see <http://www.gnu.org/licenses/>. **
**                                                                        **
****************************************************************************
**  Contact: Michael Cooper                                               **
**  Website: http://flysight.ca/                                          **
****************************************************************************/

#pragma once

#include <stdint.h>
#include <stdlib.h>

namespace flysight {
namespace fw {
namespace detail {

void display_8mhz(size_t num_bytes, const uint8_t* pixels, volatile uint8_t* port_ptr, uint8_t bit);

} // namespace detail

constexpr uint32_t color_rgb(const uint8_t red, const uint8_t green, const uint8_t blue) {
    return (static_cast<uint32_t>(red) << 8) | (static_cast<uint32_t>(green) << 0) |
           (static_cast<uint32_t>(blue) << 16);
}

constexpr uint8_t color_red(const uint32_t rgb) {
    return (rgb >> 8) & 0xff;
}

constexpr uint8_t color_green(const uint32_t rgb) {
    return (rgb >> 0) & 0xff;
}

constexpr uint8_t color_blue(const uint32_t rgb) {
    return (rgb >> 16) & 0xff;
}

template<size_t MAX_LEDS = 16>
class PositionLeds {
    static constexpr size_t BYTES_PER_PIXEL = 3;

public:
    explicit PositionLeds(volatile uint8_t* port, const uint8_t bit) : port_(port), bit_(bit) {
        set_num_leds(15);
        update();
    }

    void set_lane_width(const int16_t lane_width) {
        lane_width_ = lane_width;
        update();
    }

    void set_num_leds(const size_t num_leds) {
        if (num_leds > MAX_LEDS) {
            return;
        }
        num_leds_ = num_leds;
        update();
    }

    void set_led_offset(const size_t led_offset) {
        if (led_offset >= num_leds_) {
            return;
        }
        led_offset_ = led_offset;
        update();
    }

    int16_t get_position_offset() const noexcept { return position_offset_; }
    int16_t get_meters_per_led() const noexcept { return meters_per_led_; }

    size_t size() const { return num_leds_ - led_offset_; }

    void display_value(const uint32_t color_rgb, const int step = 1) {
        set_value(color_rgb, step);
        display();
    }

    void set_value(const uint32_t color_rgb, const int step = 1) {
        for (size_t i = 0; i < size(); ++i) {
            if (static_cast<int>(i) % step == 0) {
                set_led(i, color_rgb);
            }
            else {
                set_led(i, 0);
            }
        }
        changed_ = true;
    }

    void set_num_leds(const uint8_t num_leds, const uint32_t color_rgb) {
        for (size_t i = 0; i < size(); ++i) {
            const bool is_center = i == size() / 2;
            if (is_center) {
                set_led(i, LED_MIDDLE);
            } else if (i < num_leds || (size() - i - 1) < num_leds) {
                set_led(i, color_rgb);
            } else {
                set_led(i, 0);
            }
        }
        changed_ = true;
    }

    void display_position(const int16_t pos) {
        set_position(pos);
        display();
    }

    void set_position(const int16_t pos) {
        // Wrapping here does not matter since all LEDs will still be set to LED_OUTSIDE
        int16_t led_pos = pos - position_offset_;

#ifndef SMOOTH_LEDS
        for (size_t i = 0; i < size(); ++i) {
            const bool is_center = i == size() / 2;

            if (abs(led_pos) >= half_lane_width_) {
                set_led(i, is_center ? LED_OUTSIDE | LED_CENTER : LED_OUTSIDE);
            }
            else if (-meters_per_led_div2_ <= led_pos && led_pos < meters_per_led_div2_) {
                set_led(i, is_center ? LED_MIDDLE_CENTER : LED_MIDDLE);
            }
            else {
                set_led(i, is_center ? LED_CENTER : LED_INSIDE);
            }

            led_pos += meters_per_led_;
        }
#else
        for (size_t i = 0; i < size(); ++i) {
            const bool is_center = i == size() / 2;

            if (abs(led_pos) >= half_lane_width_) {
                uint8_t val = 0x10 * (abs(led_pos) - half_lane_width_) / meters_per_led_;
                if (val > 0x10) {
                    val = 0x10;
                }
                set_led(i, is_center ? LED_OUTSIDE | LED_CENTER : color_rgb(val, 0, 0));
            }
            else if (abs(led_pos) < meters_per_led_) {
                uint8_t val = 0x10 * (meters_per_led_ - abs(led_pos)) / meters_per_led_;
                if (val > 0x10) {
                    val = 0x10;
                }
                set_led(i, is_center ? LED_MIDDLE_CENTER : color_rgb(0, val, 0));
            }
            else {
                set_led(i, is_center ? LED_CENTER : LED_INSIDE);
            }
            led_pos += meters_per_led_;
        }
#endif
    }

    void display() {
        if (changed_) {
            detail::display_8mhz(sizeof(leds_), leds_, port_, bit_);
        }
        changed_ = false;
    }

    void set_led(size_t index, const uint32_t color_rgb) {
        if (get_led(index) == color_rgb) {
            return;
        }

        index += led_offset_;
        leds_[index * BYTES_PER_PIXEL + 0] = (color_rgb >> 0) & 0xff;
        leds_[index * BYTES_PER_PIXEL + 1] = (color_rgb >> 8) & 0xff;
        leds_[index * BYTES_PER_PIXEL + 2] = (color_rgb >> 16) & 0xff;
        changed_ = true;
    }

    uint32_t get_led(size_t index) const {
        uint32_t ret = 0;
        index += led_offset_;
        ret |= static_cast<uint32_t>(leds_[index * BYTES_PER_PIXEL + 0]) << 0;
        ret |= static_cast<uint32_t>(leds_[index * BYTES_PER_PIXEL + 1]) << 8;
        ret |= static_cast<uint32_t>(leds_[index * BYTES_PER_PIXEL + 2]) << 16;
        return ret;
    }

    // clang-format off
    static constexpr uint32_t LED_ON            = color_rgb(0x00, 0x10, 0x00); // green: flysight is turned on
    static constexpr uint32_t LED_EXIT          = color_rgb(0x10, 0x00, 0x10); // purple: exit detected
    static constexpr uint32_t LED_OUTSIDE       = color_rgb(0x10, 0x00, 0x00); // red: outside the lane
    static constexpr uint32_t LED_CENTER        = color_rgb(0x00, 0x00, 0x10); // blue: center of the display
    static constexpr uint32_t LED_MIDDLE        = color_rgb(0x00, 0x10, 0x00); // green: middle of the lane
    static constexpr uint32_t LED_INSIDE        = color_rgb(0x00, 0x00, 0x00); // off: inside the lane
    static constexpr uint32_t LED_MIDDLE_CENTER = color_rgb(0x10, 0x10, 0x00); // yellow: middle / center overlap
    static constexpr uint32_t LED_ERROR         = color_rgb(0x10, 0x08, 0x00); // orange
    // clang-format on

private:
    size_t num_leds_ = MAX_LEDS;
    size_t led_offset_ = 0;
    int16_t lane_width_ = 600;

    uint8_t leds_[MAX_LEDS * BYTES_PER_PIXEL];

    volatile uint8_t* port_;
    uint8_t bit_;

    int16_t half_lane_width_;
    int16_t meters_per_led_;
    int16_t meters_per_led_div2_;
    int16_t position_offset_;

    bool changed_ = true;

    void update() {
        const int16_t led_adjust = 0;
        half_lane_width_ = lane_width_ / 2;
        meters_per_led_ = (((half_lane_width_ * 2) / (static_cast<int16_t>(size()) - led_adjust)));
        meters_per_led_div2_ = (meters_per_led_ / 2);

        // Without this, if meters_per_led_div2_ gets truncated, the step size
        // (led_pos += meters_per_led_ in set_position) will prevent the center
        // of the lane from being displayed for certain positions.
        meters_per_led_ = meters_per_led_div2_ * 2;

        position_offset_ = (static_cast<int16_t>(size() / 2)) * meters_per_led_;
        memset(leds_, 0, sizeof(leds_));
    }
};

} // namespace fw
} // namespace flysight
