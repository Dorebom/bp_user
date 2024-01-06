#include "basic_joycon_node.hpp"

#include <iostream>
#include <iomanip>
#include <vector>
#include <cstdio>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/joystick.h>

#define JOY_DEV "/dev/input/js0"

using namespace std;

BasicJoyconNode::BasicJoyconNode(/* args */)
: joy_fd(-1),
  num_of_axis(0),
  num_of_buttons(0)
{
    is_main_thread_running_ = false;
    node_state_machine_ = node_state_machine::UNCONFIGURED;
    cmd_node_state_machine_ = node_state_machine::UNCONFIGURED;
}

BasicJoyconNode::~BasicJoyconNode()
{
}


void BasicJoyconNode::_initialize_processing()
{
}
void BasicJoyconNode::_ready_processing()
{
    get_state();
    display_state();
}
void BasicJoyconNode::_repair_processing()
{
    get_state();
    display_state();
}
void BasicJoyconNode::_stable_processing()
{
    get_state();
    display_state();
}
void BasicJoyconNode::_force_stop_processing()
{
    get_state();
    display_state();
}

void BasicJoyconNode::_end_processing()
{
    if(joy_fd!=-1)
    {
        close(joy_fd);
    }
}

/* node_state change process */
// -> initialize
bool BasicJoyconNode::_any_to_initialize_processing() {


    if((joy_fd=open(JOY_DEV,O_RDONLY)) < 0)
    {
        cerr<<"Failed to open "<<JOY_DEV<<endl;
        exit(-1);
    }

    ioctl(joy_fd, JSIOCGAXES, &num_of_axis);
    ioctl(joy_fd, JSIOCGBUTTONS, &num_of_buttons);
    ioctl(joy_fd, JSIOCGNAME(80), &name_of_joystick);

    joy_button.resize(num_of_buttons,0);
    joy_axis.resize(num_of_axis,0);

    print_log("Joystick: " + std::string(name_of_joystick));

    fcntl(joy_fd, F_SETFL, O_NONBLOCK);   // using non-blocking mode

    return true;
}
// -> ready (reset process)
bool BasicJoyconNode::_any_to_ready_processing() {return true;} // stable, repair and ready
// -> force stop
bool BasicJoyconNode::_any_to_force_stop_processing() {return true;} // stable, repair and ready
// -> normal flow
bool BasicJoyconNode::_ready_to_repair_processing() {return true;}
bool BasicJoyconNode::_ready_to_stable_processing() {return true;}
bool BasicJoyconNode::_repair_to_stable_processing() {return true;}
bool BasicJoyconNode::_stable_to_repair_processing() {return true;}

bool BasicJoyconNode::_any_to_initialize_processing_after(){return true;}
bool BasicJoyconNode::_any_to_ready_processing_after(){return true;}
bool BasicJoyconNode::_any_to_force_stop_processing_after(){return true;}
bool BasicJoyconNode::_ready_to_repair_processing_after(){return true;}
bool BasicJoyconNode::_ready_to_stable_processing_after(){return true;}
bool BasicJoyconNode::_repair_to_stable_processing_after(){return true;}
bool BasicJoyconNode::_stable_to_repair_processing_after(){return true;}

void BasicJoyconNode::_set_config(nlohmann::json json_data)
{
}

void BasicJoyconNode::_configure()
{
    node_state_ = std::make_shared<st_node_state>();
    node_cmd_ = std::make_shared<node_cmd>(node_config_.cmd_stack_size);
    node_sys_cmd_ = std::make_shared<node_cmd>(node_config_.cmd_stack_size);
    print_log("BasicJoyconNode Configure");
}

void BasicJoyconNode::_set_state()
{
    node_state_->state_code.state_machine = node_state_machine_;
}

void BasicJoyconNode::get_state()
{
    js_event js;

    read(joy_fd, &js, sizeof(js_event));

    switch (js.type & ~JS_EVENT_INIT)
    {
    case JS_EVENT_AXIS:
        if((int)js.number>=joy_axis.size())
        {
            cerr<<"err:"<<(int)js.number<<endl;
        }
        joy_axis[(int)js.number]= js.value;
      break;
    case JS_EVENT_BUTTON:
        if((int)js.number>=joy_button.size())
        {
            cerr<<"err:"<<(int)js.number<<endl;
        }
        joy_button[(int)js.number]= (int)js.value;
      break;
    }

    joycon_state_ = (st_joycon_state*)node_state_->data;

    for (int i = 0; i < joy_button.size(); i++)
    {
        joycon_state_->button[i] = (int)joy_button[i];
    }

    for (int i = 0; i < joy_axis.size(); i++)
    {
        joycon_state_->axis[i] = (double)joy_axis[i] / 32767.0;
    }

}

void BasicJoyconNode::display_state()
{
    print_log("Joystick: " + std::string(name_of_joystick));
    print_log("Axes: " + std::to_string(num_of_axis));
    print_log("Buttons: " + std::to_string(num_of_buttons));

    print_log("Button Value: "
                + std::to_string(joycon_state_->button[0]) + " "
                + std::to_string(joycon_state_->button[1]) + " "
                + std::to_string(joycon_state_->button[2]) + " "
                + std::to_string(joycon_state_->button[3]) + " "
                + std::to_string(joycon_state_->button[4]) + " "
                + std::to_string(joycon_state_->button[5]) + " "
                + std::to_string(joycon_state_->button[6]) + " "
                + std::to_string(joycon_state_->button[7]) + " "
                + std::to_string(joycon_state_->button[8]) + " "
                + std::to_string(joycon_state_->button[9]) + " "
                + std::to_string(joycon_state_->button[10]) + " "
                + std::to_string(joycon_state_->button[11]));
    print_log("Axis Value: "
                + std::to_string(joycon_state_->axis[0]) + " "
                + std::to_string(joycon_state_->axis[1]) + " "
                + std::to_string(joycon_state_->axis[2]) + " "
                + std::to_string(joycon_state_->axis[3]) + " "
                + std::to_string(joycon_state_->axis[4]) + " "
                + std::to_string(joycon_state_->axis[5]));

}