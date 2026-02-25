#ifndef GREENHOUSE_HPP
#define GREENHOUSE_HPP

#include <cadmium/modeling/devs/coupled.hpp>
#include "nature.hpp"
#include "basil_plant.hpp"

using namespace cadmium;

struct greenhouse : public Coupled {
    Port<int> out_greenshouse_event;

    greenhouse(const std::string& id) : Coupled(id) {
        out_greenshouse_event = addOutPort<int>("out_greenhouse_event");

        auto nature_model = addComponent<nature>("nature_model");
        auto plant_model = addComponent<basil_plant>("plant_model");

        // EIC: Couple nature outputs to plant inputs
        addCoupling(nature_model->rain_out, plant_model->in_rain);
        addCoupling(nature_model->sun_out, plant_model->in_sun);

        // EOC: Couple plant output to greenhouse output
        addCoupling(plant_model->out_event, out_greenshouse_event);
    }
};

#endif // GREENHOUSE_HPP