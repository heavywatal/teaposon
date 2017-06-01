// -*- mode: c++; coding: utf-8 -*-
/*! @file haploid.cpp
    @brief Implementation of Haploid class
*/
#include "haploid.hpp"
#include "transposon.hpp"

#include <wtl/debug.hpp>
#include <wtl/iostr.hpp>
#include <wtl/prandom.hpp>

#include <cmath>
#include <iostream>
#include <numeric>

namespace tek {

double Haploid::XI_ = 1e-4;
double Haploid::EXCISION_RATE_ = 1e-6;
double Haploid::MEAN_SELECTION_COEF_ = 1e-4;
std::valarray<double> Haploid::SELECTION_COEFS_GP_(Haploid::NUM_SITES);
std::poisson_distribution<> Haploid::NUM_MUTATIONS_DIST_(1.0);
std::poisson_distribution<> Haploid::NUM_CHIASMATA_DIST_(1.0);
std::bernoulli_distribution Haploid::INDEL_DIST_(0.5);
std::bernoulli_distribution Haploid::EXCISION_DIST_(0.5);
std::shared_ptr<Transposon> Haploid::ORIGINAL_TE_ = std::make_shared<Transposon>();

namespace po = boost::program_options;

po::options_description Haploid::options_desc() {HERE;
    po::options_description description("Haploid");
    description.add_options()
      ("xi", po::value(&XI_)->default_value(XI_))
      ("nu", po::value(&EXCISION_RATE_)->default_value(EXCISION_RATE_))
      ("lambda", po::value(&MEAN_SELECTION_COEF_)->default_value(MEAN_SELECTION_COEF_));
    return description;
}

void Haploid::set_SELECTION_COEFS_GP() {HERE;
    std::bernoulli_distribution bernoulli(PROP_FUNCTIONAL_SITES_);
    std::exponential_distribution<double> expo_dist(1.0 / MEAN_SELECTION_COEF_);
    for (size_t i=0; i<NUM_SITES; ++i) {
        if (bernoulli(wtl::sfmt())) {
            SELECTION_COEFS_GP_[i] = expo_dist(wtl::sfmt());
        }
    }
}

void Haploid::set_parameters(const size_t popsize, const double theta, const double rho) {HERE;
    const double mu = Transposon::LENGTH * theta / popsize / 4.0;
    const double c = rho / popsize / 4.0;
    EXCISION_DIST_.param(decltype(EXCISION_DIST_)::param_type(EXCISION_RATE_));
    NUM_CHIASMATA_DIST_.param(decltype(NUM_CHIASMATA_DIST_)::param_type(c * NUM_SITES)); // -1?
    NUM_MUTATIONS_DIST_.param(decltype(NUM_MUTATIONS_DIST_)::param_type(mu));
    INDEL_DIST_.param(decltype(INDEL_DIST_)::param_type(mu * INDEL_RATIO_));
    set_SELECTION_COEFS_GP();
}

size_t Haploid::random_index() {
    static std::uniform_int_distribution<size_t> SITES_DIST(0, Haploid::NUM_SITES - 1U);
    return SITES_DIST(wtl::sfmt());
}

Haploid Haploid::copy_founder() {
    // TODO: avoid functional site?
    static auto idx = random_index();
    Haploid founder;
    founder.sites_[idx] = ORIGINAL_TE_;
    return founder;
}

double Haploid::fitness(const Haploid& other) const {
    unsigned int copy_number = 0;
    double prod_1_zs = 1.0;
    for (size_t j=0; j<NUM_SITES; ++j) {
        double one_zs = 1.0;
        if (this->sites_[j]) {++copy_number; one_zs -= SELECTION_COEFS_GP_[j];}
        if (other.sites_[j]) {++copy_number; one_zs -= SELECTION_COEFS_GP_[j];}
        prod_1_zs *= one_zs;
    }
    const double s_cn = XI_ * std::pow(copy_number, TAU_);
    return std::max(prod_1_zs * (1.0 - s_cn), 0.0);
}

bool Haploid::has_transposon() const {
    for (const auto& p: sites_) {
        if (p) return true;
    }
    return false;
}

std::vector<std::shared_ptr<Transposon>> Haploid::transpose() {
    std::vector<std::shared_ptr<Transposon>> copying_transposons;
    for (auto& p: sites_) {
        if (!p) continue;
        std::bernoulli_distribution bern(p->transposition_rate());
        if (bern(wtl::sfmt())) {
            copying_transposons.push_back(p);
        }
        if (EXCISION_DIST_(wtl::sfmt())) {p.reset();}
    }
    return copying_transposons;
}

void Haploid::transpose(Haploid& other) {
    static std::bernoulli_distribution COIN_DIST(0.5);
    auto copying_transposons = this->transpose();
    {
        auto tmp = other.transpose();
        copying_transposons.insert(copying_transposons.end(),
            std::make_move_iterator(tmp.begin()),
            std::make_move_iterator(tmp.end()));
    }
    for (auto& p: copying_transposons) {
        if (COIN_DIST(wtl::sfmt())) {
            auto& dest = this->sites_[random_index()];
            if (dest) continue;
            dest = std::move(p);
        } else {
            auto& dest = other.sites_[random_index()];
            if (dest) continue;
            dest = std::move(p);
        }
    }
    // TODO: should we choose targets from empty sites?
}

void Haploid::recombine(Haploid& other) {
    const size_t num_chiasmata = NUM_CHIASMATA_DIST_(wtl::sfmt());
    if (num_chiasmata == 0U) return;
    std::vector<size_t> positions(num_chiasmata + 1U);
    for (size_t i=0; i<num_chiasmata; ++i) {
        positions[i] = random_index();
    }
    positions[num_chiasmata] = -1U;
    std::sort(positions.begin(), positions.end());
    bool flg = false;
    for (size_t i=0, j=0; j<NUM_SITES; ++j) {
        if (j == positions[i]) {
            flg ^= flg;
            ++i;
        }
        if (flg) {
            auto tmp = sites_[j];
            sites_[j] = other.sites_[j];
            other.sites_[j] = tmp;
        }
    }
}

void Haploid::mutate() {
    for (auto& p: sites_) {
        if (!p) continue;
        const int num_mutations = NUM_MUTATIONS_DIST_(wtl::sfmt());
        const bool is_deactivating = INDEL_DIST_(wtl::sfmt());
        if (num_mutations > 0 || is_deactivating) {
            p = std::make_shared<Transposon>(*p);
        }
        for (int i=0; i<num_mutations; ++i) {
            p->mutate();
        }
        if (is_deactivating) {
            p->indel();
        }
    }
}

void Haploid::count_activities(std::map<double, unsigned int>* const counter) const {
    for (const auto& p: sites_) {
        if (p) {
            ++counter->operator[](p->activity());
        }
    }
}

std::ostream& Haploid::write_sample(std::ostream& ost) const {
    for (const auto& p: sites_) {
        if (p) {
            ost << *p << "\t" << p->ds() << "\t" << p->dn() << "\n";
        }
    }
    return ost;
}

std::ostream& Haploid::write(std::ostream& ost) const {
    for (const auto& p: sites_) {
        if (p) p->write_summary(ost);
    }
    return ost;
}

std::ostream& operator<<(std::ostream& ost, const Haploid& x) {
    return x.write(ost);
}

void Haploid::unit_test() {HERE;
    Haploid x = Haploid::copy_founder();
    std::cout << x << std::endl;
    x.write_sample(std::cout);
    std::cout << "max(s_gpj): " << *std::max_element(std::begin(SELECTION_COEFS_GP_), std::end(SELECTION_COEFS_GP_)) << std::endl;
}

} // namespace tek
