cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(DustparticleSim)


# Find Geant4 with UI and visualization enabled
find_package(Geant4 REQUIRED ui_all vis_all)

# Include Geant4 settings
include(${Geant4_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR}/include)

# Locate source and header files
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

file(GLOB MACRO_FILES "*.mac")

file(COPY ${MACRO_FILES} DESTINATION ${PROJECT_BINARY_DIR})

# Define executable
add_executable(dustparticle sim.cc ${sources} ${headers})

# Link necessary libraries
target_link_libraries(dustparticle ${Geant4_LIBRARIES})

# Custom build target
add_custom_target(DustparticleSim DEPENDS dustparticle)
