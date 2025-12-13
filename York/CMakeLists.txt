project(York)

set(YORK_BASE_DIR ${CMAKE_SOURCE_DIR}/York)
set(YORK_SOURCE_DIR ${YORK_BASE_DIR}/src/York)

set(YORK_SOURCE_FILES
  ${YORK_BASE_DIR}/vendor/wayland-extensions/xdg-shell-client-protocol.c
  ${YORK_BASE_DIR}/vendor/wayland-extensions/zwlr-layer-shell-unstable-v1-client-protocol.c
)

set(YORK_HEADER_FILES
  ${YORK_SOURCE_DIR}/Core/result.hpp
  
  ${YORK_BASE_DIR}/vendor/wayland-extensions/xdg-shell-client-protocol.h
  ${YORK_BASE_DIR}/vendor/wayland-extensions/zwlr-layer-shell-unstable-v1-client-protocol.h
)

add_library(York STATIC ${YORK_SOURCE_FILES} ${YORK_HEADER_FILES})
target_include_directories(York 
  PUBLIC ${YORK_BASE_DIR}/src
  PUBLIC ${YORK_BASE_DIR}/vendor/wayland-extensions
)

find_package(Vulkan REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(WAYLAND REQUIRED wayland-client wayland-cursor)

target_link_directories(York PUBLIC ${WAYLAND_LIBRARY_DIRS})
target_link_libraries(York 
  PUBLIC Vulkan::Vulkan 
  PUBLIC ${WAYLAND_LIBRARIES}
)