/* 
 * @brief Class Handles Command line argument parsing and validation
 *
 * Args object made of optional and mandatory args structs
 * Struct will lay out default values for flags if none are passed in
 * Getopt will be used to parse optional arguments if given
 * Appropriate error messages will be displayed for incorrect usages
 * Program mandates two arguments for the file inputs
 * See macros for more usage details
 * 
 */

#include "args_handling.h"
#include <getopt.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#define OPT_INVALID -1
#define N_FLAG_IDENTIFER 'n'
#define X_FLAG_IDENTIFER 'x'
#define Y_FLAG_IDENTIFER 'y'
#define B_FLAG_IDENTIFER 'b'
#define E_FLAG_IDENTIFER 'e'
#define FLAG_OPTIONS "n:x:y:b:e:"
#define OUT_OF_RANGE_FLAG(flag) flag<1
#define INVALID_FLAG_ERROR_MESSAGE "Flag value must be greater than 1"

namespace // Validation methods will check a condition, if it is not met we will print the appropriate message and exit the program.
{
    void validate_flag(const int flag)
    {
        if (OUT_OF_RANGE_FLAG(flag)) {
            printf(INVALID_FLAG_ERROR_MESSAGE);
            exit(EXIT_FAILURE);
        }
    }
}

namespace
{
    /**
     * Use optargs to populate n:f:a:l flags
     */
    Args getOptionalArgs(int argc, char* argv[])
    {
        Args optionalArgs;
        int option_index = 0;
        while ((option_index = getopt(argc, argv, FLAG_OPTIONS)) != OPT_INVALID) 
        {
            switch (option_index) 
            {
                case N_FLAG_IDENTIFER:
                    optionalArgs.n_flag = std::stoi(optarg);
                    break;
                case X_FLAG_IDENTIFER:
                    optionalArgs.x_flag = std::stoi(optarg);
                    break;
                case Y_FLAG_IDENTIFER:
                    optionalArgs.b_flag = std::stoi(optarg);
                    break;
                case B_FLAG_IDENTIFER:
                    optionalArgs.y_flag = std::stoi(optarg);
                    break;
                case E_FLAG_IDENTIFER:
                    optionalArgs.e_flag = std::stoi(optarg);
                    break;
                default:
                    validate_flag(std::stoi(optarg));
            }
        }
        return optionalArgs;
    }
}

const Args ArgsHandling::processArgs(int argc, char* argv[])
{
    return Args{getOptionalArgs(argc, argv)};
}
