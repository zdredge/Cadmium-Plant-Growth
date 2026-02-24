#include <iostream>
#include <string>
#include <cadmium/modeling/devs/coupled.hpp>
#include <cadmium/simulation/root_coordinator.hpp>
#include <cadmium/simulation/logger/stdout.hpp>
#include <cadmium/lib/iestream.hpp>
#include "stem.hpp"

using namespace cadmium;

// Experiment 1: Growth Threshold
// Send 10 consecutive WATER (1) signals on in_response.
// Expect out_event = 1 (growth) on the 10th signal.
struct top_test_stem_growth : public Coupled {

    Port<int> out_request_test;
    Port<int> out_event_test;

    top_test_stem_growth(const std::string& id) : Coupled(id) {
        out_request_test = addOutPort<int>("out_request_test");
        out_event_test = addOutPort<int>("out_event_test");

        auto sun_reader = addComponent<cadmium::lib::IEStream<int>>("sun_reader", "inputs/stem_input_sun_empty.txt");
        auto response_reader = addComponent<cadmium::lib::IEStream<int>>("response_reader", "inputs/stem_input_response_growth.txt");

        auto stem_model = addComponent<stem>("stem_model");

        addCoupling(sun_reader->out, stem_model->in_sun);
        addCoupling(response_reader->out, stem_model->in_response);

        addCoupling(stem_model->out_request, out_request_test);
        addCoupling(stem_model->out_event, out_event_test);
    }
};

// Experiment 2: Wilt Threshold
// Send 5 consecutive DRY (0) signals on in_response.
// Expect out_event = 0 (wilt) on the 5th signal.
struct top_test_stem_wilt : public Coupled {

    Port<int> out_request_test;
    Port<int> out_event_test;

    top_test_stem_wilt(const std::string& id) : Coupled(id) {
        out_request_test = addOutPort<int>("out_request_test");
        out_event_test = addOutPort<int>("out_event_test");

        auto sun_reader = addComponent<cadmium::lib::IEStream<int>>("sun_reader", "inputs/stem_input_sun_empty.txt");
        auto response_reader = addComponent<cadmium::lib::IEStream<int>>("response_reader", "inputs/stem_input_response_wilt.txt");

        auto stem_model = addComponent<stem>("stem_model");

        addCoupling(sun_reader->out, stem_model->in_sun);
        addCoupling(response_reader->out, stem_model->in_response);

        addCoupling(stem_model->out_request, out_request_test);
        addCoupling(stem_model->out_event, out_event_test);
    }
};

// Experiment 3: Polling Behavior
// Send sun=1 at t=0.5 to enable daytime.
// No response events — just verify that out_request fires every 2 seconds.
struct top_test_stem_polling : public Coupled {

    Port<int> out_request_test;
    Port<int> out_event_test;

    top_test_stem_polling(const std::string& id) : Coupled(id) {
        out_request_test = addOutPort<int>("out_request_test");
        out_event_test = addOutPort<int>("out_event_test");

        auto sun_reader = addComponent<cadmium::lib::IEStream<int>>("sun_reader", "inputs/stem_input_sun_on.txt");
        auto response_reader = addComponent<cadmium::lib::IEStream<int>>("response_reader", "inputs/stem_input_sun_empty.txt");

        auto stem_model = addComponent<stem>("stem_model");

        addCoupling(sun_reader->out, stem_model->in_sun);
        addCoupling(response_reader->out, stem_model->in_response);

        addCoupling(stem_model->out_request, out_request_test);
        addCoupling(stem_model->out_event, out_event_test);
    }
};

// Experiment 4: Recovery First
// Send 5 DRY (0) signals to reach stress=5 (wilt at t=5),
// then 5 WATER (1) signals to heal stress back to 0,
// then 10 more WATER (1) signals to accumulate nutrients and trigger growth.
// Expected: wilt at t=5, growth at t=20.
struct top_test_stem_recovery : public Coupled {

    Port<int> out_request_test;
    Port<int> out_event_test;

    top_test_stem_recovery(const std::string& id) : Coupled(id) {
        out_request_test = addOutPort<int>("out_request_test");
        out_event_test = addOutPort<int>("out_event_test");

        auto sun_reader = addComponent<cadmium::lib::IEStream<int>>("sun_reader", "inputs/stem_input_sun_empty.txt");
        auto response_reader = addComponent<cadmium::lib::IEStream<int>>("response_reader", "inputs/stem_input_response_recovery.txt");

        auto stem_model = addComponent<stem>("stem_model");

        addCoupling(sun_reader->out, stem_model->in_sun);
        addCoupling(response_reader->out, stem_model->in_response);

        addCoupling(stem_model->out_request, out_request_test);
        addCoupling(stem_model->out_event, out_event_test);
    }
};

int main() {
    // Experiment 1: Growth Threshold
    std::cout << "=== Experiment 1: Growth Threshold ===" << std::endl;
    {
        auto test_system = std::make_shared<top_test_stem_growth>("test_stem_growth_system");
        auto rootCoordinator = cadmium::RootCoordinator(test_system);
        rootCoordinator.setLogger<cadmium::STDOUTLogger>();
        rootCoordinator.start();
        rootCoordinator.simulate(15.0);
        rootCoordinator.stop();
    }

    std::cout << std::endl;

    // Experiment 2: Wilt Threshold
    std::cout << "=== Experiment 2: Wilt Threshold ===" << std::endl;
    {
        auto test_system = std::make_shared<top_test_stem_wilt>("test_stem_wilt_system");
        auto rootCoordinator = cadmium::RootCoordinator(test_system);
        rootCoordinator.setLogger<cadmium::STDOUTLogger>();
        rootCoordinator.start();
        rootCoordinator.simulate(10.0);
        rootCoordinator.stop();
    }

    std::cout << std::endl << "=== Experiment 3: Polling Behavior ===" << std::endl;
    {
        auto test_system = std::make_shared<top_test_stem_polling>("test_stem_polling_system");
        auto rootCoordinator = cadmium::RootCoordinator(test_system);
        rootCoordinator.setLogger<cadmium::STDOUTLogger>();
        rootCoordinator.start();
        rootCoordinator.simulate(12.0);
        rootCoordinator.stop();
    }

    std::cout << std::endl << "=== Experiment 4: Recovery First ===" << std::endl;
    {
        auto test_system = std::make_shared<top_test_stem_recovery>("test_stem_recovery_system");
        auto rootCoordinator = cadmium::RootCoordinator(test_system);
        rootCoordinator.setLogger<cadmium::STDOUTLogger>();
        rootCoordinator.start();
        rootCoordinator.simulate(25.0);
        rootCoordinator.stop();
    }

    std::cout << std::endl << "Stem model tests completed." << std::endl;
    return 0;
}
