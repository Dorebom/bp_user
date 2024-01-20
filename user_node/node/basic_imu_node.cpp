#include "basic_imu_node.hpp"

#include <chrono>

#include "../data_struct/cmd/st_imu_cmd.hpp"

BasicImuNode::BasicImuNode(/* args */)
{
    is_main_thread_running_ = false;
    node_state_machine_ = node_state_machine::UNCONFIGURED;
    cmd_node_state_machine_ = node_state_machine::UNCONFIGURED;

    is_display_state_ = false;
    s_cDataUpdate = 0;
    s_iCurBaud = 9600;
}

BasicImuNode::~BasicImuNode()
{
}

void BasicImuNode::_initialize_processing()
{
    char cBuff[1];
    if (serial_read_data(fd, (unsigned char*)cBuff, 1))
    {
        WitSerialDataIn(cBuff[0]);
    }
}
void BasicImuNode::_ready_processing()
{
    get_state();
    display_state();
}
void BasicImuNode::_repair_processing()
{
    get_state();
    display_state();
}
void BasicImuNode::_stable_processing()
{
    get_state();
    display_state();
}
void BasicImuNode::_force_stop_processing()
{
    get_state();
    display_state();
}

void BasicImuNode::_end_processing()
{
    serial_close(fd);
}

/* node_state change process */
// -> initialize
bool BasicImuNode::_any_to_initialize_processing() {

    if((fd = serial_open((unsigned char*)portName.c_str(), 9600)) < 0)
    {
        printf("open %s fail\n", portName.c_str());
        return false;
    }
    else
    {
        printf("open %s success\n", portName.c_str());
    }

    WitInit(WIT_PROTOCOL_NORMAL, 0x50);
	WitRegisterCallBack(SensorDataUpdata);

    print_log("BasicImuNode Initialize");
    AutoScanSensor((char *)portName.c_str());

    return true;
}
// -> ready (reset process)
bool BasicImuNode::_any_to_ready_processing() {return true;} // stable, repair and ready
// -> force stop
bool BasicImuNode::_any_to_force_stop_processing() {return true;} // stable, repair and ready
// -> normal flow
bool BasicImuNode::_ready_to_repair_processing() {return true;}
bool BasicImuNode::_ready_to_stable_processing() {return true;}
bool BasicImuNode::_repair_to_stable_processing() {return true;}
bool BasicImuNode::_stable_to_repair_processing() {return true;}

bool BasicImuNode::_any_to_initialize_processing_after(){return true;}
bool BasicImuNode::_any_to_ready_processing_after(){return true;}
bool BasicImuNode::_any_to_force_stop_processing_after(){return true;}
bool BasicImuNode::_ready_to_repair_processing_after(){return true;}
bool BasicImuNode::_ready_to_stable_processing_after(){return true;}
bool BasicImuNode::_repair_to_stable_processing_after(){return true;}
bool BasicImuNode::_stable_to_repair_processing_after(){return true;}

void BasicImuNode::_set_config(nlohmann::json json_data)
{
    baudRate = json_data.at("baudRate");
    portName = json_data.at("portName");
}

void BasicImuNode::_configure()
{
    node_state_ = std::make_shared<st_node_state>();
    node_cmd_ = std::make_shared<node_cmd>(node_config_.cmd_stack_size);
    node_sys_cmd_ = std::make_shared<node_cmd>(node_config_.cmd_stack_size);
    imu_state_ = (st_imu_state*)node_state_->data;
    print_log("BasicImuNode Configure");

    print_log("port name: " + portName);
    print_log("baud rate: " + std::to_string(baudRate));
    serial_manager_.setPortName(portName.c_str());
    serial_manager_.setBaudRate(baudRate);
}

void BasicImuNode::_set_state()
{
    node_state_->state_code.state_machine = node_state_machine_;
}

void BasicImuNode::get_state()
{
    char cBuff[1];
    int i;

    while(serial_read_data(fd, (unsigned char*)cBuff, 1))
	{
		WitSerialDataIn(cBuff[0]);
	}

    if(s_cDataUpdate)
    {
        for(i = 0; i < 3; i++)
        {
            imu_state_->linear_acceleration[i] = sReg[AX+i] / 32768.0f * 16.0f;
            imu_state_->angular_velocity[i] = sReg[GX+i] / 32768.0f * 2000.0f;
            imu_state_->orientation[i] = sReg[Roll+i] / 32768.0f * 180.0f;
        }
        imu_state_->mag[0] = sReg[HX];
        imu_state_->mag[1] = sReg[HY];
        imu_state_->mag[2] = sReg[HZ];
    }
}

void BasicImuNode::display_state()
{
    if (is_display_state_)
    {
        if(s_cDataUpdate & ACC_UPDATE)
        {
            print_log("acc:"
                + std::to_string(imu_state_->linear_acceleration[0]) + " "
                + std::to_string(imu_state_->linear_acceleration[1]) + " "
                + std::to_string(imu_state_->linear_acceleration[2]));
            s_cDataUpdate &= ~ACC_UPDATE;
        }
        if(s_cDataUpdate & GYRO_UPDATE)
        {
            print_log("gyro:"
                + std::to_string(imu_state_->angular_velocity[0]) + " "
                + std::to_string(imu_state_->angular_velocity[1]) + " "
                + std::to_string(imu_state_->angular_velocity[2]));
            s_cDataUpdate &= ~GYRO_UPDATE;
        }
        if(s_cDataUpdate & ANGLE_UPDATE)
        {
            print_log("angle:"
                + std::to_string(imu_state_->orientation[0]) + " "
                + std::to_string(imu_state_->orientation[1]) + " "
                + std::to_string(imu_state_->orientation[2]));
            s_cDataUpdate &= ~ANGLE_UPDATE;
        }
        if(s_cDataUpdate & MAG_UPDATE)
        {
            print_log("mag:"
                + std::to_string(imu_state_->mag[0]) + " "
                + std::to_string(imu_state_->mag[1]) + " "
                + std::to_string(imu_state_->mag[2]));
            s_cDataUpdate &= ~MAG_UPDATE;
        }
    }
}

void BasicImuNode::user_cmd_executor()
{
    st_node_cmd cmd;
    if (node_cmd_->cmd_stack_.size() > 0)
    {
        cmd = node_cmd_->cmd_stack_.pop();
        print_log("[cmd] " + std::to_string(cmd.cmd_code.cmd_type));
        switch ((imu_cmd_list)cmd.cmd_code.cmd_type)
        {
        case imu_cmd_list::DISPLAY_STATE:
            print_log("[cmd] DISPLAY_STATE");
            if (is_display_state_)
            {
                is_display_state_ = false;
            }
            else
            {
                is_display_state_ = true;
            }
            break;
        case imu_cmd_list::CHANGE_SM_READY:
            print_log("[cmd] CHANGE_SM_READY");
            ChangeReady();
            break;
        case imu_cmd_list::CHANGE_SM_STABLE:
            print_log("[cmd] CHANGE_SM_STABLE");
            ChangeStable();
            break;
        case imu_cmd_list::CHANGE_SM_REPAIR:
            print_log("[cmd] CHANGE_SM_REPAIR");
            ChangeRepair();
            break;
        case imu_cmd_list::CHANGE_SM_FORCE_STOP:
            print_log("[cmd] CHANGE_SM_FORCE_STOP");
            PushForceStop();
            break;
        case imu_cmd_list::RELEASE_FORCE_STOP:
            print_log("[cmd] RELEASE_FORCE_STOP");
            ReleaseForceStop();
            break;
        default:
            break;
        }
    }
}

void BasicImuNode::SensorDataUpdata(uint32_t uiReg, uint32_t uiRegNum)
{
    int i;
    for(i = 0; i < uiRegNum; i++)
    {
        switch(uiReg)
        {
//            case AX:
//            case AY:
            case AZ:
				s_cDataUpdate |= ACC_UPDATE;
            break;
//            case GX:
//            case GY:
            case GZ:
				s_cDataUpdate |= GYRO_UPDATE;
            break;
//            case HX:
//            case HY:
            case HZ:
				s_cDataUpdate |= MAG_UPDATE;
            break;
//            case Roll:
//            case Pitch:
            case Yaw:
				s_cDataUpdate |= ANGLE_UPDATE;
            break;
            default:
				s_cDataUpdate |= READ_UPDATE;
			break;
        }
		uiReg++;
    }
}

void BasicImuNode::AutoScanSensor(char *dev)
{
    int i, iRetry;
	char cBuff[1];

	for(i = 1; i < 10; i++)
	{
		serial_close(fd);
		s_iCurBaud = c_uiBaud[i];
		fd = serial_open((unsigned char*)dev, c_uiBaud[i]);

		iRetry = 2;
		do
		{
			s_cDataUpdate = 0;
			WitReadReg(AX, 3);
			Delayms(200);
			while(serial_read_data(fd, (unsigned char*)cBuff, 1))
			{
				WitSerialDataIn(cBuff[0]);
			}
			if(s_cDataUpdate != 0)
			{
				printf("%d baud find sensor\r\n\r\n", c_uiBaud[i]);
				return ;
			}
			iRetry--;
		}while(iRetry);
	}
	printf("can not find sensor\r\n");
	printf("please check your connection\r\n");

}

void BasicImuNode::Delayms(uint16_t ucMs)
{
     usleep(ucMs*1000);
}

void BasicImuNode::set_param(st_imu_param_cmd param)
{
}

