#----------------------------------------------------
# Setup the project
#----------------------------------------------------

cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(pgsproject)

#----------------------------------------------------
# Cpp flags
#----------------------------------------------------

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

#----------------------------------------------------
# G4 flags
#----------------------------------------------------

add_definitions(-DG4MULTITHREADED)

#----------------------------------------------------
# Find packages
#----------------------------------------------------

find_package(Geant4 REQUIRED ui_all vis_all)
find_package(ROOT REQUIRED COMPONENTS Core)

if(Geant4_FOUND) 
    include(${Geant4_USE_FILE})
else() 
    message(FATAL_ERROR "Geant4 not found!")
endif()

if(ROOT_FOUND)
    include(${ROOT_USE_FILE})
else()
    message(FATAL_ERROR "Geant4 not found!")
endif() 

#----------------------------------------------------
# Include directory for this project
#----------------------------------------------------

set(PROJECT_INCLUDEs_DIR ${PROJECT_SOURCE_DIR}/headers)
set(PROJECT_SOURCEs_DIR ${PROJECT_SOURCE_DIR}/sources)

#----------------------------------------------------
# Locate sources and headers for this project
#----------------------------------------------------

file(GLOB_RECURSE sources ${PROJECT_SOURCEs_DIR}/*.cpp)
file(GLOB_RECURSE headers ${PROJECT_INCLUDEs_DIR}/*.h)
# /home/mats/projects/geant4/pMultiStepPGGenerator/sharedFiles
file(GLOB_RECURSE shared_sources ${PROJECT_SOURCE_DIR}/sharedFiles/*.h)

#----------------------------------------------------
# Add the executable
#----------------------------------------------------

add_executable(simulation main.cpp ${sources} ${headers})

target_include_directories(simulation PRIVATE ${PROJECT_INCLUDEs_DIR})
target_link_libraries(simulation PRIVATE ${Geant4_LIBRARIES} ${ROOT_LIBRARIES})

#----------------------------------------------------
# Move files to backup
#----------------------------------------------------

file(COPY macros DESTINATION ${PROJECT_BINARY_DIR}/macros)

add_custom_command(TARGET simulation POST_BUILD    
    COMMAND rsync -a --exclude=build* --exclude=bin* ${PROJECT_SOURCE_DIR}/ ${PROJECT_BINARY_DIR}/backup/
)

#----------------------------------------------------
# Install the executable to 'bin'
#----------------------------------------------------

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
endif()

install(TARGETS simulation 
        DESTINATION ${PROJECT_SOURCE_DIR}/bin 
        CONFIGURATIONS Release Debug
)

install(FILES ${simulation_SCRIPTS} DESTINATION ${PROJECT_SOURCE_DIR}/macros)

#----------------------------------------------------
# Move macros to bin
#----------------------------------------------------

#file(COPY macros DESTINATION ${PROJECT_SOURCE_DIR}/bin)

#----------------------------------------------------
# Create costum target for build dependency
#----------------------------------------------------

add_custom_target(pgsproject DEPENDS simulation)
