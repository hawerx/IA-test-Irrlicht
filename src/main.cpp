#include "../include/Nivel.hpp"


int main ()
{ 
    srand(time(0));

    //Device creation
    Device device (1280, 720);

    //================
    // Seleccion de Niveles
    NivelFlocking nivelFlocking(device , 100);
    //NivelAstarPursuit nivelApursuit (device , 20);

    //================
    //Main loop
    game_loop(device, nivelFlocking.getEntities());

    return 0;

}