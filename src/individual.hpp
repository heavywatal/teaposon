// -*- mode: c++; coding: utf-8 -*-
/*! @file individual.hpp
    @brief Interface of Individual class
*/
#pragma once
#ifndef TEK_INDIVIDUAL_HPP_
#define TEK_INDIVIDUAL_HPP_

#include <iosfwd>
#include <vector>
#include <valarray>
#include <memory>

#include "transposon.hpp"

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

namespace tek {

typedef std::vector<std::shared_ptr<Transposon>> haploid_t;

class Individual {
  public:
    static constexpr unsigned int NUM_SITES = 2000;

    Individual(): chromosomes_{haploid_t(NUM_SITES), haploid_t(NUM_SITES)} {};
    Individual(const haploid_t&, const haploid_t&);

    double fitness() const;
    void transpose();
    void recombine();
    void mutate();

    std::ostream& write(std::ostream&) const;
    friend std::ostream& operator<<(std::ostream&, const Individual&);

    static void unit_test();

  private:
    std::valarray<double> genotype() const;
    double s_cn(const unsigned int) const;

    static constexpr double INDEL_RATIO_ = 0.2;
    static constexpr double TAU_ = 1.5;
    static double XI_;
    static double EXCISION_RATE_;

    std::pair<haploid_t, haploid_t> chromosomes_;
};

} // namespace tek

#endif /* TEK_INDIVIDUAL_HPP_ */
