//
// Copyright 2011 Ettus Research LLC
// Copyright 2018 Ettus Research, a National Instruments Company
//
// SPDX-License-Identifier: GPL-3.0-or-later
//

#pragma once

#include <uhd/config.hpp>
#include <uhd/exception.hpp>
#include <map>
#include <string>
#include <boost/type_traits.hpp>

namespace uhd {

/*!
 * A sensor value stores a sensor reading as a string with unit and data type.
 * The sensor value class can be used in the following way:
 *
 * sensor_value_t ref_lock_sensor("Reference", my_lock, "unlocked", "locked");
 * std::cout << ref_lock_sensor.to_pp_string() << std::endl;
 * //prints Reference: locked
 *
 * sensor_value_t temp_sensor("Temperature", my_temp, "C");
 * std::cout << temp_sensor.to_pp_string() << std::endl;
 * //prints Temperature: 38.5 C
 */
struct UHD_API sensor_value_t
{
    typedef std::map<std::string, std::string> sensor_map_t;

    /*!
     * Create a sensor value from a boolean.
     * \param name the name of the sensor
     * \param value the value true or false
     * \param utrue the unit string when value is true
     * \param ufalse the unit string when value is false
     */
    sensor_value_t(const std::string& name,
        bool value,
        const std::string& utrue,
        const std::string& ufalse);

    /*!
     * Create a sensor value from an integer.
     * \param name the name of the sensor
     * \param value the signed integer value
     * \param unit the associated unit type
     * \param formatter the formatter string
     */
    sensor_value_t(const std::string& name,
        signed value,
        const std::string& unit,
        const std::string& formatter = "%d");

    /*!
     * Create a sensor value from a real number.
     * \param name the name of the sensor
     * \param value the real number value
     * \param unit the associated unit type
     * \param formatter the formatter string
     */
    sensor_value_t(const std::string& name,
        double value,
        const std::string& unit,
        const std::string& formatter = "%f");

    /*!
     * Create a sensor value from a string.
     * \param name the name of the sensor
     * \param value the real number value
     * \param unit the associated unit type
     */
    sensor_value_t(
        const std::string& name, const std::string& value, const std::string& unit);

    /*!
     * Create a sensor value from a map.
     *
     * The map must have the following keys: name, type, value, and unit.
     *
     * type must one of the following strings: BOOLEAN, INTEGER, REALNUM,
     * or STRING (see data_type_t).
     */
    sensor_value_t(const std::map<std::string, std::string>& sensor_dict);

    /*!
     * Create a sensor value from another sensor value.
     * \param source the source sensor value to copy
     */
    sensor_value_t(const sensor_value_t& source);

    //! convert the sensor value to a boolean
    bool to_bool(void) const;

    //! convert the sensor value to an integer
    signed to_int(void) const;

    //! convert the sensor value to real number
    double to_real(void) const;

    //! convert the sensor value to sensor_map_t
    sensor_map_t to_map(void) const;

    //! The name of the sensor value
    std::string name;

    /*!
     * The sensor value as a string.
     * For integer and real number types, this will be the output of the formatter.
     * For boolean types, the value will be the string literal "true" or "false".
     */
    std::string value;

    /*!
     * The sensor value's unit type.
     * For boolean types, this will be the one of the two units
     * depending upon the value of the boolean true or false.
     */
    std::string unit;

    //! Enumeration of possible data types in a sensor
    enum data_type_t { BOOLEAN = 'b', INTEGER = 'i', REALNUM = 'r', STRING = 's' };

    //! The data type of the value
    data_type_t type;

    //! Returns the value string
    constexpr std::string& to_string(void)
    {
        return value;
    }

    //! Returns the value string
    constexpr data_type_t get_type(void)
    {
        return type;
    }

    //! Convert this sensor value into a printable string
    std::string to_pp_string(void) const;

    //! Assignment operator for sensor value
    sensor_value_t& operator=(const sensor_value_t& value);


    inline void assert_type(data_type_t o_type) const {
        if (type != o_type) {
            throw uhd::runtime_error("sensor_value_t type asseration failed: " + std::to_string(type) + " != " + std::to_string(o_type));
        }
    }

    template<typename T> inline T get_value() const;

    template<typename T> struct data_type_v;

    template<typename T> struct data_type_cvref {
        static constexpr auto type =
            data_type_v<typename boost::remove_cv_ref<T>::type>::type;
    };

    template<typename T> inline bool is_value(T v) const {
        return data_type_cvref<T>::type == type && get_value<T>() == v;
    }
    template<typename T> inline bool is_not_value(T v) const {
        return data_type_cvref<T>::type != type || get_value<T>() != v;
    }
};

template<> inline bool sensor_value_t::get_value<bool>() const {
    assert_type(BOOLEAN); return to_bool();
}
template<> inline int sensor_value_t::get_value<int>() const {
    assert_type(INTEGER); return to_int();
}
template<> inline double sensor_value_t::get_value<double>() const {
    assert_type(REALNUM); return to_real();
}
template<> inline const std::string& sensor_value_t::get_value<const std::string&>() const {
    assert_type(STRING); return value;
}

template<> struct sensor_value_t::data_type_v<bool>{
    static constexpr auto type = data_type_t::BOOLEAN;
};
template<> struct sensor_value_t::data_type_v<int> {
    static constexpr auto type = data_type_t::INTEGER;
};
template<> struct sensor_value_t::data_type_v<double> {
    static constexpr auto type = data_type_t::REALNUM;
};
template<> struct sensor_value_t::data_type_v<std::string> {
    static constexpr auto type = data_type_t::STRING;
};

} // namespace uhd
