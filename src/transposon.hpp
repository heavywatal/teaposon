// -*- mode: c++; coding: utf-8 -*-
/*! @file transposon.hpp
    @brief Interface of Transposon class
*/
#pragma once
#ifndef TEK_TRANSPOSON_HPP_
#define TEK_TRANSPOSON_HPP_

#include "dna.hpp"

#include <boost/program_options.hpp>

#include <iosfwd>
#include <array>
#include <random>

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

namespace tek {

/*! @brief Transposon class
*/
class Transposon {
  public:
    //! number of base pairs
    static constexpr uint_fast32_t LENGTH = 300;
    //! number of synonymous sites
    static constexpr uint_fast32_t NUM_SYNONYMOUS_SITES = LENGTH / 3;
    //! number of nonsynonymous sites
    static constexpr uint_fast32_t NUM_NONSYNONYMOUS_SITES = LENGTH - NUM_SYNONYMOUS_SITES;
    //! resiprocal of synonymous sites
    static constexpr double OVER_SYNONYMOUS_SITES = 1.0 / NUM_SYNONYMOUS_SITES;
    //! resiprocal of nonsynonymous sites
    static constexpr double OVER_NONSYNONYMOUS_SITES = 1.0 / NUM_NONSYNONYMOUS_SITES;
    //! basal value of transposition rate
    static constexpr double MAX_TRANSPOSITION_RATE = 0.01;

    //! default constructor
    Transposon() = default;

    template <class URNG> inline
    void mutate(URNG& generator) {
        static std::uniform_int_distribution<uint_fast32_t> UNIF_LEN(0U, LENGTH - 1U);
        uint_fast32_t pos = UNIF_LEN(generator);
        if (pos >= NUM_NONSYNONYMOUS_SITES) {
            synonymous_sites_.flip(pos -= NUM_NONSYNONYMOUS_SITES, generator);
        } else {
            nonsynonymous_sites_.flip(pos, generator);
        }
    }

    template <class URNG> inline
    void speciate(URNG& generator) {species_ = generator();}

    void indel() {has_indel_ = true;}
    double activity() const {
        if (has_indel_) return 0.0;
        return ACTIVITY_[nonsynonymous_sites_.count()];
    }
    double transposition_rate() const {
        return MAX_TRANSPOSITION_RATE * activity();
    }

    bool has_indel() const {return has_indel_;}
    uint_fast32_t species() const {return species_;}
    double dn() const {return nonsynonymous_sites_.count() * OVER_NONSYNONYMOUS_SITES;}
    double ds() const {return synonymous_sites_.count() * OVER_SYNONYMOUS_SITES;}

    std::ostream& write_summary(std::ostream&) const;
    std::ostream& write_fasta(std::ostream&, const uint_fast32_t copy_number=0) const;
    std::ostream& write_sequence(std::ostream&) const;
    //! shortcut for write_summary()
    friend std::ostream& operator<<(std::ostream&, const Transposon&);

    static void set_parameters();
    static boost::program_options::options_description options_desc();
    static void test();

  private:

    static double calc_activity(const uint_fast32_t num_mutations);
    static void test_activity();
    static void test_activity(std::ostream&, const double alpha, const unsigned int beta);

    //! intercept of sequence identity to make activity zero
    static double ALPHA_;
    //! 1 - ALPHA_
    static double THRESHOLD_;
    //! exponent of activity curve
    static unsigned int BETA_;
    //! pre-calculated activity values
    static std::array<double, NUM_NONSYNONYMOUS_SITES> ACTIVITY_;

    DNA<NUM_NONSYNONYMOUS_SITES> nonsynonymous_sites_;
    DNA<LENGTH - NUM_NONSYNONYMOUS_SITES> synonymous_sites_;
    bool has_indel_ = false;
    uint_fast32_t species_ = 0;
};

} // namespace tek

#endif /* TEK_TRANSPOSON_HPP_ */
