option(FETCH_DEPS "Enable fetching external dependencies" OFF)

if(FETCH_DEPS)
include(FetchContent)

# Set defaults to prevent interactive prompts
set(FETCHCONTENT_QUIET OFF)
set(FETCHCONTENT_UPDATES_DISCONNECTED ON)

# Bullet Physics
FetchContent_Declare(
  bullet
  GIT_REPOSITORY https://github.com/bulletphysics/bullet3.git
  GIT_TAG        v3.24
)
FetchContent_MakeAvailable(bullet)

# SDL2 (for cross-platform window/input)
FetchContent_Declare(
  sdl2
  GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
  GIT_TAG        2.28.2
)
FetchContent_MakeAvailable(sdl2)

# GLM (math)
FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG        0.9.9.8
)
FetchContent_MakeAvailable(glm)

# stb (single-header utilities)
FetchContent_Declare(
  stb
  GIT_REPOSITORY https://github.com/nothings/stb.git
  GIT_TAG        v3.24
)
FetchContent_MakeAvailable(stb)

# ImGui (editor/debug)
FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG        v3.24
)
FetchContent_MakeAvailable(imgui)

# Assimp (model loading)
FetchContent_Declare(
  assimp
  GIT_REPOSITORY https://github.com/assimp/assimp.git
  GIT_TAG        v3.24
)
FetchContent_MakeAvailable(assimp)

# OpenAL-soft (optional audio backend)
FetchContent_Declare(
  openal
  GIT_REPOSITORY https://github.com/kcat/openal-soft.git
  GIT_TAG        v3.24
)
FetchContent_MakeAvailable(openal)

else()
  message(STATUS "FETCH_DEPS is OFF; skipping external dependency fetch. Use -DFETCH_DEPS=ON to enable network fetch.")
endif()
