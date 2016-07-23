cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(report VERSION 0.1.0 LANGUAGES CXX)

# Compiler
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler Options
if(MSVC)
  foreach(flag
      CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
      CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE)
    if(${flag} MATCHES "/MD")
      string(REPLACE "/MD" "/MT" ${flag} "${${flag}}")
    endif()
    if(CMAKE_VS_PLATFORM_TOOLSET MATCHES "_xp")
      set(${flag} "${${flag}} /arch:IA32")
    endif()
  endforeach()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")
else()
  foreach(flag CMAKE_C_FLAGS CMAKE_CXX_FLAGS)
    set(${flag} "${${flag}} -Wall -Wextra -Werror -Wno-unused-parameter")
  endforeach()
endif()

# Definitions
if(WIN32)
  add_definitions(-D_UNICODE -DUNICODDE -DWIN32_LEAN_AND_MEAN -DNOMINMAX)
  add_definitions(-D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE -D_ATL_SECURE_NO_DEPRECATE)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS)
  if(CMAKE_VS_PLATFORM_TOOLSET MATCHES "_xp")
    add_definitions(-DWINVER=0x0501 -D_WIN32_WINNT=0x0501)
  else()
    add_definitions(-DWINVER=0x0A00 -D_WIN32_WINNT=0x0A00)
  endif()
endif()

# Linker Options
# FreeBSD: -Wl,-rpath=/usr/local/lib/gcc6
if(WIN32)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /manifestuac:NO /ignore:4099")
else()
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
endif()

# Packages
find_package(Threads REQUIRED)
find_package(asio REQUIRED PATHS third_party/asio)

# Configuration Files
set(PRODUCT_TEXT "Report")
set(PROJECT_TEXT "Report Server")
set(COPYING_TEXT "2016 Anonymous")
configure_file(res/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

# Sources
function(assign_source_group)
  foreach(source IN ITEMS ${ARGN})
    if(IS_ABSOLUTE ${source})
      file(RELATIVE_PATH source "${CMAKE_CURRENT_SOURCE_DIR}" "${source}")
    endif()
    get_filename_component(source_path "${source}" PATH)
    if(MSVC)
      string(REPLACE "/" "\\" source_path "${source_path}")
    endif()
    source_group("${source_path}" FILES "${source}")
  endforeach()
endfunction(assign_source_group)

file(GLOB_RECURSE res res/*.h.in res/*.rc)
assign_source_group(${res})

file(GLOB_RECURSE headers src/*.h)
assign_source_group(${headers})

file(GLOB_RECURSE sources src/*.cc)
assign_source_group(${sources})

# Executable
add_executable(${PROJECT_NAME} ${res} ${headers} ${sources})
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME $<LOWER_CASE:${PROJECT_NAME}>)
target_link_libraries(${PROJECT_NAME} PRIVATE ${CMAKE_THREAD_LIBS_INIT} asio)

# Include Directories
target_include_directories(${PROJECT_NAME}
  PRIVATE ${CMAKE_CURRENT_BINARY_DIR} src res)

# Install Target
install(TARGETS ${PROJECT_NAME} DESTINATION bin)

# Install License
find_program(PANDOC_PATH NAMES pandoc)
if(PANDOC_PATH)
  function(convert src dst)
    file(GLOB header res/header.html)
    execute_process(COMMAND ${PANDOC_PATH} -s -S -f markdown -t html -H "${header}" -o "${dst}" "${src}")
  endfunction(convert)

  file(READ license.md text)
  file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/license.md "${text}")

  file(GLOB licenses third_party/*/license.md)
  if(licenses)
    file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/license.md "\n")
    file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/license.md "## Third Party\n")
    file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/license.md "Licenses and statements from third party libraries.\n")
  endif()
  foreach(license IN ITEMS ${licenses})
    file(READ ${license} text)
    file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/license.md "\n")
    file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/license.md "${text}")
  endforeach()

  convert(${CMAKE_CURRENT_BINARY_DIR}/license.md ${CMAKE_CURRENT_BINARY_DIR}/license.html)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/license.html DESTINATION bin)
endif()
