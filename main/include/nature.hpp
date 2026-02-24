#ifndef NATURE_HPP
#define NATURE_HPP


#include <cadmium/modeling/devs/atomic.hpp>
#include <iostream>
#include <random>

using namespace cadmium;

struct natureState {
    int timeLeftToRain;
    int timeLeftToSun;
    int sunStatus; // 0 for no sun, 1 for sun
    enum class State { WAITING, SUNNY, RAINING } currentState;
    mutable std::mt19937 rng;
    std::uniform_int_distribution<int> rainDist;
    

    explicit natureState(): timeLeftToRain(6), timeLeftToSun(12), sunStatus(0), currentState(State::WAITING),
        rng(std::random_device{}()), rainDist(24, 48) {}
};

std::ostream& operator<<(std::ostream& os, const natureState& s) {
    os << "timeLeftToRain: " << s.timeLeftToRain
       << " timeLeftToSun: " << s.timeLeftToSun
       << " sunStatus: " << s.sunStatus
       << " state: " << static_cast<int>(s.currentState);
    return os;
}

class nature : public Atomic<natureState> {
    public:
    Port<int> rain_out;
    Port<int> sun_out;

    nature(const std::string& id): Atomic<natureState>(id, natureState()) {
        rain_out = addOutPort<int>("rain_out");
        sun_out = addOutPort<int>("sun_out");
    }

    void internalTransition(natureState& state) const override {
        if (state.currentState == natureState::State::WAITING) {
            if (state.timeLeftToSun <= state.timeLeftToRain) { // timeLeftToSun expires first, transition to SUNNY
                state.currentState = natureState::State::SUNNY;
                state.sunStatus = (state.sunStatus == 0) ? 1 : 0;
            } else { // timeLeftToRain expires first, transition to RAINING
                state.currentState = natureState::State::RAINING;
            }
        } else {
            if (state.currentState == natureState::State::SUNNY) {
                state.timeLeftToRain -= state.timeLeftToSun; // Decrease rain timer by the time spent in sunny state. This keeps the timers in sync.
                state.timeLeftToSun = 12; // Reset sun timer
            } else { // RAINING
                state.timeLeftToSun -= state.timeLeftToRain; 
                state.timeLeftToRain = state.rainDist(state.rng); // Randomize next rain time between 5 and 10 seconds
            }
            state.currentState = natureState::State::WAITING; // After transitioning, go back to waiting
        }
    }

    void externalTransition(natureState& state, double e) const override {
        // No external events for nature
    }

    void output(const natureState& state) const override {
        if (state.currentState == natureState::State::RAINING) {
            rain_out->addMessage(1); // Indicate it's raining
        }
        if (state.currentState == natureState::State::SUNNY) {
            sun_out->addMessage(state.sunStatus); // Indicate it's sunny
        }
    }

    [[nodiscard]] double timeAdvance(const natureState& state) const override {
        if (state.currentState == natureState::State::WAITING) {
             return std::min(state.timeLeftToRain, state.timeLeftToSun);
        } else {
            return 0.0; // Immediate transition when in SUNNY or RAINING state
        }
    }
};

#endif // NATURE_HPP