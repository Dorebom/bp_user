#pragma once

enum class imu_cmd_list : int
{
    DISPLAY_STATE = 0,
    CHANGE_SM_READY,
    CHANGE_SM_STABLE,
    CHANGE_SM_REPAIR,
    CHANGE_SM_FORCE_STOP,
    RELEASE_FORCE_STOP,

    PERIOD_CMD // この行は削除しないこと
};

struct st_imu_param_cmd
{
    /* data */
    imu_cmd_list cmd_type;
};
