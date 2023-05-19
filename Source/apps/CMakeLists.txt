cmake_minimum_required(VERSION 3.6)

project(libcamera-vid)
add_executable(libcamera-vid libcamera_vid.cpp)
target_link_libraries(libcamera-vid saving_frames libcamera_app encoders outputs)


set(EXECUTABLES libcamera-vid)

set(EXECUTABLE_OUTPUT_PATH  ${CMAKE_BINARY_DIR})
install(TARGETS ${EXECUTABLES} RUNTIME DESTINATION bin)
