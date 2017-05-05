// -*- mode: c++; coding: utf-8 -*-
/*! @file transposon.hpp
    @brief Interface of Transposon class
*/
#pragma once
#ifndef TEK_TRANSPOSON_HPP_
#define TEK_TRANSPOSON_HPP_

#include <iosfwd>
#include <bitset>

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

namespace tek {

class Transposon {
  public:
    Transposon() = default;

    std::ostream& write(std::ostream&) const;
    friend std::ostream& operator<<(std::ostream&, const Transposon&);

    static void unit_test();

  private:
    static constexpr unsigned int SEQUENCE_LENGTH_ = 200;
    static double ALPHA_;
    static unsigned int BETA_;

    std::bitset<SEQUENCE_LENGTH_> sequence_;
};

} // namespace tek

#endif /* TEK_TRANSPOSON_HPP_ */
