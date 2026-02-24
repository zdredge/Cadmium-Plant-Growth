#ifndef SOIL_HPP
#define SOIL_HPP

#include <cadmium/modeling/devs/atomic.hpp>
#include <iostream>

using namespace cadmium;

struct soilState {
    int moistureLevel; 
    enum class State { WAITING, ABSORBING, RESPONDING } currentState;

    explicit soilState(): moistureLevel(0), currentState(State::WAITING) {}
};

std::ostream& operator<<(std::ostream& os, const soilState& s) {
    os << "moistureLevel: " << s.moistureLevel
       << " state: " << static_cast<int>(s.currentState);
    return os;
}

class soil : public Atomic<soilState> {
    public:
    Port<int> in_rain;
    Port<int> in_request;

    Port<int> out_soil_status;

    soil(const std::string& id): Atomic<soilState>(id, soilState()) {
        in_rain = addInPort<int>("in_rain");
        in_request = addInPort<int>("in_request");
        out_soil_status = addOutPort<int>("out_soil_status");
    }

    void internalTransition(soilState& state) const override {
        if (state.currentState == soilState::State::RESPONDING) {
            state.moistureLevel = std::max(0, state.moistureLevel - 1); // Decrease moisture level after responding
            
        }

        // After absorbing rain or responding to request, go back to waiting state
        state.currentState = soilState::State::WAITING;
    }

    void externalTransition(soilState& state, double e) const override {
        /*
        There is a race conditon that has been discovered in this method that leads to errors. If both in_rain and in_request receive messages at the same time, 
        the soil will absorb the rain but ignore the request from the Stem model. To fix this, we need to increment the moisture level based on the rain first, 
        and then check for the request. This way, the soil will always respond to the request regardless of whether it receives rain or not.
        */

        // Check for incoming messages on both ports
        bool receivedRequest = !in_request->empty();
        bool receivedRain = !in_rain->empty();

        if (receivedRain) { 
            state.moistureLevel += in_rain->getBag().back(); // Increase moisture level based on rain
        } 
        
        // Recheck the input ports after processing rain to ensure we respond to requests even if rain is received at the same time
        if (receivedRequest) {
            state.currentState = soilState::State::RESPONDING;
        } else if (receivedRain) {
            state.currentState = soilState::State::ABSORBING;
        }
    }

    void output(const soilState& state) const override {
        if (state.currentState == soilState::State::RESPONDING) {
            if (state.moistureLevel > 0) {
                out_soil_status->addMessage(1); // Indicate soil is moist
            } else {
                out_soil_status->addMessage(0); // Indicate soil is dry
            }
        } 
    }

    [[nodiscard]] double timeAdvance(const soilState& state) const override {
        if (state.currentState == soilState::State::WAITING) {
            return std::numeric_limits<double>::infinity(); // Wait indefinitely for external events
        } else {
            return 0.0; // Immediate internal transition after absorbing rain or responding to request
        }
    }
};

#endif // SOIL_HPP