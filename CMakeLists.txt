cmake_minimum_required(VERSION 3.14)
project(ProjectBeta C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Diligent Engine Setup ---
# We disable global installation and shared libraries to keep the build static/local
set(DILIGENT_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
set(DILIGENT_NO_FORMAT_VALIDATION ON CACHE BOOL "" FORCE)
set(DILIGENT_BUILD_TESTS OFF CACHE BOOL "" FORCE)

add_subdirectory(third_party/DiligentEngine)

# --- Source Files ---
file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.hpp")

# 2. Gather ONLY the files in src/tools
file(GLOB_RECURSE TOOLS_SOURCES "src/tools/*.cpp")

# 3. Remove the tools files from the main SOURCES list
#    This prevents "multiple definition of main" errors.
if(TOOLS_SOURCES)
    list(REMOVE_ITEM SOURCES ${TOOLS_SOURCES})
endif()

# --- Executable ---
add_executable(ProjectBeta ${SOURCES})

    
# ====================================================================
# START OF TARGETED PARITY FIX
#
# This is the safe and recommended approach.
# We apply strict floating-point flags ONLY to the source files
# responsible for world generation to avoid impacting the performance
# of other libraries like Diligent Engine.

# List all .cpp files that #include and use your world generation classes.
# In your case, it's most likely just main.cpp. If you split worldgen
# into its own .cpp file, add it to this list.
set(WORLDGEN_SOURCES
    src/main.cpp
)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    message(STATUS "Detected GCC/Clang, applying strict floating-point flags to worldgen sources.")
    # Use set_source_files_properties to apply flags to specific files.
    set_source_files_properties(${WORLDGEN_SOURCES}
        PROPERTIES COMPILE_FLAGS "-fno-fast-math -ffp-contract=off -ffloat-store"
    )
else()
    message(WARNING "Compiler is not GCC or Clang. Strict floating-point flags not applied. Parity may not be guaranteed.")
endif()
#
# END OF TARGETED PARITY FIX
# ====================================================================

# --- Includes ---
target_include_directories(ProjectBeta PRIVATE 
    src 
    third_party/DiligentEngine/DiligentCore/Graphics/GraphicsEngine/interface
    third_party/DiligentEngine/DiligentCore/Graphics/GraphicsEngineVulkan/interface
    third_party/DiligentEngine/DiligentCore/Common/interface
    third_party/DiligentEngine/DiligentCore/Platforms/Linux/interface
    third_party/DiligentEngine/DiligentTools/TextureLoader/interface
    third_party/DiligentEngine/DiligentCore/Graphics/GraphicsTools/interface
)

# --- Linking ---
# We link against the Core, Tools, and the Shader Compiler
target_link_libraries(ProjectBeta 
    PRIVATE 
    Diligent-GraphicsEngineVk-static
    Diligent-Archiver-static 
    Diligent-TextureLoader
)

# Link System Libraries (Linux Specific)
# We need X11 glfw/DL/Pthread for GLFW and Vulkan
target_link_libraries(ProjectBeta PRIVATE dl pthread X11 glfw)

# Handle Assets Copy
add_custom_command(TARGET ProjectBeta POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets ${CMAKE_BINARY_DIR}/assets
)