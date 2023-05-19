project(reference_counter)

add_library(reference_counter ReferenceCounter.cpp ReferenceCounter.hpp)

install(TARGETS reference_counter LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})