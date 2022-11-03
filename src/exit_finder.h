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

namespace flysight {
namespace fw {

enum Direction {
    UP,
    DOWN,
};

enum ExitFindStatus {
    FOUND,
    RESET,
    NO_CHANGE,
};

struct ExitFinder {
    // ** config values **

    /// Do not detect exits if lower than this altitude in mm above ground. Exit
    /// detection is disabled if this is less than 0
    int32_t min_alt_agl_mm = -1;

    /// Do not detect exit altitude until lower than this threshold. This check
    // is disabled if < 0.
    int32_t cfg_exit_alt_agl_mm = -1;

    /// velocity_down (down is positive) to determine if going up (cm/s)
    int32_t up_thresh_cmps = -800;

    /// velocity_down (down is positive) to determine if going down (cm/s)
    int32_t down_thresh_cmps = 800;

    /// Number of consecutive down measurements needed to set exit_alt_valid
    uint8_t num_down = 5;

    /// Number of consecutive up measurements needed to reset exit_alt_valid
    uint8_t num_up = 50;

    /// Detect an exit
    ///
    /// @param height_msl_mm        Height above sea level in mm
    /// @param velocity_down_cmps   Downward (positive) velocity in cm/s
    /// @param dz_elevation_mm      DZ elevation above sea level in mm
    /// @param time_ms              Time of height/velocity measurement in ms
    ExitFindStatus detect(int32_t height_msl_mm, int32_t velocity_down_cmps,
                          int32_t dz_elevation_mm, int32_t time_ms);

    bool is_enabled() const noexcept { return min_alt_agl_mm >= 0; }

    bool exit_alt_valid() const noexcept { return exit_alt_valid_; }
    int32_t exit_alt_mm() const noexcept { return exit_alt_mm_; }
    int32_t exit_time_ms() const noexcept { return exit_time_ms_; }

private:
    /// Is the exit altitude valid?
    bool exit_alt_valid_ = false;

    /// The exit altitude (MSL) in mm
    int32_t exit_alt_mm_ = 0;

    /// The time of the exit in milliseconds since start of day
    int32_t exit_time_ms_ = 0;

    /// Direction of previous `count` velocity measurements
    Direction direction_ = UP;

    /// Number of velocity measurements
    uint8_t count_ = 0;

    /// The current exit altitude (MSL) in mm, latched into exit_alt_mm when
    /// exit_alt_valid changes from false to true
    int32_t curr_exit_alt_mm_ = 0;

    /// The current time of the exit in milliseconds since start of day,
    /// latched into exit_alt_mm when exit_alt_valid changes from false to true
    int32_t curr_exit_time_ms_ = 0;
};

} // namespace fw
} // namespace flysight
