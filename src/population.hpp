// -*- mode: c++; coding: utf-8 -*-
/*! @file population.hpp
    @brief Interface of Population class
*/
#pragma once
#ifndef TEK_POPULATION_HPP_
#define TEK_POPULATION_HPP_

#include <iosfwd>
#include <vector>
#include <map>

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

namespace tek {

class Haploid;

class Population {
  public:
    static constexpr double THETA = 0.01;
    static constexpr double RHO = 200;

    Population(const size_t size, const size_t num_founders=1, const unsigned int concurrency=1);
    bool evolve(const size_t max_generations, const size_t record_interval);

    std::ostream& write_summary(std::ostream&) const;
    std::ostream& write_fasta(std::ostream&) const;
    std::ostream& write_individual(std::ostream&, const size_t i=0) const;
    std::ostream& write(std::ostream&) const;
    friend std::ostream& operator<<(std::ostream&, const Population&);

    static void test();
  private:
    std::vector<double> step(const double previous_max_fitness=1.0);

    bool is_extinct() const;

    std::vector<Haploid> gametes_;
    const unsigned int concurrency_;
};

} // namespace tek

#endif /* TEK_POPULATION_HPP_ */
