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

#ifdef __AVR__
#    include "exit_finder.h"
#else
#    include "flysight/fw/exit_finder.h"
#endif

namespace flysight {
namespace fw {

ExitFindStatus ExitFinder::detect(const int32_t height_msl_mm, const int32_t velocity_down_cmps,
                                  const int32_t dz_elevation_mm, const int32_t time_ms) {
    if (min_alt_agl_mm < 0 || height_msl_mm < min_alt_agl_mm + dz_elevation_mm) {
        return NO_CHANGE;
    }

    if (cfg_exit_alt_agl_mm >= 0 && height_msl_mm > cfg_exit_alt_agl_mm + dz_elevation_mm) {
        return NO_CHANGE;
    }

    if (direction_ == DOWN) {
        // Currently going down
        if (velocity_down_cmps > down_thresh_cmps) {
            if (count_ == 0) {
                curr_exit_alt_mm_ = height_msl_mm;
                curr_exit_time_ms_ = time_ms;
            }
            if (count_ < 255) {
                ++count_;
            }
        }
        else if (velocity_down_cmps < up_thresh_cmps) {
            count_ = 1;
            direction_ = UP;
        }
        else {
            count_ = 0;
        }

        if (!exit_alt_valid_ && count_ > num_down) {
            exit_alt_valid_ = true;
            exit_alt_mm_ = curr_exit_alt_mm_;
            exit_time_ms_ = curr_exit_time_ms_;
            return FOUND;
        }
    }
    else {
        // Currently going up
        if (velocity_down_cmps > down_thresh_cmps) {
            count_ = 1;
            curr_exit_alt_mm_ = height_msl_mm;
            curr_exit_time_ms_ = time_ms;
            direction_ = DOWN;
        }
        else if (velocity_down_cmps < up_thresh_cmps) {
            if (count_ < 255) {
                ++count_;
            }
        }
        else {
            count_ = 0;
        }

        if (exit_alt_valid_ && count_ > num_up) {
            exit_alt_valid_ = false;
            return RESET;
        }
    }

    return NO_CHANGE;
}

} // namespace fw
} // namespace flysight
