// -*- mode: c++; coding: utf-8 -*-
/*! @file transposon.hpp
    @brief Interface of Transposon class
*/
#pragma once
#ifndef TEK_TRANSPOSON_HPP_
#define TEK_TRANSPOSON_HPP_

#include <boost/program_options.hpp>

#include <iosfwd>
#include <bitset>
#include <array>
#include <random>

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

namespace tek {

class Transposon {
  public:
    static constexpr size_t LENGTH = 300;
    static constexpr size_t NUM_SYNONYMOUS_SITES = LENGTH / 3;
    static constexpr size_t NUM_NONSYNONYMOUS_SITES = LENGTH - NUM_SYNONYMOUS_SITES;
    static constexpr double OVER_SYNONYMOUS_SITES = 1.0 / NUM_SYNONYMOUS_SITES;
    static constexpr double OVER_NONSYNONYMOUS_SITES = 1.0 / NUM_NONSYNONYMOUS_SITES;
    static constexpr double MAX_TRANSPOSITION_RATE = 0.01;

    Transposon() = default;

    template <class URNG> inline
    void mutate(URNG& generator) {
        static std::uniform_int_distribution<size_t> UNIF_LEN(0U, LENGTH - 1U);
        size_t pos = UNIF_LEN(generator);
        if (pos >= NUM_NONSYNONYMOUS_SITES) {
            synonymous_sites_.flip(pos -= NUM_NONSYNONYMOUS_SITES);
        } else {
            nonsynonymous_sites_.flip(pos);
        }
    }

    void indel() {has_indel_ = true;}
    double activity() const {
        if (has_indel_) return 0.0;
        return ACTIVITY_[nonsynonymous_sites_.count()];
    }
    double transposition_rate() const {
        return MAX_TRANSPOSITION_RATE * activity();
    }

    bool has_indel() const {return has_indel_;}
    double dn() const {return nonsynonymous_sites_.count() * OVER_NONSYNONYMOUS_SITES;}
    double ds() const {return synonymous_sites_.count() * OVER_SYNONYMOUS_SITES;}

    std::ostream& write_summary(std::ostream&) const;
    std::ostream& write_fasta(std::ostream&, const unsigned int copy_number=0) const;
    std::ostream& write_sequence(std::ostream&) const;
    friend std::ostream& operator<<(std::ostream&, const Transposon&);

    static void set_parameters();
    static boost::program_options::options_description options_desc();
    static void test();

  private:

    static double calc_activity(const size_t num_mutations);
    static void test_activity();
    static void test_activity(std::ostream&, const double alpha, const unsigned int beta);

    static double ALPHA_;
    static double THRESHOLD_;
    static unsigned int BETA_;
    static std::array<double, NUM_NONSYNONYMOUS_SITES> ACTIVITY_;

    std::bitset<NUM_NONSYNONYMOUS_SITES> nonsynonymous_sites_;
    std::bitset<LENGTH - NUM_NONSYNONYMOUS_SITES> synonymous_sites_;
    bool has_indel_ = false;
};

} // namespace tek

#endif /* TEK_TRANSPOSON_HPP_ */
