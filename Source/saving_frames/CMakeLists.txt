cmake_minimum_required(VERSION 3.6)
project(saving_frames)

set(CMAKE_CXX_STANDARD 20)

include_directories(Semaphore)
include_directories(ConcurrentQueue)
include_directories(BufferPool)
include_directories(ReferenceCounter)

add_subdirectory(ReferenceCounter)
add_subdirectory(Semaphore)
add_subdirectory(ConcurrentQueue)
add_subdirectory(BufferPool)
add_subdirectory(tests)

add_library(saving_frames INTERFACE)
target_link_libraries(saving_frames INTERFACE reference_counter counting_semaphore concurrent_queue buffer_pool)
