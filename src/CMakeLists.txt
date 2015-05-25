# src

file(GLOB MAIN_SRC "*.cpp")

add_executable(myapp ${MAIN_SRC})

#add_custom_command(TARGET myapp POST_BUILD
#                  COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_BINARY_DIR}
#                   COMMAND ${CMAKE_COMMAND} -E copy_directory
#                   ${CMAKE_SOURCE_DIR}/resources ${CMAKE_SOURCE_DIR}/install/bin/resources)


add_subdirectory(sound)
add_subdirectory(draw)
set(INTERNAL_LIBS sound draw)



target_link_libraries(myapp ${INTERNAL_LIBS} ${EXTERNAL_LIBS})

install(TARGETS myapp DESTINATION ${INSTALL_DIR})

