cmake_minimum_required (VERSION 3.14)

SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

# Set the project name
project (Test)

# Create a sources variable with a link to all cpp files to compile
set (
  SOURCES 
  test/test_main.cpp
)

# Add an executable with the above sources
add_executable(${PROJECT_NAME} ${SOURCES})

# Add preprocessor variable
add_compile_definitions(PERFORMANCE_TEST)

# Set the directories that should be included in the build command for this target
# when running g++ these will be included as -I/directory/path/
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${PROJECT_SOURCE_DIR}/src
        $(PROJECT_SOURCE_DIR)/test
        ${PROJECT_SOURCE_DIR}/dependencies/googletest/include
)

# Set the library files to be used during the linking phase
target_link_libraries(${PROJECT_NAME}
    ${PROJECT_SOURCE_DIR}/test/test_helper.cpp
    ${PROJECT_SOURCE_DIR}/dependencies/googletest/lib/libgtest.a
    ${PROJECT_SOURCE_DIR}/dependencies/googletest/lib/libgtest_main.a
)


