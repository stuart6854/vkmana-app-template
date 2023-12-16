# FmtLib string format
CPMAddPackage(
  NAME fmt
  GIT_TAG 10.1.1
  GITHUB_REPOSITORY fmtlib/fmt
  SYSTEM ON
)

# GLM Maths
CPMAddPackage(
  NAME glm
  GIT_TAG 0.9.9.8
  GITHUB_REPOSITORY g-truc/glm
  SYSTEM ON
)

# GLFW
CPMAddPackage(
  NAME glfw
  GIT_TAG 3.3.9
  GITHUB_REPOSITORY glfw/glfw
  SYSTEM ON
  OPTIONS
  "GLFW_BUILD_TESTS OFF"
  "GLFW_BUILD_EXAMPLES OFF"
  "GLFW_BULID_DOCS OFF"
)

# VkMana
CPMAddPackage(
  NAME VkMana
  GIT_TAG main
  GITHUB_REPOSITORY stuart6854/VkMana
  SYSTEM ON
  OPTIONS
  "VKMANA_BUILD_SAMPLES OFF"
)

# Dear ImGui
CPMAddPackage(
  NAME imgui
  GIT_TAG docking
  GITHUB_REPOSITORY ocornut/imgui
  DOWNLOAD_ONLY
)

if(imgui_ADDED)
  file(GLOB IMGUI_SOURCE ${imgui_SOURCE_DIR}/**.cpp)
  add_library(imgui ${IMGUI_SOURCE})
  target_include_directories(imgui SYSTEM PUBLIC INTERFACE ${imgui_SOURCE_DIR})
endif()
