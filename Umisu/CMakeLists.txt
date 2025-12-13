project(MikuAgent)

set(MAGENT_SOURCE_DIR ${MIKU_SOURCE_DIR}/MikuAgent)

set(MAGENT_SOURCE_FILES 
  ${MAGENT_SOURCE_DIR}/src/main.cpp
  ${MAGENT_SOURCE_DIR}/src/window.cpp
  ${MAGENT_SOURCE_DIR}/src/MikuAgent.cpp
  ${MAGENT_SOURCE_DIR}/src/vulkan-context.cpp
  ${MAGENT_SOURCE_DIR}/src/renderer.cpp

  ${MAGENT_SOURCE_DIR}/src/window.hpp
  ${MAGENT_SOURCE_DIR}/src/MikuAgent.hpp
  ${MAGENT_SOURCE_DIR}/src/vulkan-context.hpp
  ${MAGENT_SOURCE_DIR}/src/renderer.hpp

  ${CMAKE_SOURCE_DIR}/vendor/wayland/xdg-shell-client-protocol.c
  ${CMAKE_SOURCE_DIR}/vendor/wayland/zwlr-layer-shell-unstable-v1-client-protocol.c

)
add_executable(miku ${MAGENT_SOURCE_FILES})

find_package(Vulkan REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(WAYLAND REQUIRED wayland-client wayland-cursor)
link_directories(${WAYLAND_LIBRARY_DIRS})

target_link_libraries(miku 
  PRIVATE Vulkan::Vulkan
  PRIVATE ${WAYLAND_LIBRARIES}
)