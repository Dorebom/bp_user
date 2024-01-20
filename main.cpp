#include <iostream>
#include <chrono>
#include <thread>

#include "bp_system/b_system/hub.hpp"

#include "example_node/node/b_simple_node_a.hpp"
#include "example_node/node/b_simple_node_b.hpp"
#include "user_node/node/basic_imu_node.hpp"

int main()
{
    std::cout << "[Main]Robot System Start..." << std::endl;

    hub hub_("behavior_hub.json", "../bp_system/_config/");
    hub_.Start();

    std::this_thread::sleep_for(std::chrono::seconds(2));
    //b_hub_.exec_node(behavior_node_list::SIMPLE_NODE_A, behavior_node_list::HUB);

    b_simple_node_a node_a;
    b_simple_node_b node_b;

    BasicImuNode imu_node;

    //hub_.store_node(node_a, "b_simple_node_a");
    //hub_.store_node(node_b, "b_simple_node_b");

    hub_.store_node(imu_node, "basic_imu_node");

    hub_.show_stored_node();

    //hub_.exec_node("b_simple_node_a", "b_simple_node_a.json", "../bp_system/example_node/config/");

    hub_.exec_node("basic_imu_node", "basic_imu_node.json", "../user_node/config/");

    //b_hub_.exec_node(behavior_node_list::EXAMPLE_PUB_CONTROL, behavior_node_list::HUB);

    while(1)
    {
        std::this_thread::sleep_for(std::chrono::seconds(3));
        hub_.ChangeStable();
        std::this_thread::sleep_for(std::chrono::seconds(10));
        hub_.End();
        std::this_thread::sleep_for(std::chrono::seconds(1));
        break;
    }
    std::cout << "[Main]Robot System End..." << std::endl;
    return 0;
}
