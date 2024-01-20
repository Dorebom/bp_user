#pragma once

// Base node, cmd and state
#include "b_system/b_node.hpp"
#include "_common/node_cmd.hpp"
#include "_common/node_state.hpp"
// List
#include "b_system/b_hub_cmd/b_hub_cmd_list.hpp"

#include "../data_struct/state/st_imu_state.hpp"
#include "../data_struct/cmd/st_imu_cmd.hpp"

#include "witmotion/serial.hpp"
#include "witmotion/wit_c_sdk.hpp"
#include "witmotion/REG.h"
#include <stdint.h>

#include "../common/comm/Serial/SerialManager.hpp"

/*
https://wit-motion.gitbook.io/witmotion-sdk/wit-standard-protocol/sdk/linux-c_sdk_normal-onboard-serial-port-and-3-in-1-serial-port-usage-tutorial
*/
#define ACC_UPDATE		0x01
#define GYRO_UPDATE		0x02
#define ANGLE_UPDATE	0x04
#define MAG_UPDATE		0x08
#define READ_UPDATE		0x80

static volatile char s_cDataUpdate = 0;
static int fd, s_iCurBaud;

const int c_uiBaud[] = {2400 , 4800 , 9600 , 19200 , 38400 , 57600 , 115200 , 230400 , 460800 , 921600};

class BasicImuNode : public b_node
{
private:
    /* node_state loop process */
    void _initialize_processing() override;
    void _ready_processing() override;
    void _repair_processing() override;
    void _stable_processing() override;
    void _force_stop_processing() override;
    void _end_processing() override;
    //void transit_processing() override;
    /* node state change process */
    // -> initialize
    bool _any_to_initialize_processing() override;
    // -> ready (reset process)
    bool _any_to_ready_processing() override; // stable, repair and ready
    // -> force stop
    bool _any_to_force_stop_processing() override; // stable, repair and ready
    // -> normal flow
    bool _ready_to_repair_processing() override;
    bool _ready_to_stable_processing() override;
    bool _repair_to_stable_processing() override;
    bool _stable_to_repair_processing() override;
    // -> after transiting
    bool _any_to_initialize_processing_after() override;
    bool _any_to_ready_processing_after() override;
    bool _any_to_force_stop_processing_after() override;
    bool _ready_to_repair_processing_after() override;
    bool _ready_to_stable_processing_after() override;
    bool _repair_to_stable_processing_after() override;
    bool _stable_to_repair_processing_after() override;

    void _configure() override;
    void _set_config(nlohmann::json json_data) override;
    void _set_state() override;
    //void cmd_executor();

    /* user data */
    st_imu_state* imu_state_;
    bool is_display_state_;
    SerialManager serial_manager_;
    std::string portName;
    unsigned int baudRate;

    /* user function */
    void get_state();
    void display_state();
    void user_cmd_executor();
    void set_param(st_imu_param_cmd param);
    static void SensorDataUpdata(uint32_t uiReg, uint32_t uiRegNum);
    static void AutoScanSensor(char* dev);
    static void Delayms(uint16_t ucMs);
public:
    BasicImuNode(/* args */);
    ~BasicImuNode();
    std::shared_ptr<b_node> Clone() const override{
        return std::make_shared<BasicImuNode>(*this);
    }
};  // class simple_node_a