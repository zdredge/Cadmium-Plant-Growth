#ifndef STEM_HPP
#define STEM_HPP

#include <cadmium/modeling/devs/atomic.hpp>
#include <iostream>

using namespace cadmium;

 struct stemState {
    int stressLevel;
    int nutrientCount;
    bool isDaytime;
    enum class State { WAITING, REQUESTING, PROCESS_SOIL } currentState;
    enum class PendingReset { NONE, NUTRIENT } pendingReset;

    explicit stemState(): stressLevel(0), nutrientCount(0), isDaytime(false), currentState(State::WAITING), pendingReset(PendingReset::NONE) {}
};

std::ostream& operator<<(std::ostream& os, const stemState& s) {
    os << "stressLevel: " << s.stressLevel
       << " nutrientCount: " << s.nutrientCount
       << " isDaytime: " << s.isDaytime
       << " state: " << static_cast<int>(s.currentState);
    return os;
}

class stem : public Atomic<stemState> {
    public:
    Port<int> in_sun;
    Port<int> in_response;

    Port<int> out_request;
    Port<int> out_event;

    stem(const std::string& id): Atomic<stemState>(id, stemState()) {
        in_sun = addInPort<int>("in_sun");
        in_response = addInPort<int>("in_response");
        out_request = addOutPort<int>("out_request");
        out_event = addOutPort<int>("out_event");
    }

    void internalTransition(stemState& state) const override {
        if (state.currentState == stemState::State::WAITING) {
            // Polling: if daytime, transition to REQUESTING; otherwise stay WAITING
            if (state.isDaytime) {
                state.currentState = stemState::State::REQUESTING;
            }
            // If not daytime, remain in WAITING (idle poll)
        } else if (state.currentState == stemState::State::REQUESTING) {
            // After sending request, go back to waiting for next poll
            state.currentState = stemState::State::WAITING;
        } else if (state.currentState == stemState::State::PROCESS_SOIL) {
            // Apply pending resets from output phase
            if (state.pendingReset == stemState::PendingReset::NUTRIENT) {
                state.nutrientCount = 0;
            }
            state.pendingReset = stemState::PendingReset::NONE;
            state.currentState = stemState::State::WAITING;
        }
    }

    void externalTransition(stemState& state, double e) const override {
        if (!in_sun->empty()) {
            // Sun only updates the daytime flag — no state transition
            state.isDaytime = (in_sun->getBag().back() == 1);
        }
        if (!in_response->empty()) {
            int response = in_response->getBag().back();
            if (response == 1) {
                // Recovery First: water heals stress before contributing to growth
                if (state.stressLevel > 0) {
                    state.stressLevel -= 1; // Use water to heal stress
                } else {
                    state.nutrientCount += 1; // Increase nutrient count if soil is moist
                    if (state.nutrientCount == 5) {
                        state.pendingReset = stemState::PendingReset::NUTRIENT;
                    }
                }
            } else {
                state.stressLevel += 1; // Increase stress level if soil is dry
            }
            state.currentState = stemState::State::PROCESS_SOIL;
        }
    }
    

    void output(const stemState& state) const override {
        if (state.currentState == stemState::State::REQUESTING) {
            out_request->addMessage(1); // Request soil status
        } else if (state.currentState == stemState::State::PROCESS_SOIL) {
            if (state.nutrientCount == 5) {
                out_event->addMessage(1); // Indicate growth event
            } else if (state.stressLevel >= 10) {
                out_event->addMessage(0); // Indicate wilt event
            }
        }
    }

    [[nodiscard]] double timeAdvance(const stemState& state) const override {
        if (state.currentState == stemState::State::WAITING) {
            return 2.0; // Poll every 2 seconds
        } else {
            return 0.0; // Process immediately when in REQUESTING or PROCESS_SOIL state
        }
    }
};

#endif // STEM_HPP