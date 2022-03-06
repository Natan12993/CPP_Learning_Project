#include "tower_sim.hpp"

int main(int argc, char** argv)
{
    ContextInitialiser contextInitializer = ContextInitialiser {argc, argv};
    TowerSimulation simulation { contextInitializer };
    simulation.launch();

    return 0;
}