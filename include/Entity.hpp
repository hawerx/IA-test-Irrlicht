#pragma once
#include <algorithm>
#include <cstdint>

#include <irrlicht/irrlicht.h>
#include "Device.hpp"
#include "Util.hpp"

constexpr int   PLAYER_ID                     =     0;

constexpr float MOVEMENT_SPEED_PLAYER         =   10.F;
constexpr float ENTITY_MAX_SPEED              =   2.F;
constexpr float STEERING_MAX_FORCE            =   0.55F;
constexpr float NEIGHBOR_RADIUS               =   50.F;
constexpr float DESIRED_SEPARATION            =   3.F;

static int NUM_ENTIDADES;

using namespace irr;

struct CinematicMovement
{
    float               linear_speed    {0.0};
    float               angular_speed   {0.0};      // speed of angular movement
    float               orientation     {0.0};        //or alineation in rad
};

struct BoidData
{
    core::vector3df     alignment {};
    core::vector3df     cohesion  {};
    core::vector3df     separation{};

    float neighborRadius    {NEIGHBOR_RADIUS};
    float maxSpeed          {ENTITY_MAX_SPEED};
    float maxSteeringforce  {STEERING_MAX_FORCE};
};

struct Entity
{
    protected:
        scene::ISceneNode*  entity;

        core::vector3df     targetPosition      { };    
        core::vector3df     velocity            { };
        core::vector3df     acceleration        { };    //default 0
        core::vector3df     prev_acceleration   { };

        float angle             { util::random<float>(0 , 2*M_PI ) };
        bool                hasConstantSpeed    { };

        //MOVIMIENtTO VECTORIAL
        CinematicMovement   movement;
        //Boids
        BoidData            boid;

    public:

        Entity( scene::ISceneNode * node, core::vector3df const & pos = {0,0,0}, float maxSpeed = ENTITY_MAX_SPEED)    
            :   entity {node}
        {
            boid.maxSpeed = maxSpeed ;
            node->setID(NUM_ENTIDADES);
            setPosition(pos);
            NUM_ENTIDADES++;
            velocity = core::vector3df(cos(angle), 0 , sin(angle));
        }

        core::vector3df & getVelocity()
        {
            return velocity;
        }

        core::vector3df & getAcceleration()
        {
            return acceleration;
        }

        void setVelocity( core::vector3df const & newVelocity)
        {
            velocity = newVelocity;
            
            //entity->setRotation(core::vector3df(velocity.X, util::vector::getAngleDegree(velocity), velocity.Z));
        }

        core::vector3df const & getPosition() const
        {
            return entity->getPosition();
        }


        core::vector3df const & getRotation() const
        {
            return entity->getRotation();
        }

        void resetAcceleration() {acceleration *= 0; }

        void setRotation(core::vector3df const & newRotation)
        {
            entity->setRotation(newRotation);
        }

        void setPosition(core::vector3df const & newPosition)
        {
            entity->setPosition(newPosition);
        }

        // void setSpeed(float const & newSpeed)
        // {
        //     speed = newSpeed;
        // }

        // void setMaxAcceleration(float const & newMaxAcceleration)
        // {
        //     max_acceleration = newMaxAcceleration;
        // }

        void setHasConstantSpeed(bool const & newHasConstantSpeed)
        {
            hasConstantSpeed = newHasConstantSpeed;
        }

        int getID() const noexcept
        {
            return entity->getID();
        }

        scene::ISceneNode * getEntityNode() const noexcept
        {
            return entity;
        }

        CinematicMovement & getMovement()
        {
            return movement;
        }

        float module(core::vector3df const & v)
        {
            return sqrt((v.X * v.X) + (v.Y * v.Y) + (v.Z * v.Z));
        }

        void limitVelocity(core::vector3df  & v, float const & maxspeed)
        {
            float multiplier = ((maxspeed * 100)/module(v))/100;
            
            v.X *= multiplier;
            v.Y *= multiplier;
            v.Z *= multiplier;

            std::clamp(v.X , 0.F , maxspeed );
            std::clamp(v.Y , 0.F , maxspeed );
            std::clamp(v.Z , 0.F , maxspeed );

        }


    core::vector3df seek(core::vector3df const & target)
    {
            core::vector3df desired = target - getPosition();
            desired.normalize();
            desired *= boid.maxSpeed;

            core::vector3df steer   = desired - velocity;
            limitVelocity(steer , boid.maxSteeringforce);
            
            return steer;
    }

    float getDistanceXZ(core::vector3df v1, core::vector3df v2)
    {
        return sqrt(((v2.X-v1.X)*(v2.X-v1.X)) + ((v2.Y-v1.Y)*(v2.Y-v1.Y)) + ((v2.Z-v1.Z)*(v2.Z-v1.Z)));
    }
        //Aligment
        core::vector3df alignment(std::vector<Entity> boids) noexcept
        {
            core::vector3df suma = {0,0,0};
            int count = 0;

            for(int i = 0; i < boids.size(); i++)
            {
                //float dist = getPosition().getDistanceFrom(boids[i].getPosition()) ;
                
                float dist = getDistanceXZ(getPosition(), boids[i].getPosition());

                if(dist > 0 && dist < NEIGHBOR_RADIUS)
                {
                    suma += boids[i].getVelocity();
                    count++;
                }
            }

            if(count > 0)
            {
                suma /=(float)count;
                suma.normalize();
                suma *= boid.maxSpeed;

                core::vector3df steer = suma - velocity;
                limitVelocity(steer, boid.maxSteeringforce);

                return steer;
            }
            else{
                return core::vector3df{0,0,0};
            }
        }

        //Separation
        core::vector3df separation(std::vector<Entity> boids) noexcept
        {
            core::vector3df steer = {0,0,0};
            int count = 0;

            for(int i = 0; i < boids.size(); i++)
            {
                 //float dist = getPosition().getDistanceFrom(boids[i].getPosition()) ;
                
                float dist = getDistanceXZ(getPosition(), boids[i].getPosition());

                if(dist > 0 && dist < DESIRED_SEPARATION)
                {
                    //calcular vector apuntando hacia afuera del vecino
                    core::vector3df diff = getPosition() - boids[i].getPosition();
                    diff.normalize();
                    diff /= dist;
                    steer += diff;
                    count++;
                }
            }
            //average
            if(count > 0 )
                steer /= (float)count;

            //mientras el vector sea mayor a 0 

            if(module(steer) > 0 )
            {
                steer.normalize();
                steer *= boid.maxSpeed;
                steer -= velocity;
                limitVelocity(steer, boid.maxSteeringforce);
            }

            return steer;
        }

        //Cohesion
        core::vector3df cohesion(std::vector<Entity> boids) noexcept
        {
            core::vector3df suma;
            int count = 0;

            for(int i = 0; i < boids.size(); i++)
            {
                 //float dist = getPosition().getDistanceFrom(boids[i].getPosition()) ;
                
                float dist = getDistanceXZ(getPosition(), boids[i].getPosition());
                
                if(dist  > 0 && dist < NEIGHBOR_RADIUS)
                {
                    suma += boids[i].getPosition();
                    count++;
                }
            }

            if(count > 0 )
            {
                suma /= (float) count;
                return seek(suma);      //busca la posicion
            }
            else{
                return core::vector3df{0,0,0};
            }
        }

            void applyForce(core::vector3df force)
        {
            acceleration += force;
        }

        //Flocking behavior
        void flock (std::vector<Entity> boids)
        {
           boid.separation  = separation (boids);
           boid.alignment   = alignment (boids);
           boid.cohesion    = cohesion (boids);

           boid.separation *= 1;
           boid.alignment  *= 1;
           boid.cohesion   *= 1;

           applyForce(boid.separation);
           applyForce(boid.alignment);
           applyForce(boid.cohesion);
        }

        void updateEntityVelocity()
        {
            velocity += acceleration;
            limitVelocity(velocity, boid.maxSpeed);
        }
        
        void updateEntityMovement(float const & frameDeltaTime)
        {
            setPosition( getPosition() + (velocity * frameDeltaTime));
            resetAcceleration();
        }

};


// void updatePlayerVelocity(Device const & device, Entity & ent , f32 const & delta, float const & linearS = 0, float const & angularS = 0)
// {
//     auto& move      = ent.getMovement();
//     move.linear_speed   = move.angular_speed = 0;    

//     if(device.getInputHandler().IsKeyDown(irr::KEY_ESCAPE))
//             device.getIrrlichtDevice()->closeDevice();

//     if(device.getInputHandler().IsKeyDown(irr::KEY_KEY_W))
//         move.linear_speed   =   1;
//     else if(device.getInputHandler().IsKeyDown(irr::KEY_KEY_S))
//          move.linear_speed  =   -1;

//     if(device.getInputHandler().IsKeyDown(irr::KEY_KEY_A))
//         move.angular_speed  = - (2*M_PI/delta);
//     else if(device.getInputHandler().IsKeyDown(irr::KEY_KEY_D))
//         move.angular_speed  =   (2*M_PI/ delta) ;
//     // if(device.getInputHandler().IsKeyDown(irr::KEY_SPACE))
//     //     nodePosition.Y += (speed * frameDeltaTime);
    
//     // if(device.getInputHandler().IsKeyDown(irr::KEY_KEY_C))
//     //     nodePosition.Y -= (speed * frameDeltaTime);
// }


void updatePlayerMovement (Device const & device, Entity & ent, f32 const & frameDeltaTime, float const & speed)
{
    core::vector3df nodePosition = ent.getEntityNode()->getPosition();
   
    core::vector3df cameraFacing (sin((ent.getEntityNode()->getRotation().Y + 90.F ) * M_PI/180.0f ), 0, cos( ( ent.getEntityNode()->getRotation().Y + 90.0f ) * M_PI/180.0f ) );
    core::vector3df cameraFacingLateral (sin((ent.getEntityNode()->getRotation().Y ) * M_PI/180.0f ), 0, cos( ( ent.getEntityNode()->getRotation().Y  ) * M_PI/180.0f ) );
    
    cameraFacing.normalize();
    cameraFacingLateral.normalize();

    if(device.getInputHandler().IsKeyDown(irr::KEY_ESCAPE))
            device.getIrrlichtDevice()->closeDevice();

    if(device.getInputHandler().IsKeyDown(irr::KEY_KEY_W))
        nodePosition += (speed * frameDeltaTime) * cameraFacingLateral;
    else if(device.getInputHandler().IsKeyDown(irr::KEY_KEY_S))
        nodePosition -= (speed * frameDeltaTime) * cameraFacingLateral;

    if(device.getInputHandler().IsKeyDown(irr::KEY_KEY_A))
        nodePosition -= (speed * frameDeltaTime) * cameraFacing;
    else if(device.getInputHandler().IsKeyDown(irr::KEY_KEY_D))
        nodePosition += (speed * frameDeltaTime) * cameraFacing;
    
    if(device.getInputHandler().IsKeyDown(irr::KEY_SPACE))
        nodePosition.Y += (speed * frameDeltaTime);
    
    if(device.getInputHandler().IsKeyDown(irr::KEY_KEY_C))
        nodePosition.Y -= (speed * frameDeltaTime);
    
    if(device.getInputHandler().IsKeyDown(irr::KEY_KEY_H))
        nodePosition.Y -= (speed * frameDeltaTime);


    ent.setPosition(nodePosition);
};





void updateEntities(Device const & device, std::vector<Entity> entities, f32 const & frameDeltaTime)
{
    
    for(Entity ent : entities)
    {
        if(ent.getID() == 0)
        {
            //updatePlayerVelocity(device,ent,frameDeltaTime,MOVEMENT_SPEED_PLAYER);
            updatePlayerMovement(device, ent, frameDeltaTime, MOVEMENT_SPEED_PLAYER);
        }
        else
        {
            ent.flock(entities);
            ent.updateEntityVelocity();
            ent.updateEntityMovement(frameDeltaTime);
        }
    }
};


void createEntities(Device const & device , size_t const & numEntities, std::vector<Entity> & outVector)
{

    for(int i = 0; i < numEntities ; i++)
    {

        if(i == 0)
        {
            //Player ID 0 
           outVector.emplace_back( Entity(addCameraFPS(device)));
           outVector.at(0).setPosition(core::vector3df(-10,0,-10));   
        }
        else
        {
            auto ent = Entity(createSphere(device));
            outVector.emplace_back( ent );   
        }
    }

}