cmake_minimum_required (VERSION 3.14)

# Set the project name
project (Test)

# Create a sources variable with a link to all cpp files to compile
set (
  SOURCES 
  test/test_main.cpp
  test/test_alloc.h
  test/test_vector.h
)

# Add an executable with the above sources
add_executable(${PROJECT_NAME} ${SOURCES})

# Set the directories that should be included in the build command for this target
# when running g++ these will be included as -I/directory/path/
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${PROJECT_SOURCE_DIR}/src
)


