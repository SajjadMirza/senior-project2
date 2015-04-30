# src/draw

file(GLOB DRAW_SRC "*.cpp")

add_library(draw STATIC ${DRAW_SRC})
