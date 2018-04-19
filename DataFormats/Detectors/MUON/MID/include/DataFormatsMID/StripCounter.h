// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file   DataFormatsMID/StripCounter.h
/// \brief  Strip counter
/// \author Gabriele G. Fronzé <gfronze at cern.ch>
/// \date   19 April 2018

#ifndef O2_MID_STRIPCOUNTER_H
#define O2_MID_STRIPCOUNTER_H

#include <stdexcept>
#include <cstdint>
#include <cstddef>
#include <array>
#include <boost/serialization/access.hpp>
#include <boost/serialization/array.hpp>

namespace o2
{
namespace mid
{
/// Hit counter structure for MID strips
struct Counter {
  /// Add pattern bitwise to corresponding counter element
  template <class patternT>
  typename std::enable_if<std::is_integral<patternT>::value, Counter&>::type operator+=(patternT pattern)
  {
    static_assert(8 * sizeof(pattern) == 16,
                  "This pattern is too big to fit. The limit is 16 bits (2 bytes).");

    for (int iBit = 0; iBit < counter.size(); ++iBit) {
      counter[iBit] += (pattern >> iBit) & 1;
    }

    return *this;
  }
  /// Get specific counter value
  uint64_t operator[](size_t index)
  {
    if (index > counter.size() - 1)
      throw std::out_of_range("Invalid index.");
    return counter[index];
  }
  /// Reset all counters
  void reset() { counter.fill(0); }
  /// Check if enough statistics to send
  bool enoughToSend(uint64_t minValue) const
  {
    bool ret = true;
    for (const auto& itCounter : counter) {
      ret &= (itCounter > minValue);
    }
    return ret;
  }

  std::array<uint64_t, 16> counter; ///< single local board counter

  friend class boost::serialization::access;

  /// Serializes the struct
  template <class Archive>
  void serialize(Archive& ar, const unsigned int version)
  {
    ar& counter;
  }
};

/// Strip counter structure for MID
struct StripCounter {
  /// Number of patterns per column: 4 bend (at most) + 1 non-bend
  static const uint16_t nCounters = 5;

  /// Add non-bending plane patterns to counters
  void addNonBendPattern(uint16_t pattern) { counters[nCounters - 1] += pattern; }
  /// Get non-bending plane counters
  Counter getNonBendCounters() { return counters[nCounters - 1]; }
  /// Add bending plane patterns to counters
  void addBendPattern(uint16_t pattern, uint16_t line) { counters[line] += pattern; }
  /// Get bending plane counters
  Counter getBendCounters(uint16_t line) { return counters[line]; }
  /// Check if enough statistics to send
  bool enoughToSend(uint64_t minValue) const
  {
    bool ret = true;
    for (const auto& itCounter : counters) {
      ret &= itCounter.enoughToSend(minValue);
    }
    return ret;
  }

  std::array<Counter, nCounters> counters; ///< array of local board counters

  friend class boost::serialization::access;

  /// Serializes the struct
  template <class Archive>
  void serialize(Archive& ar, const unsigned int version)
  {
    ar& counters;
  }
};

/// A container of 7 StripCounter to describe one detector element
struct CounterStruct {
  /// Number of columns in one detector element
  static const uint16_t nColumns = 7;

  StripCounter& operator[](size_t index)
  {
    if (index > nColumns - 1)
      throw std::out_of_range("Invalid index.");
    return columns[index];
  }

  std::array<StripCounter, nColumns> columns; ///< array of columns in a chamber

  friend class boost::serialization::access;

  /// Serializes the struct
  template <class Archive>
  void serialize(Archive& ar, const unsigned int version)
  {
    ar& columns;
  }
};

/// Defines possible kinds of data
enum dataType { PHYS, DEAD, NOISY, SIZE };

/// A container of Counterstruct to handle different kinds of data
struct CounterContainer {
  /// Get one of the CounterStructs
  CounterStruct& operator[](size_t index) { return countersContainer[index]; }
  /// Check if enough statistics to send
  bool enoughToSend(dataType index, uint64_t minValue) const
  {
    bool ret = true;
    for (const auto& itCounter : countersContainer[(size_t)index].columns) {
      ret &= itCounter.enoughToSend(minValue);
    }
    return ret;
  }

  std::array<CounterStruct, dataType::SIZE> countersContainer; ///< container of counters

  friend class boost::serialization::access;

  /// Serializes the struct
  template <class Archive>
  void serialize(Archive& ar, const unsigned int version)
  {
    ar& countersContainer;
  }
};
} // namespace mid
} // namespace o2

#endif /* O2_MID_STRIPCOUNTER_H */