#ifndef _FMODDriver_H
#define _FMODDriver_H_

#include <common.hpp>

#include <fmod.hpp>
#include <fmod_errors.h>

namespace sound {
    class FMODDriver {
        private:
        public:
            FMODDriver();
            ~FMODDriver();
            FMOD::System *system;
            
            void testSound();
    };
}

#endif
