#ifndef _FMODDriver_H
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
    };
}

#endif
