#include "InputHandler.hpp"

#include <cstdint>
#include <memory>
#include <stdexcept>
#include <iostream>

using namespace irr;

constexpr float MOVEMENT_SPEED_PLAYER   =   5.0;
constexpr float MOVEMENT_SPEED_SUBJECTS =   3.0; 


struct Device
{
    using DeviceDestructor = void (*)(irr::IrrlichtDevice*); 
    
    private:
        
        InputHandler                                            inputManager;
        std::unique_ptr < IrrlichtDevice , DeviceDestructor >   device;
        video::IVideoDriver*                                    driver;
        scene::ISceneManager*                                   smgr;
        static void destroy(IrrlichtDevice* dev) {  dev->drop(); }
    
    public:
        Device() :
            device  {   createDevice(
                        video::EDT_OPENGL,core::dimension2d<u32>(640,480),
                        16, false, false, false, &inputManager),
                        destroy 
                    },
            driver  { device ? device -> getVideoDriver()   : nullptr},
            smgr    { device ? device -> getSceneManager()  : nullptr}
        {
            if(!device)
                std::cerr << "Device not found\n";
        };

        ~Device() {
        };

        u32 const getTime() const noexcept {  return device->getTimer()->getTime();  }

        IrrlichtDevice*         getIrrlichtDevice() const   {   return device.get();  }

        video::IVideoDriver*    getVideoDriver()    const   {   return driver;  }

        scene::ISceneManager*   getSceneManager()   const   {   return smgr;    }

        InputHandler const &    getInputHandler()   const   {   return inputManager;    }

};

void game_loop(Device const & device, scene::ISceneNode* node )
{
    int lastFPS = -1;
    u32 then = device.getTime();

    while (device.getIrrlichtDevice()->run())
    {
        // Work out a frame delta time.
        const u32 now = device.getTime();
        const f32 frameDeltaTime = (f32)(now - then) / 1000.f; // Time in seconds
        then = now;
        //=======================

        core::vector3df nodePosition = node->getPosition();

        if(device.getInputHandler().IsKeyDown(irr::KEY_ESCAPE))
                device.getIrrlichtDevice()->closeDevice();

        if(device.getInputHandler().IsKeyDown(irr::KEY_KEY_W))
            nodePosition.Y += MOVEMENT_SPEED_PLAYER * frameDeltaTime;
        else if(device.getInputHandler().IsKeyDown(irr::KEY_KEY_S))
            nodePosition.Y -= MOVEMENT_SPEED_PLAYER * frameDeltaTime;

        if(device.getInputHandler().IsKeyDown(irr::KEY_KEY_A))
            nodePosition.X -= MOVEMENT_SPEED_PLAYER * frameDeltaTime;
        else if(device.getInputHandler().IsKeyDown(irr::KEY_KEY_D))
            nodePosition.X += MOVEMENT_SPEED_PLAYER * frameDeltaTime;

        node->setPosition(nodePosition);

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
}

scene::ISceneNode* createSphere(Device const & device)
{
    scene::ISceneNode * node = device.getSceneManager()->addSphereSceneNode();
    if (node)
    {
        node->setPosition(core::vector3df(0,0,30));
        node->setMaterialTexture(0, device.getVideoDriver()->getTexture("assets/wall.bmp"));
        node->setMaterialFlag(video::EMF_LIGHTING, false);
    }

    return node;
}

scene::ICameraSceneNode* addCameraFPS(Device const & device)
{
    return device.getSceneManager()->addCameraSceneNodeFPS();
}

int main ()
{
    Device device;

    addCameraFPS(device);

    game_loop(device, createSphere(device));

    return 0;

}