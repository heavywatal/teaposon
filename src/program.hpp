// -*- mode: c++; coding: utf-8 -*-
/*! @file program.hpp
    @brief Interface of Program class
*/
#pragma once
#ifndef TEK_PROGRAM_HPP_
#define TEK_PROGRAM_HPP_

#include <vector>
#include <string>

#include <wtl/exception.hpp>

namespace boost {
    namespace program_options {
        class options_description;
    }
}

namespace tek {

/*! @brief Represents single run
*/
class Program {
  public:
    //! Parse command arguments
    Program(const std::vector<std::string>& args);

    //! Top level function that should be called once from main()
    void run();

    /////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
  private:
    boost::program_options::options_description options_desc();
    boost::program_options::options_description positional_desc();
    void help_and_exit();

    size_t popsize_ = 42;
    size_t num_repeats_ = 1;
};

} // namespace tek

#endif /* TEK_PROGRAM_HPP_ */
