#pragma once

#include "Entity.hpp"
#include "InputHandler.hpp"
#include <cstdint>
#include <memory>
#include <iostream>
#include <vector>

using namespace irr;

using DeviceDestructor  = void (*)(irr::IrrlichtDevice*); 
using DeviceType        = std::unique_ptr < IrrlichtDevice , DeviceDestructor >;    

struct Entity;
struct Device;
void updateEntities(Device const & device, std::vector<Entity> entities, f32 const & frameDeltaTime);

struct Device
{
    private:
    
        InputHandler            inputManager;
        DeviceType              device;
        video::IVideoDriver*    driver;
        scene::ISceneManager*   smgr;

        static void destroy(IrrlichtDevice* dev) {  dev->drop(); }
    
    public:
        Device(uint32_t w = 640 , uint32_t h = 480) :
            device  {   createDevice(
                        video::EDT_OPENGL,core::dimension2d<u32>(w,h),
                        16, false, false, false, &inputManager),
                        destroy 
                    },
            driver  { device ? device -> getVideoDriver()   : nullptr},
            smgr    { device ? device -> getSceneManager()  : nullptr}
        {
            if(!device)
                throw std::runtime_error("ERROR: No se ha podido crear el device.");
        
             device->getCursorControl()->setVisible(false);
        };

        ~Device() {
        };

        u32 getTime() const noexcept {  return device->getTimer()->getTime();  }

        IrrlichtDevice*         getIrrlichtDevice() const   {   return device.get();  }

        video::IVideoDriver*    getVideoDriver()    const   {   return driver;  }

        scene::ISceneManager*   getSceneManager()   const   {   return smgr;    }

        InputHandler const &    getInputHandler()   const   {   return inputManager;    }

};

void game_loop(Device const & device , std::vector<Entity> entities)
{
    int lastFPS = -1;
    u32 then = device.getTime();

    while (device.getIrrlichtDevice()->run())
    {
        if(device.getIrrlichtDevice()->isWindowActive())
        {
                // Work out a frame delta time.
            const u32 now = device.getTime();
            const f32 frameDeltaTime = (f32)(now - then) / 1000.f; // Time in seconds
            then = now;
            //=======================

            updateEntities(device, entities , frameDeltaTime);
           
            //========================

            device.getVideoDriver()->beginScene(true, true, video::SColor(255,113,113,133));
            device.getSceneManager()->drawAll(); // draw the 3d scene
            device.getVideoDriver()->endScene();

            //=======================
            int fps = device.getVideoDriver()->getFPS();
            if (lastFPS != fps)
            {
                core::stringw tmp(L"Movement Example - Irrlicht Engine [");
                tmp += device.getVideoDriver()->getName();
                tmp += L"] fps: ";
                tmp += fps;

                device.getIrrlichtDevice()->setWindowCaption(tmp.c_str());
                lastFPS = fps;
            }
        }
        else
            device.getIrrlichtDevice()->yield();
    }
};


scene::ISceneNode* createSphere(Device const & device)
{
    scene::ISceneNode * node = device.getSceneManager()->addSphereSceneNode();
    if (node)
    {
        node->setPosition(core::vector3df(0,0,0));
        node->setMaterialTexture(0, device.getVideoDriver()->getTexture("assets/wall.bmp"));
        node->setMaterialFlag(video::EMF_LIGHTING, false);
        node->setScale(core::vector3df(0.1,0.1,0.1));
    }

    return node;
};

scene::ICameraSceneNode* addCameraFPS(Device const & device)
{
    return device.getSceneManager()->addCameraSceneNodeFPS();
};

