#include <iostream>
#include <string>
#include <cadmium/modeling/devs/coupled.hpp>
#include <cadmium/simulation/root_coordinator.hpp>
#include <cadmium/simulation/logger/stdout.hpp>
#include "nature.hpp" // Make sure this is in the same directory or properly linked

using namespace cadmium;

// 1. Create a top-level coupled model specifically for testing
struct top_test_nature : public Coupled {
    
    // Define output ports for the test harness so Cadmium logs them clearly
    Port<int> out_sun_test;
    Port<int> out_rain_test;

    top_test_nature(const std::string& id) : Coupled(id) {
        out_sun_test = addOutPort<int>("out_sun_test");
        out_rain_test = addOutPort<int>("out_rain_test");

        // 2. Instantiate your Nature atomic model
        auto nature_model = addComponent<nature>("nature_model");

        // 3. Route the atomic model's outputs to the test harness outputs (EOC)
        addCoupling(nature_model->sun_out, out_sun_test);
        addCoupling(nature_model->rain_out, out_rain_test);
    }
};

int main() {
    // 4. Instantiate the test system
    auto test_system = std::make_shared<top_test_nature>("test_nature_system");

    // 5. Set the simulation time.
    // Your report requires 48s for the Solar test and 100s for the Rain test.
    // We will set the clock to 100 seconds to satisfy both.
    auto rootCoordinator = cadmium::RootCoordinator(test_system);
    rootCoordinator.setLogger<cadmium::STDOUTLogger>();

    // 6. Run the simulation
    rootCoordinator.start();
    rootCoordinator.simulate(100.0);
    rootCoordinator.stop();

    std::cout << "Nature model test completed. Please check the Cadmium log files." << std::endl;
    return 0;
}