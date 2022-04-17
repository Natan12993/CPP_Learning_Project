#include "tower_sim.hpp"

int main(int argc, char** argv)
{
    test_generic_points();
    ContextInitialiser contextInitializer = ContextInitialiser {argc, argv};
    TowerSimulation simulation { contextInitializer };
    simulation.launch();
    return 0;
}