project(concurrent_queue)

add_library(concurrent_queue ConcurrentQueue.hpp ConcurrentQueue.cpp)
target_link_libraries(concurrent_queue reference_counter counting_semaphore)

install(TARGETS concurrent_queue LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})