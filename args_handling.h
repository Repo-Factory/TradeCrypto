#ifndef ARGS_HANDLING_H
#define ARGS_HANDLING_H

#define N_FLAG_DEFAULT 120
#define X_FLAG_DEFAULT 0
#define Y_FLAG_DEFAULT 0
#define B_FLAG_DEFAULT 0
#define E_FLAG_DEFAULT 0

struct Args
{
    int n_flag = N_FLAG_DEFAULT;
    int x_flag = X_FLAG_DEFAULT;
    int y_flag = Y_FLAG_DEFAULT;
    int b_flag = B_FLAG_DEFAULT;
    int e_flag = E_FLAG_DEFAULT;
};

namespace ArgsHandling
{
    const Args processArgs(int argc, char* argv[]);
}

#endif