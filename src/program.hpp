/*! @file program.hpp
    @brief Interface of Program class
*/
#pragma once
#ifndef TEK_PROGRAM_HPP_
#define TEK_PROGRAM_HPP_

#include <vector>
#include <string>

namespace boost {namespace program_options {class options_description;}}

namespace tek {

/*! @brief Program class
*/
class Program {
  public:
    //! Parse command arguments
    Program(const std::vector<std::string>& args);
    //! Parse command arguments
    Program(int argc, char* argv[])
    : Program(std::vector<std::string>(argv, argv + argc)) {}

    //! Top level function that should be called once from global main
    void run();

  private:
    //! called from run()
    void main();
    //! options description for Program class
    boost::program_options::options_description options_desc();

    /////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
    //! @addtogroup params
    //! @{

    //! \f$N\f$, population size
    size_t popsize_ = 500u;
    //! @} params
    /////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

    //! initial number of individuals with TE
    size_t initial_freq_ = 1u;
    //! maximum number of generations to simulate
    size_t num_generations_ = 1000u;
    //! number of generations to simulate after population split
    size_t num_generations_after_split_ = 0u;
    //! interval of recording
    size_t record_interval_ = 10u;
    //! enum Recording
    int record_flags_ = 3;
    //! name of output directory
    std::string outdir_;
    //! writen to "program_options.conf"
    std::string config_string_;
};

} // namespace tek

#endif /* TEK_PROGRAM_HPP_ */
