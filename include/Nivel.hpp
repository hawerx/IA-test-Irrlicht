#pragma once
#include "Entity.hpp"

struct NivelFlocking
{
    private:
        //Vector de entidades
        std::vector <Entity> boids;

    public:
        NivelFlocking(Device const & device, size_t const & numEntNivel)
        { 
            createEntities(device, numEntNivel, boids);
        }

        std::vector<Entity> & getEntities ()
        {
            return boids;
        }      
};

struct NivelAstarPursuit
{   private:
        //Vector de entidades
        std::vector <Entity> entities;
    public:
         NivelAstarPursuit(Device const & device, size_t const & numEntNivel)
        { 
            createEntities(device, numEntNivel, entities);
        }      
};


