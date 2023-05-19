project(tests)

add_subdirectory(lib/gtest)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_subdirectory(BufferPool)
add_subdirectory(ConcurrentQueue)
add_subdirectory(SavingToFile)

add_executable(all_tests BufferPool/tst_BufferPool.cpp ConcurrentQueue/tst_ConcurrentQueue.cpp SavingToFile/tst_SavingToFile.cpp)
target_link_libraries(all_tests gtest gtest_main)
target_link_libraries(all_tests concurrent_queue buffer_pool)