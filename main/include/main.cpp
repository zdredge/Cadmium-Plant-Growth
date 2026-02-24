#include <iostream>
#include <string>
#include <cadmium/simulation/root_coordinator.hpp>
#include <cadmium/simulation/logger/stdout.hpp>
#include "greenhouse.hpp"

int main() {
    auto model = std::make_shared<greenhouse>("greenhouse_simulation");

    auto rootCoordinator = cadmium::RootCoordinator(model);
    rootCoordinator.setLogger<cadmium::STDOUTLogger>();

    rootCoordinator.start();
    rootCoordinator.simulate(500.0);
    rootCoordinator.stop();

    std::cout << "Greenhouse simulation completed." << std::endl;
    return 0;
}
