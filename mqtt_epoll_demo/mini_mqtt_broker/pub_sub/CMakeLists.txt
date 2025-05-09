cmake_minimum_required(VERSION 3.10)
project(MosquittoExample)

# Find Mosquitto library
#find_package(Mosquitto REQUIRED)

include_directories(./include)

link_libraries(mosquitto)
# Add your source files
set(SOURCES_SUB sub.cpp)
set(SOURCES_PUB pub.cpp)

# Link Mosquitto library
#target_link_libraries(sub PRIVATE Mosquitto::Mosquitto)
#target_link_libraries(pub PRIVATE Mosquitto::Mosquitto)
#link_libraries(mosquitto)

# Check the operating system and adjust settings accordingly
if (WIN32)
    # Additional settings for Windows
    # For example, you might want to link against specific Windows libraries or set compiler flags
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        link_directories(${CMAKE_CURRENT_SOURCE_DIR}/bin/win/x64/debug)
        SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/./bin/win/x64/debug)
    else()
        link_directories(${CMAKE_CURRENT_SOURCE_DIR}/bin/win/x64/release)
        SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/./bin/win/x64/release)
    endif()
    # target_link_libraries(sub PRIVATE WindowsLib)
    # target_link_libraries(pub PRIVATE WindowsLib)
    # target_compile_definitions(sub PRIVATE WINDOWS_DEFINE)
    # target_compile_definitions(pub PRIVATE WINDOWS_DEFINE)
elseif (UNIX)
    # Additional settings for Linux
    # For example, you might want to link against specific Linux libraries or set compiler flags
     if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	#link_directories(${CMAKE_CURRENT_SOURCE_DIR}/bin/linux/x64/debug)
	#SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/./bin/linux/x64/debug)
    else()
	#link_directories(${CMAKE_CURRENT_SOURCE_DIR}/bin/linux/x64/release)
	#SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/./bin/linux/x64/release)
    endif()
    # target_link_libraries(sub PRIVATE LinuxLib)
    # target_link_libraries(pub PRIVATE LinuxLib)
    # target_compile_definitions(sub PRIVATE LINUX_DEFINE)
    # target_compile_definitions(pub PRIVATE LINUX_DEFINE)
endif ()

# Create executables
add_executable(sub ${SOURCES_SUB})
add_executable(pub ${SOURCES_PUB})
