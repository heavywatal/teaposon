/*! @file dna.hpp
    @brief Interface of DNA class
*/
#pragma once
#ifndef TEK_DNA_HPP_
#define TEK_DNA_HPP_

#include <cstdint>
#include <array>
#include <string>
#include <iostream>

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

namespace tek {

/*! @brief DNA class
*/
template <uint_fast32_t N>
class DNA {
    //! template for translation from integer to character
    static const std::string NUCLEOTIDE;

  public:
    //! diviation from the original
    uint_fast32_t count() const {
        uint_fast32_t cnt = 0u;
        for (const auto x: sequence_) {
            if (x > 0u) ++cnt;
        }
        return cnt;
    }

    //! mutate i-th site
    template <class URBG> inline
    void flip(const uint_fast32_t i, URBG& generator) {
        typename URBG::result_type random_bits = 0u;
        while ((random_bits = generator()) == 0u) {;}
        while ((0b00000011u & random_bits) == 0u) {
            random_bits >>= 2;
        }
        sequence_[i] ^= (0b00000011u & random_bits);
    }

    //! get i-th nucleotide as char
    const char& operator[](const size_t i) const {
        return NUCLEOTIDE[sequence_[i]];
    }

    //! translate integer to nucleotide character and print
    std::ostream& write(std::ostream& ost) const {
        for (const auto x: sequence_) {
            ost << NUCLEOTIDE[x];
        }
        return ost;
    }

    //! shortcut of write()
    friend std::ostream& operator<<(std::ostream& ost, const DNA& x) {
        return x.write(ost);
    }

    //! count different nucleotides
    uint_fast32_t operator-(const DNA<N>& other) const {
        uint_fast32_t diff = 0u;
        for (uint_fast32_t i=0u; i<N; ++i) {
            if (this->sequence_[i] != other.sequence_[i]) ++diff;
        }
        return diff;
    }

  private:
    //! sequence as integer array
    std::array<uint_fast8_t, N> sequence_ = {};
};

template <uint_fast32_t N>
const std::string DNA<N>::NUCLEOTIDE = "ACGT";

//! unit test
template <class URBG> inline
void DNA_test(URBG& generator) {
    constexpr uint_fast32_t N = 30u;
    DNA<N> x, y;
    std::cerr << x << std::endl;
    for (uint_fast32_t i=0u; i<N; ++i) {
        x.flip(i, generator);
    }
    std::cerr << x << std::endl;
    std::cerr << x - y << std::endl;
}

} // namespace tek

#endif /* TEK_DNA_HPP_ */
