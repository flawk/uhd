//
// Copyright 2010-2011,2015 Ettus Research LLC
// Copyright 2018 Ettus Research, a National Instruments Company
//
// SPDX-License-Identifier: GPL-3.0-or-later
//

#pragma once

#include <uhd/config.hpp>
#include <list>
#include <map>
#include <unordered_map>
#include <vector>

namespace uhd {

/*!
 * A templated dictionary class with a python-like interface.
 */
template <typename Key, typename Val>
class UHD_API_HEADER dict
{
public:
    /*!
     * Create a new empty dictionary.
     */
    dict(void);

    /*!
     * Input iterator constructor:
     * Makes boost::assign::map_list_of work.
     * \param first the begin iterator
     * \param last the end iterator
     */
    template <typename InputIterator>
    dict(InputIterator first, InputIterator last);

    //! Copy constructor from a std::map<Key, Val>
    //dict(const std::map<Key, Val>&);

    //! Move constructor from a std::map<Key, Val>
    //dict(std::map<Key, Val>&&);

    //! Assignment operator (=) from a std::map<Key, Val>
    //dict& operator=(const std::map<Key, Val>&);

    //! Move assignment operator (=) from a std::map<Key, Val>
    //dict& operator=(std::map<Key, Val>&&);

    //! Copy constructor from a std::map<Key, Val>
    //dict(const std::unordered_map<Key, Val>&);

    //! Move constructor from a std::map<Key, Val>
    //dict(std::unordered_map<Key, Val>&&);

    //! Assignment operator (=) from a std::unordered_map<Key, Val>
    //dict& operator=(const std::unordered_map<Key, Val>&);

    //! Move assignment operator (=) from a std::unordered_map<Key, Val>
    //dict& operator=(std::unordered_map<Key, Val>&&);

    /*!
     * Input iterator emplace
     * \param first the begin iterator
     * \param last the end iterator
     */
    template <typename InputIterator>
    void emplace(InputIterator first, InputIterator last);

    /*!
     * Input iterator insert
     * \param first the begin iterator
     * \param last the end iterator
     */
    template <typename InputIterator>
    void insert(InputIterator first, InputIterator last);

    //! Begin iterator
    typename std::list<std::pair<Key, Val>>::iterator begin() noexcept;

    //! Begin const iterator
    typename std::list<std::pair<Key, Val>>::const_iterator begin() const noexcept;

    //! End iterator
    typename std::list<std::pair<Key, Val>>::iterator end() noexcept;

    //! End const iterator
    typename std::list<std::pair<Key, Val>>::const_iterator end() const noexcept;

    //! Clears this dict
    void clear(void);

    /*!
     * Get the number of elements in this dict.
     * \return the number of elements
     */
    std::size_t size(void) const;

    /*!
     * Get a list of the keys in this dict.
     * Key order depends on insertion precedence.
     * \return vector of keys
     */
    std::vector<Key> keys(void) const;

    /*!
     * Get a list of the values in this dict.
     * Value order depends on insertion precedence.
     * \return vector of values
     */
    std::vector<Val> vals(void) const;

    /*!
     * Does the dictionary contain this key?
     * \param key the key to look for
     * \return true if found
     */
    bool has_key(const Key& key) const;

    /*!
     * Get a value in the dict or default.
     * \param key the key to look for
     * \param other use if key not found
     * \return the value or default
     */
    const Val& get(const Key& key, const Val& other) const;

    /*!
     * Get a value in the dict or throw.
     * \param key the key to look for
     * \return the value or default
     */
    const Val& get(const Key& key) const;

    /*!
     * Set a value in the dict at the key.
     * \param key the key to set at
     * \param val the value to set
     */
    void set(const Key& key, const Val& val);

    /*!
     * Get a value for the given key if it exists.
     * If the key is not found throw an error.
     * \param key the key to look for
     * \return the value at the key
     * \throw an exception when not found
     */
    const Val& operator[](const Key& key) const;

    /*!
     * Set a value for the given key, however, in reality
     * it really returns a reference which can be assigned to.
     * \param key the key to set to
     * \return a reference to the value
     */
    Val& operator[](const Key& key);

    /*!
     * Equals operator for the dict type
     * \param other the dict being compared to this
     * \return whether or not the two dict's are equal
     */
    bool operator==(const dict<Key, Val>& other) const;

    /*!
     * Not equal operator for the dict type
     * \param other the dict being compared to this
     * \return whether or not the two dict's are not equal
     */
    bool operator!=(const dict<Key, Val>& other) const;

    /*!
     * Pop an item out of the dictionary.
     * \param key the item key
     * \return the value of the item
     * \throw an exception when not found
     */
    Val pop(const Key& key);

    /*! Update this dictionary with values from another.
     *
     * Basically, this copies all the key/value pairs from \p new_dict
     * into this dict. When the key is already present in the current
     * dict, it either overwrites the current value (if \p fail_on_conflict
     * is false) or it throws (if \p fail_on_conflict is true *and* the
     * values differ).
     *
     * With the exception of \p fail_on_conflict, this behaves analogously
     * to Python's dict.update() method.
     *
     * \param new_dict The arguments to copy.
     * \param fail_on_conflict If true, throws.
     * \throws uhd::value_error
     */
    void update(const dict<Key, Val>& new_dict, bool fail_on_conflict = true);

    //! Typecast operator to std::map<>
    operator std::map<Key, Val>() const;

    //! Typecast move operator to std::map<>
    //operator std::map<Key, Val>&&() &&;

    //! Typecast operator to std::unordered_map<>
    operator std::unordered_map<Key, Val>() const;

    //! Typecast move operator to std::unordered_map<>
    //operator std::unordered_map<Key, Val>&&() &&;

private:
    typedef std::pair<Key, Val> pair_t;
    std::list<pair_t> _map; // private container
};

} // namespace uhd

#include <uhd/types/dict.ipp>
