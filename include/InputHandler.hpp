#pragma once
#pragma GCC diagnostic ignored "-Wenum-compare"
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wswitch"

#include <irrlicht/irrlicht.h>

class InputHandler : public irr::IEventReceiver
{
    private:

        //  We use this array to store the current state of each key
        bool KeyIsDown[irr::KEY_KEY_CODES_COUNT] { 0 };
        //  Check if Mouse Left Click is Pressed
        bool mouseLeftisDown    { false };
        //  Check if Mouse Right Click is Pressed
        bool mouseRightIsDown   { false };
        //  Check if Mouse Middle Click is Pressed
        bool mouseMiddleClick   { false };
        //  Stores mouse Position in the screen
        irr::core::vector2d<irr::s32> mousePosition { };
        //  Stores mouse wheel position
        irr::f32 mouseWheelPosition { };


    public:
        
        InputHandler()  { }
        
        ~InputHandler() { }

        virtual bool OnEvent(irr::SEvent const & event)
        {
            //Switch to check keyboard or mouse events
            switch(event.EventType)
            {
                // Check mouse Events
                case irr::EET_MOUSE_INPUT_EVENT :
                
                    if(event.MouseInput.Event == irr::EMIE_LMOUSE_PRESSED_DOWN)
                        mouseLeftisDown = true;
                    if(event.MouseInput.Event == irr::EMIE_RMOUSE_PRESSED_DOWN)
                        mouseRightIsDown = true;
                    if(event.MouseInput.Event == irr::EMIE_MMOUSE_PRESSED_DOWN)
                        mouseMiddleClick = true;
            
                    if(event.MouseInput.Event == irr::EMIE_LMOUSE_LEFT_UP)
                        mouseLeftisDown = false;
                    if(event.MouseInput.Event == irr::EMIE_RMOUSE_LEFT_UP)
                        mouseRightIsDown = false;
                    if(event.MouseInput.Event == irr::EMIE_MMOUSE_LEFT_UP)
                        mouseMiddleClick = false;

                    if(event.MouseInput.Event == irr::EMIE_MOUSE_MOVED)
                    {
                        mousePosition.X = event.MouseInput.X;
                        mousePosition.Y = event.MouseInput.Y;
                    }

                    if(event.EventType == irr::EMIE_MOUSE_WHEEL)
                    {
                        mouseWheelPosition += event.MouseInput.Wheel;
                    }

                break;
                

                // Check keyboard Events
                case irr::EET_KEY_INPUT_EVENT : 
                    KeyIsDown[event.KeyInput.Key] = event.KeyInput.PressedDown;
                break;
                
            }
            
            
            return false;
        }

        // This is used to check whether a key is being held down
        virtual bool IsKeyDown(irr::EKEY_CODE keyCode) const
        {
            return KeyIsDown[keyCode];
        }

        bool const & mouseLeftClickIsPressed()  const noexcept
        {
            return mouseLeftisDown;
        }

        bool const & mouseRightClickIsPressed()  const noexcept
        {
            return mouseRightIsDown;
        }

        bool const & mouseMiddleClickIsPressed()  const noexcept
        {
            return mouseMiddleClick;
        }

        irr::f32 const & getMouseWheelPosition()  const noexcept
        {
            return mouseWheelPosition;
        }
        
        irr::core::vector2d<irr::s32> const & getMousePosition () const noexcept
        {
            return mousePosition;
        };

};