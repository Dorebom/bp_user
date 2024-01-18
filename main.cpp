#include <iostream>
#include <chrono>
#include <thread>

#include "bp_system/b_system/hub.hpp"

#include "example_node/node/b_simple_node_a.hpp"
#include "example_node/node/b_simple_node_b.hpp"
#include "user_node/node/basic_sub_servo.hpp"
#include "user_node/node/basic_pub_control.hpp"

int main()
{
    std::cout << "[Main]Robot System Start..." << std::endl;

    hub hub_("behavior_hub.json", "../bp_system/_config/");
    hub_.Start();

    std::this_thread::sleep_for(std::chrono::seconds(2));

    b_simple_node_a node_a;
    b_simple_node_b node_b;

    BasicSubServo servo_node;
    BasicPubControl pub_node;

    hub_.store_node(node_a, "b_simple_node_a");
    hub_.store_node(node_b, "b_simple_node_b");

    hub_.store_node(servo_node, "basic_sub_servo");
    hub_.store_node(pub_node, "basic_pub_control");

    hub_.show_stored_node();

    //hub_.exec_node("basic_sub_servo", "sub_servo.json", "../user_node/config/");
    hub_.exec_node("basic_pub_control", "pub_control.json", "../user_node/config/");

    while(1)
    {
        std::this_thread::sleep_for(std::chrono::seconds(3));
        hub_.ChangeStable();
        std::this_thread::sleep_for(std::chrono::seconds(3));
        hub_.End();
        std::this_thread::sleep_for(std::chrono::seconds(1));
        break;
    }
    std::cout << "[Main]Robot System End..." << std::endl;
    return 0;
}
