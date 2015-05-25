#include <sound/FMODDriver.hpp>



namespace sound {

    const char *sound_test = "resources/sounds/Ode_to_Joy.ogg";

    FMODDriver::FMODDriver() {
        FMOD_RESULT result;

        result = FMOD::System_Create(&system); // Create the main system object.
        if (result != FMOD_OK) {
            printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
            exit(-1);
        }

        result = system->init(512, FMOD_INIT_NORMAL, 0); // Initialize FMOD.
        if (result != FMOD_OK) {
            printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
            exit(-1);
        }
    }
    
    FMODDriver::~FMODDriver() {
        system->release();
    }
    
    void FMODDriver::testSound() {
        FMOD::Sound *audio;
        system->createSound(sound_test, FMOD_LOOP_NORMAL, 0, &audio);

        FMOD::Channel *channel;
        system->playSound(audio, NULL, false, &channel);
    }
}
