cmake_minimum_required (VERSION 3.15)

# Set the project name
project (Test)

# Create a sources variable with a link to all cpp files to compile
set(
  SOURCES 
  ${PROJECT_SOURCE_DIR}/test/test_main.cpp
  ${PROJECT_SOURCE_DIR}/test/test_helper.cpp
)

# Add an executable with the above sources
add_executable(${PROJECT_NAME} ${SOURCES})

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  Message ( STATUS "Setting GNU switches")
  set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
  set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  Message ( STATUS "Setting MSVC switches")
  set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
  set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3")
  set_property(TARGET ${PROJECT_NAME} PROPERTY
              MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
	MESSAGE(STATUS "current platform: Linux, add pthread")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -pthread")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -pthread")
endif (CMAKE_SYSTEM_NAME MATCHES "Linux")

# Add preprocessor variable
add_compile_definitions(PERFORMANCE_TEST)

# Set the directories that should be included in the build command for this target
# when running g++ these will be included as -I/directory/path/
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${PROJECT_SOURCE_DIR}/src
        ${PROJECT_SOURCE_DIR}/test
        ${PROJECT_SOURCE_DIR}/dependencies/googletest/include
)

# Set the library files to be used during the linking phase
set(GTEST_LIBD_MSVC
    debug ${PROJECT_SOURCE_DIR}/dependencies/googletest/lib/gtestd.lib
    debug ${PROJECT_SOURCE_DIR}/dependencies/googletest/lib/gtest_maind.lib
    debug ${PROJECT_SOURCE_DIR}/dependencies/googletest/lib/gmockd.lib
    debug ${PROJECT_SOURCE_DIR}/dependencies/googletest/lib/gmock_maind.lib
)
set(GTEST_LIB_MSVC
    optimized ${PROJECT_SOURCE_DIR}/dependencies/googletest/lib/gtest.lib
    optimized ${PROJECT_SOURCE_DIR}/dependencies/googletest/lib/gtest_main.lib
    optimized ${PROJECT_SOURCE_DIR}/dependencies/googletest/lib/gmock.lib
    optimized ${PROJECT_SOURCE_DIR}/dependencies/googletest/lib/gmock_main.lib
)
set(GTEST_LIBD_GNU
    debug ${PROJECT_SOURCE_DIR}/dependencies/googletest/lib/libgtestd.a
    debug ${PROJECT_SOURCE_DIR}/dependencies/googletest/lib/libgtest_maind.a
    debug ${PROJECT_SOURCE_DIR}/dependencies/googletest/lib/libgmockd.a
    debug ${PROJECT_SOURCE_DIR}/dependencies/googletest/lib/libgmock_maind.a
)
set(GTEST_LIB_GNU
    optimized ${PROJECT_SOURCE_DIR}/dependencies/googletest/lib/libgtest.a
    optimized ${PROJECT_SOURCE_DIR}/dependencies/googletest/lib/libgtest_main.a
    optimized ${PROJECT_SOURCE_DIR}/dependencies/googletest/lib/libgmock.a
    optimized ${PROJECT_SOURCE_DIR}/dependencies/googletest/lib/libgmock_main.a
)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  target_link_libraries(${PROJECT_NAME}
    ${GTEST_LIBD_MSVC}
    ${GTEST_LIB_MSVC}
  )
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  target_link_libraries(${PROJECT_NAME}
    ${GTEST_LIBD_GNU}
    ${GTEST_LIB_GNU}
  )
endif()