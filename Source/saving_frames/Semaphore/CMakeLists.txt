project(counting_semaphore)

add_library(counting_semaphore CountingSemaphore.cpp CountingSemaphore.h)

install(TARGETS counting_semaphore LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
