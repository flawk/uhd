//
// Copyright 2010-2012 Ettus Research LLC
// Copyright 2018 Ettus Research, a National Instruments Company
//
// SPDX-License-Identifier: GPL-3.0-or-later
//

#pragma once

#include <uhd/config.hpp>
#include <stdint.h>
#include <boost/operators.hpp>
#include <cmath>
#include <iomanip>
#include <limits>
#include <ostream>
#include <sstream>

namespace uhd {

/*!
 * A time_spec_t holds a seconds and a fractional seconds time value.
 * Depending upon usage, the time_spec_t can represent absolute times,
 * relative times, or time differences (between absolute times).
 *
 * The time_spec_t provides clock-domain independent time storage,
 * but can convert fractional seconds to/from clock-domain specific units.
 *
 * The fractional seconds are stored as double precision floating point.
 * This gives the fractional seconds enough precision to unambiguously
 * specify a clock-tick/sample-count up to rates of several petahertz.
 */
class UHD_API time_spec_t
{
private:
    /// Helpers
    static constexpr int64_t fast_llround(double x) noexcept
    {
        return static_cast<int64_t>(x + 0.5); // assumption of non-negativity
    }


    /// private time storage details
    int64_t _full_secs{};
    double _frac_secs{};

public:
    // A special value that signifies immediate execution
    static constexpr double ASAP = 0.0;

    /*!
     * Create a time_spec_t with 0 time
     */
    constexpr time_spec_t() noexcept {}

    /*!
     * Create a time_spec_t from whole seconds.
     * \param full_secs the whole/integer seconds count
     */
    constexpr time_spec_t(int64_t full_secs) noexcept : _full_secs(full_secs) {}

    /*!
     * Create a time_spec_t from whole and fractional seconds.
     * \param full_secs the whole/integer seconds count
     * \param frac_secs the fractional seconds count (default = 0)
     */
    constexpr time_spec_t(int64_t full_secs, double frac_secs) noexcept
    {
        set_from(full_secs, frac_secs);
    }

    /*!
     * Create a time_spec_t from a real-valued seconds count.
     * \param secs the real-valued seconds count (default = 0)
     */
    constexpr time_spec_t(double secs) noexcept : time_spec_t(0, secs) {}

    /*!
     * Create a time_spec_t from whole seconds and fractional ticks.
     * Translation from clock-domain specific units.
     * \param full_secs the whole/integer seconds count
     * \param tick_count the fractional seconds tick count
     * \param tick_rate the number of ticks per second
     */
    constexpr time_spec_t(
        int64_t full_secs, int64_t tick_count, double tick_rate) noexcept
        : time_spec_t(full_secs, tick_count / tick_rate)
    {
    }

    /*!
     * Re-set a time_spec_t from whole and fractional seconds.
     * \param full_secs the whole/integer seconds count
     * \param frac_secs the fractional seconds count (default = 0)
     */
    constexpr void set_from(int64_t full_secs, double frac_secs) noexcept
    {
        const int64_t _frac_int = static_cast<int64_t>(frac_secs);
        _full_secs              = full_secs + _frac_int;
        _frac_secs              = frac_secs - _frac_int;
        if (_frac_secs < 0) {
            _full_secs -= 1;
            _frac_secs += 1;
        }
    }

    /*!
     * Create a time_spec_t from a 64-bit tick count.
     * Translation from clock-domain specific units.
     * \param ticks an integer count of ticks
     * \param tick_rate the number of ticks per second
     */
    constexpr static time_spec_t from_ticks(int64_t ticks, double tick_rate) noexcept
    {
        const int64_t rate_i      = static_cast<int64_t>(tick_rate);
        const double rate_f       = tick_rate - rate_i;
        const int64_t secs_full   = ticks / rate_i;
        const int64_t ticks_error = ticks - secs_full * rate_i;
        const double ticks_frac   = ticks_error - secs_full * rate_f;
        return time_spec_t{secs_full, ticks_frac / tick_rate};
    }

    /*!
     * Convert the fractional seconds to clock ticks.
     * Translation into clock-domain specific units.
     * \param tick_rate the number of ticks per second
     * \return the fractional seconds tick count
     */
    constexpr int64_t get_tick_count(double tick_rate) const noexcept
    {
        return fast_llround(get_frac_secs() * tick_rate);
    }

    /*!
     * Convert the time spec into a 64-bit tick count.
     * Translation into clock-domain specific units.
     * \param tick_rate the number of ticks per second
     * \return an integer number of ticks
     */
    constexpr int64_t to_ticks(const double tick_rate) const noexcept
    {
        const int64_t rate_i     = static_cast<int64_t>(tick_rate);
        const double rate_f      = tick_rate - rate_i;
        const int64_t ticks_full = get_full_secs() * rate_i;
        const double ticks_error = get_full_secs() * rate_f;
        const double ticks_frac  = get_frac_secs() * tick_rate;
        return ticks_full + fast_llround(ticks_error + ticks_frac);
    }


    /*!
     * Get the time as a real-valued seconds count.
     * Note: If this time_spec_t represents an absolute time,
     * the precision of the fractional seconds may be lost.
     * \return the real-valued seconds
     */
    constexpr double get_real_secs(void) const noexcept
    {
        return get_full_secs() + get_frac_secs();
    }

    /*!
     * Get the whole/integer part of the time in seconds.
     * \return the whole/integer seconds
     */
    constexpr int64_t get_full_secs(void) const noexcept
    {
        return _full_secs;
    }

    /*!
     * Get the fractional part of the time in seconds.
     * \return the fractional seconds
     */
    constexpr double get_frac_secs(void) const noexcept
    {
        return _frac_secs;
    }

    //! stringify, keeping full precision
    operator std::string() const
    {
        std::stringstream ss;
        ss << this;
        return ss.str();
    }

    //! Implement addable interface
    friend constexpr time_spec_t operator+(const time_spec_t& l, const time_spec_t& r)
    {
        return {
            l.get_full_secs() + r.get_full_secs(), l.get_frac_secs() + r.get_frac_secs()};
    }
    time_spec_t& operator+=(const time_spec_t& rhs) noexcept
    {
        set_from(
            get_full_secs() + rhs.get_full_secs(), get_frac_secs() + rhs.get_frac_secs());
        return *this;
    }
    friend constexpr time_spec_t operator+(const time_spec_t& l, const double& r)
    {
        double full_secs = std::trunc(r);
        return {l.get_full_secs() + static_cast<int64_t>(full_secs),
            l.get_frac_secs() + (r - full_secs)};
    }
    time_spec_t& operator+=(const double& rhs) noexcept
    {
        double full_secs = std::trunc(rhs);
        set_from(get_full_secs() + static_cast<int64_t>(full_secs),
            get_frac_secs() + (rhs - full_secs));
        return *this;
    }
    //! Implement subtractable interface
    friend constexpr time_spec_t operator-(const time_spec_t& l, const time_spec_t& r)
    {
        return {
            l.get_full_secs() - r.get_full_secs(), l.get_frac_secs() - r.get_frac_secs()};
    }
    time_spec_t& operator-=(const time_spec_t& rhs) noexcept
    {
        set_from(
            get_full_secs() - rhs.get_full_secs(), get_frac_secs() - rhs.get_frac_secs());
        return *this;
    }
    friend constexpr time_spec_t operator-(const time_spec_t& l, const double& r)
    {
        double full_secs = std::trunc(r);
        return {l.get_full_secs() - static_cast<int64_t>(full_secs),
            l.get_frac_secs() - (r - full_secs)};
    }
    time_spec_t& operator-=(const double& rhs) noexcept
    {
        double full_secs = std::trunc(rhs);
        set_from(get_full_secs() - static_cast<int64_t>(full_secs),
            get_frac_secs() - (rhs - full_secs));
        return *this;
    }

    //! Implement equality_comparable interface
    friend constexpr bool operator==(const time_spec_t& l, const time_spec_t& r)
    {
        return l.get_full_secs() == r.get_full_secs()
               && l.get_frac_secs() == r.get_frac_secs();
    }

    //! Implement inequality_comparable interface
    friend constexpr bool operator!=(const time_spec_t& l, const time_spec_t& r)
    {
        return l.get_full_secs() != r.get_full_secs()
               || l.get_frac_secs() != r.get_frac_secs();
    }

    //! Implement less_than_comparable interface
    friend constexpr bool operator<(const time_spec_t& l, const time_spec_t& r)
    {
        return l.get_full_secs() < r.get_full_secs()
               || (l.get_full_secs() == r.get_full_secs()
                   && l.get_frac_secs() < r.get_frac_secs());
    }

    //! Implement greater_than_comparable interface
    friend constexpr bool operator>(const time_spec_t& l, const time_spec_t& r)
    {
        return l.get_full_secs() > r.get_full_secs()
               || (l.get_full_secs() == r.get_full_secs()
                   && l.get_frac_secs() > r.get_frac_secs());
    }

    //! Implement less_than_or_equal_comparable interface
    friend constexpr bool operator<=(const time_spec_t& l, const time_spec_t& r)
    {
        return l.get_full_secs() < r.get_full_secs()
               || (l.get_full_secs() == r.get_full_secs()
                   && l.get_frac_secs() <= r.get_frac_secs());
    }

    //! Implement greater_than_or_equal_comparable interface
    friend constexpr bool operator>=(const time_spec_t& l, const time_spec_t& r)
    {
        return l.get_full_secs() > r.get_full_secs()
               || (l.get_full_secs() == r.get_full_secs()
                   && l.get_frac_secs() >= r.get_frac_secs());
    }

    //! Implement ostream interface
    friend std::ostream& operator<<(std::ostream& os, const uhd::time_spec_t& t)
    {
        std::stringstream ss_frac;
        ss_frac << std::setprecision(std::numeric_limits<double>::digits10 + 1)
                << std::showpoint << t.get_frac_secs();
        return os << t.get_full_secs() << ss_frac.str().substr(1);
    }
};

} // namespace uhd
