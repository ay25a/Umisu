project(Umisu)

set(UMISU_BASE_DIR ${CMAKE_SOURCE_DIR}/Umisu)
set(UMISU_SOURCE_DIR ${UMISU_BASE_DIR}/src/Umisu)

set(UMISU_SOURCE_FILES
  ${UMISU_BASE_DIR}/src/main.cpp
)

#set(UMISU_HEADER_FILES)

add_executable(Umisu ${UMISU_SOURCE_FILES})
target_include_directories(Umisu 
  PRIVATE ${UMISU_SOURCE_DIR}/src
)

target_link_libraries(Umisu 
  PRIVATE York
)