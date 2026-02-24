#ifndef BASIL_PLANT_HPP
#define BASIL_PLANT_HPP

#include <cadmium/modeling/devs/coupled.hpp>
#include "soil.hpp"
#include "stem.hpp"

using namespace cadmium;

struct basil_plant : public Coupled {
    Port<int> in_rain;
    Port<int> in_sun;
    Port<int> out_event;

    basil_plant(const std::string& id) : Coupled(id) {
        in_rain = addInPort<int>("in_rain");
        in_sun = addInPort<int>("in_sun");
        out_event = addOutPort<int>("out_event");

        auto plant_soil = addComponent<soil>("plant_soil");
        auto plant_stem = addComponent<stem>("plant_stem");

        // EIC: Couple external inputs to the respective models
        addCoupling(this->in_rain, plant_soil->in_rain);
        addCoupling(this->in_sun, plant_stem->in_sun);

        // IC: Couple soil output to stem input
        addCoupling(plant_soil->out_soil_status, plant_stem->in_response);
        addCoupling(plant_stem->out_request, plant_soil->in_request);

        // EOC: Couple stem output to the plant's output
        addCoupling(plant_stem->out_event, this->out_event);
    }
};

#endif // BASIL_PLANT_HPP
