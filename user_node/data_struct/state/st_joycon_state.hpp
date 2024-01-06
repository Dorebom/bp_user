#pragma once

#include <cmath>
#include <vector>

#include "_common/node_state.hpp"

struct st_joycon_state
{
    /* data */
    node_state_machine state_machine;
    int error_count;
    int alert_count;
    int button[12];
    double axis[6];

    st_joycon_state(/* args */)
    {
        state_machine = node_state_machine::UNCONFIGURED;
        error_count = 0;
        alert_count = 0;

        for (int i = 0; i < 12; i++)
        {
            button[i] = 0;
        }
        for (int i = 0; i < 6; i++)
        {
            axis[i] = 0.0;
        }
    }
};
