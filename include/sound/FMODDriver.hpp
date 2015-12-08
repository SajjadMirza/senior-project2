#ifndef _FMODDriver_H_
#define _FMODDriver_H_

#include <common.hpp>

#include <fmod/fmod.hpp>
#include <fmod/fmod_errors.h>

namespace sound {
    class FMODDriver {
        private:
        public:
            FMODDriver();
            ~FMODDriver();
            FMOD::System *system;
            
            void testSound();
            void footStep(bool step);
            void EnergySound();
            void HanoiCheck();

            void StartSound();
            void StartCheck();

            void DoorSound();
            void DoorCheck();
    };
}

#endif
