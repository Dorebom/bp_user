#include "simple_node.hpp"
// Setting file name
#define B_NODE_CONFIG_DIRECTORY "../example_node/config/"
#define B_NODE_CONFIG_FILE "simple_node.json"

simple_node::simple_node(/* args */)
{
    is_main_thread_running_ = false;
    node_state_machine_ = node_state_machine::UNCONFIGURED;
    cmd_node_state_machine_ = node_state_machine::UNCONFIGURED;

    set_config_directory_name(B_NODE_CONFIG_DIRECTORY);
    set_config_file_name(B_NODE_CONFIG_FILE);
}

simple_node::~simple_node()
{
}

void simple_node::_initialize_processing()
{
    node_state_->state_code.state_machine = node_state_machine_;
}
void simple_node::_ready_processing()
{
    cmd_executor();
    node_state_->state_code.state_machine = node_state_machine_;
}
void simple_node::_repair_processing()
{
    cmd_executor();
    node_state_->state_code.state_machine = node_state_machine_;

}
void simple_node::_stable_processing()
{
    node_state_->state_code.state_machine = node_state_machine_;
}
void simple_node::_force_stop_processing()
{
    node_state_->state_code.state_machine = node_state_machine_;
}

void simple_node::_end_processing()
{
}

/* node_state change process */
// -> initialize
bool simple_node::_any_to_initialize_processing() {return true;}
// -> ready (reset process)
bool simple_node::_any_to_ready_processing() {return true;} // stable, repair and ready
// -> force stop
bool simple_node::_any_to_force_stop_processing() {return true;} // stable, repair and ready
// -> normal flow
bool simple_node::_ready_to_repair_processing() {return true;}
bool simple_node::_ready_to_stable_processing() {return true;}
bool simple_node::_repair_to_stable_processing() {return true;}
bool simple_node::_stable_to_repair_processing() {return true;}

bool simple_node::_any_to_initialize_processing_after(){return true;}
bool simple_node::_any_to_ready_processing_after(){return true;}
bool simple_node::_any_to_force_stop_processing_after(){return true;}
bool simple_node::_ready_to_repair_processing_after(){return true;}
bool simple_node::_ready_to_stable_processing_after(){return true;}
bool simple_node::_repair_to_stable_processing_after(){return true;}
bool simple_node::_stable_to_repair_processing_after(){return true;}

void simple_node::_set_config(nlohmann::json json_data) 
{
}
void simple_node::_configure()
{
    node_state_ = std::make_shared<st_node_state>();
    node_cmd_ = std::make_shared<node_cmd>(node_config_.cmd_stack_size);
    node_sys_cmd_ = std::make_shared<node_cmd>(node_config_.sys_cmd_stack_size);
}
void simple_node::_set_state()
{
    node_state_->state_code.state_machine = node_state_machine_;
}

void simple_node::cmd_executor()
{
    if (node_cmd_->cmd_stack_.size() != 0)
    {
        auto cmd = node_cmd_->cmd_stack_.pop();
    }
    else
    {
        // Add warning message
    }
}
