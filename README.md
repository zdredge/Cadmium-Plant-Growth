# Basil Plant Growth - DEVS Simulation

**Author:** Zachary Dredge - 101197514
**Course:** SYSC4906G

## Overview
This project models the growth of a basil plant as a discrete event system (DEVS), focusing on the link between environmental resources and biological growth. The system simulates how environmental factors impact soil moisture and ultimately trigger biological lifecycle events, such as plant growth or wilting. 

The model relies on an active polling where the plant stem dynamically requests water, processes nutrient absorption, and prioritizes stress recovery during periods of drought.

### Model Hierarchy
* **Top Model:** Greenhouse
* **Coupled Model:** Basil Plant
* **Atomic Models:** 
  * `Nature`
  * `Soil`
  * `Plant Stem`

## File Organization
This project strictly follows the standard Cadmium project directory structure:

* **`atomics/`**: Contains the C++ header files (`.hpp`) defining the atomic DEVS models.
    * `nature.hpp`: The environment event generator (sun and rain).
    * `soil.hpp`: The moisture buffer and handler.
    * `stem.hpp`: The biological growth model that tracks stress and nutrients.
* **`coupled/`**: Contains the C++ header files defining the coupled DEVS models.
    * `basil_plant.hpp`: The internal coupling of the `soil` and `stem` atomic models.
    * `greenhouse.hpp`: The top-level coupling of `nature` and `basil_plant`.
* **`top_model/`**: Contains the main C++ file (`main.cpp`) used to instantiate and run the top-level greenhouse model.
* **`test/`**: Contains the individual test benches (`test_soil.cpp`, `test_stem.cpp`, etc.) used to verify the logic of each component.
* **`input_data/`**: Contains the text files used to feed external inputs into the test cases.
* **`bin/`**: The target directory where the compiled executables are saved after running `make`.

## Prerequisites
To compile and run this simulation, you must have the following installed:
* A standard C++17 compiler (e.g., `g++`)
* The **Cadmium** DEVS simulation library
* `make` build automation tool

## Compilation
This project includes a standard `Makefile` to automate the build process.

To compile the entire project, navigate to the project directory in your terminal and run:
```bash 
make
```
## Testing
The terminal commands to run the test classes for each model, atomic and coupled, are listed below:
* **Nature**: make run_nature
* **Soil**: make run_soil
* **Plant Stem**: make run_stem
* **Basil Plant**: make run_basil_plant
* **Greenhouse**: make run_greenhouse

Additionally, as discussed in the report there are a few test cases that outputs have not been included for as they were more exporitory in nature.
The following is a set of instructions to reproduce each test.
* **Varying Time Unit Simulations**: simply adjust the parameter of the call rootCoordinator.simulate(500.0) in `main.cpp` to your desired value
* **Varying Threshold Value**:
  * To change the stress threshold: adjust stressLevel in line 93 in `stem.hpp` to your desired value. In brief, lower values drive up the number of stress events, and higher values lead to more healing cycles.
  * To change the growth threshold: adjust nutrientCoun on line 75 and 91 of `stem.hpp`. Increasing this value will limit the number of growth events, whereas decreasing it will slightly increase them.


