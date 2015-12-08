#include <sound/FMODDriver.hpp>

#include <log.hpp>

namespace sound {
    //"dvorak-symphony-no9-mvtII.ogg"
    const char *sound_test = "resources/sounds/dvorak-symphony-no9-mvtII.ogg";
//    const char *sound_test = "resources/sounds/Ode_to_Joy.ogg";
    const char *sound_foot = "resources/sounds/foot_step_wood.ogg";
    const char *sound_energy = "resources/sounds/energy.ogg";
    const char *sound_start_up = "resources/sounds/start_up.wav";
    const char *door = "resources/sounds/tos-turboliftdoor.ogg";
    const char *button = "resources/sounds/button.wav";


    static FMOD::Channel *foot_channel;
    static FMOD::Channel *hanoi_channel;
    static FMOD::Channel *start_channel;
    static FMOD::Channel *door_channel;
    static FMOD::Channel *button_channel;


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
        channel->setVolume(0.2);
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
                unsigned int lengthms;

                FMOD::Sound *audio;
                system->createSound(sound_foot, FMOD_LOOP_OFF, 0, &audio);

                system->playSound(audio, NULL, false, &foot_channel); 
                foot_channel->setVolume(0.3);
            }  
        }     
    }

    void FMODDriver::EnergySound() {
        FMOD::Sound *audio;
        system->createSound(sound_energy, FMOD_LOOP_OFF, 0, &audio);

        hanoi_channel->setVolume(0.25);
        system->playSound(audio, NULL, false, &hanoi_channel);
    }

    void FMODDriver::StartSound() {
        bool playing;
        start_channel->isPlaying(&playing);

        if (!playing) {
            FMOD::Sound *audio;
            system->createSound(sound_start_up, FMOD_LOOP_OFF, 0, &audio);

            start_channel->setVolume(0.2);
            system->playSound(audio, NULL, false, &start_channel);
        }
    }

    void FMODDriver::HanoiCheck() {

        bool playing;
        hanoi_channel->isPlaying(&playing);

        if (playing) {
            float sound;
            hanoi_channel->getVolume(&sound);

            if (sound > 0) {
                hanoi_channel->setVolume(sound - 0.015);
            }
            else {
                hanoi_channel->stop();
            }
        }
    } 

    void FMODDriver::StartCheck() {

        bool playing;
        start_channel->isPlaying(&playing);

        if (playing) {
            float sound;
            start_channel->getVolume(&sound);

            if (sound > 0) {
                start_channel->setVolume(sound - 0.015);
            }
            else {
                start_channel->stop();
            }
        }
    } 

    void FMODDriver::DoorSound() {
        bool playing;
        door_channel->isPlaying(&playing);

        if (!playing) {
            FMOD::Sound *audio;
            system->createSound(door, FMOD_LOOP_OFF, 0, &audio);

            door_channel->setVolume(0.1);
            system->playSound(audio, NULL, false, &door_channel);
        }
    }

    void FMODDriver::DoorCheck() {

        bool playing;
        door_channel->isPlaying(&playing);

        if (playing) {
            float sound;
            door_channel->getVolume(&sound);

            if (sound > 0) {
                door_channel->setVolume(sound - 0.015);
            }
            else {
                door_channel->stop();
            }
        }
    }

    void FMODDriver::ButtonSound() {
        bool playing;
        button_channel->isPlaying(&playing);

        if (!playing) {
            FMOD::Sound *audio;
            system->createSound(button, FMOD_LOOP_OFF, 0, &audio);

            button_channel->setVolume(0.15);
            system->playSound(audio, NULL, false, &button_channel);
        }
    }

    void FMODDriver::ButtonCheck() {

        bool playing;
        button_channel->isPlaying(&playing);

        if (playing) {
            float sound;
            button_channel->getVolume(&sound);

            if (sound > 0) {
                button_channel->setVolume(sound - 0.015);
            }
            else {
                button_channel->stop();
            }
        }
    }
}
