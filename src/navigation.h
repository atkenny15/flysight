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

#include "stdint.h"
#include "stdlib.h"

#ifdef ARDUINO
#    include "Arduino.h"
#elif defined(__AVR__)
#    include "math.h"
#    define PI M_PI
#else
#    include <cmath>
#    include <fmt/format.h>
#    include <fmt/ostream.h>
#    include <iostream>
#    include <numbers>
#    define PI std::numbers::pi
#endif

namespace flysight {
namespace fw {

namespace detail {

// --------------------------------------------------
// remove_cv
// --------------------------------------------------

template<class T>
struct remove_cv {
    typedef T type;
};
template<class T>
struct remove_cv<const T> {
    typedef T type;
};
template<class T>
struct remove_cv<volatile T> {
    typedef T type;
};
template<class T>
struct remove_cv<const volatile T> {
    typedef T type;
};

// --------------------------------------------------
// integral_constant
// --------------------------------------------------

template<class T, T v>
struct integral_constant {
    static constexpr T value = v;
};

using true_type = integral_constant<bool, true>;
using false_type = integral_constant<bool, false>;

// --------------------------------------------------
// is_same
// --------------------------------------------------

template<class T, class U>
struct is_same : false_type {};

template<class T>
struct is_same<T, T> : true_type {};

// --------------------------------------------------
// is_foating_point / is_integral
// --------------------------------------------------

template<class T>
struct is_floating_point
    : integral_constant<bool, is_same<float, typename remove_cv<T>::type>::value ||
                                  is_same<double, typename remove_cv<T>::type>::value ||
                                  is_same<long double, typename remove_cv<T>::type>::value> {};

template<class T>
struct is_integral
    : integral_constant<bool, is_same<int, typename remove_cv<T>::type>::value ||
                                  is_same<long int, typename remove_cv<T>::type>::value ||
                                  is_same<long long int, typename remove_cv<T>::type>::value> {};

// --------------------------------------------------
// enable_if
// --------------------------------------------------

template<bool B, class T = void>
struct enable_if {};

template<class T>
struct enable_if<true, T> {
    typedef T type;
};

template<bool B, class T>
using enable_if_t = typename enable_if<B, T>::type;

} // namespace detail

template<typename T>
struct ENU {
    // meters
    T x;
    // meters
    T y;
    // meters
    T z;
};

template<typename T>
struct ECEF;

template<typename T>
struct LLA {
    T latitude_deg;
    T longitude_deg;
    T height_msl_m;

    template<typename U = T,
             typename detail::enable_if_t<detail::is_floating_point<U>::value, bool> = true>
    ECEF<U> to_ecef() const noexcept {
        constexpr U a = 6378137.0;
        constexpr U b = 6356752.314245;
        constexpr U e2 = 1 - (b * b) / (a * a);

        const U lat_rad = this->latitude_deg * PI / 180;
        const U lon_rad = this->longitude_deg * PI / 180;

        const U n = a / sqrt(1 - e2 * sin(lat_rad) * sin(lat_rad));

        ECEF<U> ecef;
        ecef.x = (n + this->height_msl_m) * cos(lat_rad) * cos(lon_rad);
        ecef.y = (n + this->height_msl_m) * cos(lat_rad) * sin(lon_rad);
        ecef.z = ((1 - e2) * n + this->height_msl_m) * sin(lat_rad);
        return ecef;
    }

    template<typename U = T,
             typename detail::enable_if_t<detail::is_integral<U>::value, bool> = true>
    ECEF<U> to_ecef() const noexcept {
        constexpr int64_t a = 6378137;
        constexpr int64_t b = 635675231;                  // * 1e2
        constexpr int64_t e2 = 10000 - (b * b) / (a * a); // * 1e4

        const U lat_rad = this->latitude_deg * PI / 180;
        const U lon_rad = this->longitude_deg * PI / 180;

        const U n = a / sqrt(1 - e2 * sin(lat_rad) * sin(lat_rad));

        ECEF<U> ecef;
        ecef.x = (n + this->height_msl_m) * cos(lat_rad) * cos(lon_rad);
        ecef.y = (n + this->height_msl_m) * cos(lat_rad) * sin(lon_rad);
        ecef.z = ((1 - e2) * n + this->height_msl_m) * sin(lat_rad);
        return ecef;
    }
};

template<typename T>
struct ECEF_IMPL {};

template<typename T>
struct ECEF {
    // meters
    T x;
    // meters
    T y;
    // meters
    T z;

    template<typename U = T,
             typename detail::enable_if_t<detail::is_floating_point<U>::value, bool> = true>
    ENU<U> to_enu(const LLA<U>& ref_lla, const ECEF<U>& ref_ecef) const noexcept {
        const U lat_rad = ref_lla.latitude_deg * PI / 180;
        const U lon_rad = ref_lla.longitude_deg * PI / 180;

        const U x_delta = this->x - ref_ecef.x;
        const U y_delta = this->y - ref_ecef.y;
        const U z_delta = this->z - ref_ecef.z;

        ENU<U> enu;
        enu.x = -sin(lon_rad) * x_delta + cos(lon_rad) * y_delta;
        enu.y = -sin(lat_rad) * cos(lon_rad) * x_delta - sin(lat_rad) * sin(lon_rad) * y_delta +
                cos(lat_rad) * z_delta;
        enu.z = cos(lat_rad) * cos(lon_rad) * x_delta + cos(lat_rad) * sin(lon_rad) * y_delta +
                sin(lat_rad) * z_delta;
        return enu;
    }

    template<typename U = T,
             typename detail::enable_if_t<detail::is_integral<U>::value, bool> = true>
    ENU<U> to_enu(const LLA<U>& ref_lla, const ECEF<U>& ref_ecef) const noexcept {
        const U lat_rad = ref_lla.latitude_deg * PI / 180;
        const U lon_rad = ref_lla.longitude_deg * PI / 180;

        const U x_delta = this->x - ref_ecef.x;
        const U y_delta = this->y - ref_ecef.y;
        const U z_delta = this->z - ref_ecef.z;

        ENU<U> enu;
        enu.x = -sin(lon_rad) * x_delta + cos(lon_rad) * y_delta;
        enu.y = -sin(lat_rad) * cos(lon_rad) * x_delta - sin(lat_rad) * sin(lon_rad) * y_delta +
                cos(lat_rad) * z_delta;
        enu.z = cos(lat_rad) * cos(lon_rad) * x_delta + cos(lat_rad) * sin(lon_rad) * y_delta +
                sin(lat_rad) * z_delta;
        return enu;
    }
};

template<typename T>
class Navigation { // LCOV_EXCL_LINE
public:
    /// Time after exit when the lane is valid
    int32_t time_after_exit_ms = -1;

    /// Set the reference point for the lane
    void set_ref_lla(const LLA<T>& ref_lla) noexcept {
        ref_lla_ = ref_lla;
        ref_ecef_ = ref_lla_.to_ecef();
        ref_lla_valid_ = true;
    }

    bool  is_ref_lla_valid() const noexcept { return ref_lla_valid_; }
    const LLA<T>& get_ref_lla() const noexcept { return ref_lla_; }

    bool is_enabled() const noexcept { return time_after_exit_ms >= 0; }

    /// Get the distance in meters from the center of the lane.
    ///
    /// If curr_time - exit_time_ms > time_after_exit_ms, capture the position.
    /// A line from this position to the reference point is the center of the
    /// lane. This function calculates the distance to the center of the lane
    /// and returns it. If the calculated position is valid (time_after_exit_ms
    /// >= 0 and curr_time - exit_time_ms > time_after_exit_ms,), return true.
    /// Otherwise return false.
    ///
    /// @param exit_time_ms     the timestamp when the plane was exited
    /// @param curr_time_ms     the current time
    /// @param pos_lla          the current position
    /// @param lane_pos_m       the distance from the center of the lane
    /// @returns true if lane_pos_m is valid, false otherwise
    bool get_position(const int32_t exit_time_ms, const int32_t curr_time_ms, const LLA<T>& pos_lla,
                      int16_t& lane_pos_m) noexcept {
        if (time_after_exit_ms < 0) {
            return false;
        }

        if (!ref_lla_valid_) {
            return false;
        }

        if (!lane_enu_valid_) {
            if (curr_time_ms - exit_time_ms < time_after_exit_ms) {
                return false;
            }
            ECEF<T> pos_ecef = pos_lla.to_ecef();
            lane_enu_ = pos_ecef.to_enu(ref_lla_, ref_ecef_);
            lane_enu_valid_ = true;

            lane_pos_m = 0;
            return true;
        }

        const ECEF<T> pos_ecef = pos_lla.to_ecef();
        const ENU<T> pos_enu = pos_ecef.to_enu(ref_lla_, ref_ecef_);

        const T xp = pos_enu.x;
        const T yp = pos_enu.y;

        const T xl = lane_enu_.x;
        const T yl = lane_enu_.y;

        lane_pos_m = get_distance(xp, yp, xl, yl);
        return true;
    }

    int16_t get_distance(const T xp, const T yp, const T xl, const T yl) const noexcept {
        // https://en.wikipedia.org/wiki/Distance_from_a_point_to_a_line
        const T a = -yl;
        const T b = xl;
        const int16_t d = fabs(a * xp + b * yp) / sqrtf(a * a + b * b);

        // Vector A from lane start to 0, 0
        const T a1 = -xl;
        const T a2 = -yl;

        // Vector B from 0, 0 to current position
        const T b1 = xp;
        const T b2 = yp;

        // Z value of A X B, invert the sign so that left side of the lane is
        // negative and right side is positive.
        const bool is_negative = a1 * b2 - a2 * b1 > 0;

        return is_negative ? -d : d;
    }

private:
    bool ref_lla_valid_ = false;
    LLA<T> ref_lla_;
    ECEF<T> ref_ecef_;

    bool lane_enu_valid_ = false;
    ENU<T> lane_enu_;
};

#ifdef ARDUINO
template<typename T, typename detail::enable_if_t<detail::is_same<T, ECEF<float>>::value ||
                                                      detail::is_same<T, ENU<float>>::value,
                                                  bool> = true>
void display(const char* name, const T& pos) {
    Serial.write(name);
    Serial.write(": x=");
    Serial.print(pos.x);
    Serial.write(", y=");
    Serial.print(pos.y);
    Serial.write(", z=");
    Serial.print(pos.z);
    Serial.write("\n");
}

template<typename T,
         typename detail::enable_if_t<detail::is_same<T, LLA<float>>::value, bool> = true>
void display(const char* name, const T& lla) {
    Serial.write(name);
    Serial.write(": latitude_deg=");
    Serial.print(lla.latitude_deg);
    Serial.write(", longitude_deg=");
    Serial.print(lla.longitude_deg);
    Serial.write(", height_msl_m=");
    Serial.print(lla.height_msl_m);
    Serial.write("\n");
}
#endif

} // namespace fw
} // namespace flysight
