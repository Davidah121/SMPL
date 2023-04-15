#pragma once

#ifdef _WIN32

    #include <Windows.h>
    #include <vector>

    namespace glib
    {
        class TouchSimulator
        {
        public:
            static const int INPUTS_ALLOWED = 10;
            static const int L_THUMB = 0;
            static const int L_INDEX = 1;
            static const int L_MIDDLE = 2;
            static const int L_RING = 3;
            static const int L_PINKY = 4;
            static const int R_THUMB = 5;
            static const int R_INDEX = 6;
            static const int R_MIDDLE = 7;
            static const int R_RING = 8;
            static const int R_PINKY = 9;
            
            /**
             * @brief An init function that must be called before 
             *          any touch input injections can be performed.
             * 
             */
            static void init();

            /**
             * @brief Sets the inputID to be down at the specified x and y.
             *      If the input at that ID was already down, it will instead update.
             *          Update is used for updating the position for things like moving.
             *      Note that a touch point can be cancelled if not updated enough.
             *          They may not last for more than 1 second.
             * 
             * @param inputID 
             * @param x 
             * @param y 
             */
            static void setDown(int inputID, int x, int y);

            /**
             * @brief Sets the inputID to be released. It will release at the last inject point for that touch id.
             * 
             * @param inputID 
             */
            static void setUp(int inputID);

            /**
             * @brief Injects all touch inputs. This injects all 10 possible points. This is so that
             *      multiple touch points can be used. These inject calls may need be spaced out so that
             *      OS does not produce an error.
             * 
             */
            static bool inject();
        private:
            static std::vector<bool> updatePoints;
            static std::vector<POINTER_TOUCH_INFO> inputs;
        };
    }

#endif