# src/sound

file(GLOB SOUND_SRC "*.cpp")

add_library(sound STATIC ${SOUND_SRC})
