# src

add_executable(myapp main.cpp)

add_custom_command(TARGET myapp POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_BINARY_DIR}       
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_SOURCE_DIR}/resources ${CMAKE_SOURCE_DIR}/install/bin/resources)


add_subdirectory(sound)
add_subdirectory(draw)


target_link_libraries(myapp ${ASSIMP} sound draw glfw ${GLFW_LIBRARIES} ${IRRKLANG_LIBRARY} ${FMOD})

install(TARGETS myapp DESTINATION bin) 

