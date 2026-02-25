#include <iostream>
#include <string>
#include <cadmium/modeling/devs/coupled.hpp>
#include <cadmium/simulation/root_coordinator.hpp>
#include <cadmium/simulation/logger/stdout.hpp>
#include "nature.hpp" // Make sure this is in the same directory or properly linked

using namespace cadmium;

// Create a top-level coupled model specifically for testing
struct top_test_nature : public Coupled {
    
    // Define output ports for the test harness so Cadmium logs them clearly
    Port<int> out_sun_test;
    Port<int> out_rain_test;

    top_test_nature(const std::string& id) : Coupled(id) {
        out_sun_test = addOutPort<int>("out_sun_test");
        out_rain_test = addOutPort<int>("out_rain_test");

        // Instantiate your Nature atomic model
        auto nature_model = addComponent<nature>("nature_model");

        // EOC: Route the atomic model's outputs to the test harness outputs
        addCoupling(nature_model->sun_out, out_sun_test);
        addCoupling(nature_model->rain_out, out_rain_test);
    }
};

int main() {
    // Instantiate the test system
    auto test_system = std::make_shared<top_test_nature>("test_nature_system");

    auto rootCoordinator = cadmium::RootCoordinator(test_system);
    rootCoordinator.setLogger<cadmium::STDOUTLogger>();


    rootCoordinator.start();
    rootCoordinator.simulate(100.0);
    rootCoordinator.stop();

    std::cout << "Nature model test completed. Please check the Cadmium log files." << std::endl;
    return 0;
}