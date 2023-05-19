project(buffer_pool)

add_library(buffer_pool Buffer.cpp BufferPool.cpp BufferPool.hpp Buffer.hpp)
target_link_libraries(buffer_pool reference_counter)

install(TARGETS buffer_pool LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})