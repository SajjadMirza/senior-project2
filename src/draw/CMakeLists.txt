# src/draw

file(GLOB DRAW_SRC *.cpp *.cc)

add_library(draw STATIC ${DRAW_SRC})
