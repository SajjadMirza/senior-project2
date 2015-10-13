#include <sound/FMODDriver.hpp>

#include <log.hpp>

namespace sound {

    const char *sound_test = "resources/sounds/Ode_to_Joy.ogg";
    const char *sound_foot = "resources/sounds/foot_step_wood.ogg";

    static FMOD::Channel *foot_channel;

    FMODDriver::FMODDriver() {
        FMOD_RESULT result;

        result = FMOD::System_Create(&system); // Create the main system object.
        if (result != FMOD_OK) {
            ERROR("FMOD error! " << result << " " << FMOD_ErrorString(result));
            exit(-1);
        }

        result = system->init(512, FMOD_INIT_NORMAL, 0); // Initialize FMOD.
        if (result != FMOD_OK) {
            ERROR("FMOD error! " << result << " " << FMOD_ErrorString(result));
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

    void FMODDriver::footStep(bool step) {
        bool playing;
        foot_channel->isPlaying(&playing);

        if (playing) {
            // std::cout << "HAHAHAHHA" << std::endl;
            if (step == false) {
                float sound;
                foot_channel->getVolume(&sound);

                if (sound > 0) {
                    foot_channel->setVolume(sound- 0.015);
                }
                else {
                    foot_channel->stop();
                }
            }
        }
        else {
            if (step == true) {
                FMOD::Sound *audio;
                system->createSound(sound_foot, FMOD_LOOP_OFF, 0, &audio);

                system->playSound(audio, NULL, false, &foot_channel); 
                foot_channel->setVolume(0.3);
            }  
        }     
    }
}
