cmake_minimum_required(VERSION 3.5.0)
project(VkManaTemplate LANGUAGES CXX)

# CPM Dependency Management
include(cmake/CPM.cmake)

# Dependencies
include(Dependencies.cmake)

set(TARGET_NAME VkManaTemplate)

file(GLOB_RECURSE SOURCE_FILES src/**.cpp src/**.hpp)
add_executable(${TARGET_NAME} ${SOURCE_FILES})
set_target_properties(${TARGET_NAME}
    PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_link_libraries(${TARGET_NAME}
    PRIVATE
    fmt
    glm
    glfw
    VkMana
    imgui
)
