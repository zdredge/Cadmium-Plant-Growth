#include <iostream>
#include <string>
#include <cadmium/modeling/devs/coupled.hpp>
#include <cadmium/simulation/root_coordinator.hpp>
#include <cadmium/simulation/logger/stdout.hpp>
#include <cadmium/lib/iestream.hpp>
#include "basil_plant.hpp"

using namespace cadmium;

// Experiment: Internal Coupling Verification
// Pre-load soil with moisture=2 via rain at t=1,2.
// Send in_sun=1 at t=3 to trigger stem request -> soil response chain.
// Expect: soil responds WATER (1), moisture decrements to 1, stem nutrientCount=1.
struct top_test_basil_plant : public Coupled {

    Port<int> out_event_test;

    top_test_basil_plant(const std::string& id) : Coupled(id) {
        out_event_test = addOutPort<int>("out_event_test");

        // Input readers
        auto rain_reader = addComponent<cadmium::lib::IEStream<int>>("rain_reader", "inputs/basil_input_rain.txt");
        auto sun_reader = addComponent<cadmium::lib::IEStream<int>>("sun_reader", "inputs/basil_input_sun.txt");

        // Basil plant coupled model under test
        auto plant = addComponent<basil_plant>("plant_model");

        // EIC: Wire readers to basil plant inputs
        addCoupling(rain_reader->out, plant->in_rain);
        addCoupling(sun_reader->out, plant->in_sun);

        // EOC: Wire basil plant output to test harness
        addCoupling(plant->out_event, out_event_test);
    }
};

int main() {
    auto test_system = std::make_shared<top_test_basil_plant>("test_basil_plant_system");

    auto rootCoordinator = cadmium::RootCoordinator(test_system);
    rootCoordinator.setLogger<cadmium::STDOUTLogger>();

    rootCoordinator.start();
    rootCoordinator.simulate(10.0);
    rootCoordinator.stop();

    std::cout << "Basil plant internal coupling test completed. Please check the Cadmium log files." << std::endl;
    return 0;
}
