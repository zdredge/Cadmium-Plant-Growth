#include <iostream>
#include <string>
#include <cadmium/modeling/devs/coupled.hpp>
#include <cadmium/simulation/root_coordinator.hpp>
#include <cadmium/simulation/logger/stdout.hpp>
#include <cadmium/lib/iestream.hpp>
#include "soil.hpp" 

using namespace cadmium;

// Define the test frame
struct top_test_soil : public Coupled {
    
    // Create an output port on the test harness to clearly log the results
    Port<int> out_status_test;

    top_test_soil(const std::string& id) : Coupled(id) {
        out_status_test = addOutPort<int>("out_status_test");

        // 1. Instantiate the Input Readers pointing to your text files
        auto rain_reader = addComponent<cadmium::lib::IEStream<int>>("rain_reader", "inputs/soil_input_rain.txt");
        auto request_reader = addComponent<cadmium::lib::IEStream<int>>("request_reader", "inputs/soil_input_request.txt");
        
        // 2. Instantiate the Soil atomic model
        auto soil_model = addComponent<soil>("soil_model");

        // 3. EIC: Couple the file readers to the Soil's input ports
        addCoupling(rain_reader->out, soil_model->in_rain);
        addCoupling(request_reader->out, soil_model->in_request);
        
        // 4. EOC: Couple the Soil's output to the test harness output
        addCoupling(soil_model->out_soil_status, out_status_test);
    }
};

struct top_test_soil_concurrent : public Coupled {
    Port<int> out_status_test;

    top_test_soil_concurrent(const std::string& id) : Coupled(id) {
        out_status_test = addOutPort<int>("out_status_test");

        // Instantiate the readers using the new concurrent request file
        auto rain_reader = addComponent<cadmium::lib::IEStream<int>>("rain_reader_concurrent", "inputs/soil_input_rain.txt"); 
        auto request_reader = addComponent<cadmium::lib::IEStream<int>>("request_reader_concurrent", "inputs/soil_input_request_concurrent.txt"); 
        
        // Instantiate the Soil atomic model
        auto soil_model = addComponent<soil>("soil_model_concurrent");

        // EIC: Couple the file readers to the Soil's input ports
        addCoupling(rain_reader->out, soil_model->in_rain);
        addCoupling(request_reader->out, soil_model->in_request);
        
        // EOC: Couple the Soil's output to the test harness output
        addCoupling(soil_model->out_soil_status, out_status_test);
    }
};

int main() {
    // Experiment 1: Standard Lifecycle
    std::cout << "=== Experiment 1: Standard Lifecycle ===" << std::endl;
    {
        auto test_system = std::make_shared<top_test_soil>("test_soil_system");
        auto rootCoordinator = cadmium::RootCoordinator(test_system);
        rootCoordinator.setLogger<cadmium::STDOUTLogger>();
        rootCoordinator.start();
        rootCoordinator.simulate(10.0);
        rootCoordinator.stop();
    }

    std::cout << std::endl;

    // Experiment 2: Concurrent Race Condition
    std::cout << "=== Experiment 2: Concurrent Race Condition ===" << std::endl;
    {
        auto test_system = std::make_shared<top_test_soil_concurrent>("test_soil_system_concurrent");
        auto rootCoordinator = cadmium::RootCoordinator(test_system);
        rootCoordinator.setLogger<cadmium::STDOUTLogger>();
        rootCoordinator.start();
        rootCoordinator.simulate(10.0);
        rootCoordinator.stop();
    }

    std::cout << std::endl << "Soil model tests completed. Please check the Cadmium log files." << std::endl;
    return 0;
}