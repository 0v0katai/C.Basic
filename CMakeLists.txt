cmake_minimum_required(VERSION 3.15)
project(CBASIC LANGUAGES ASM C)

string(TOLOWER "${FXSDK_PLATFORM}" CB_PLATFORM)
if("${CB_PLATFORM}" STREQUAL fx)
  set(CB_VERSION "02.50.0500")
elseif("${CB_PLATFORM}" STREQUAL cg)
  set(CB_VERSION "01.50.0100")
  set(LIBS "-lc")
endif()
project(CBASIC VERSION ${CB_VERSION})
math(EXPR BUILD "${CBASIC_VERSION_PATCH}/100")
set(CB_VERSION_VALUE "${CBASIC_VERSION_MAJOR}${CBASIC_VERSION_MINOR}")
set(CB_VERSION_STRING "${CBASIC_VERSION_MAJOR}.${CBASIC_VERSION_MINOR}")

set(CB_SOURCE_BASE "${CMAKE_CURRENT_SOURCE_DIR}/src-${CB_PLATFORM}")
set(CB_LINKER_BASE "${CMAKE_CURRENT_SOURCE_DIR}/linker")
set(LINKER "${CB_LINKER_BASE}/${CB_PLATFORM}.ld")
if("${CMAKE_BUILD_TYPE}" STREQUAL "FastLoad")
  set(LINKER "${CB_LINKER_BASE}/fastload.ld")
endif()

file(GLOB_RECURSE SOURCES "${CB_SOURCE_BASE}/src/*.[c|S]")
file(GLOB_RECURSE COMMONS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.[c|S]")
list(APPEND SOURCES ${COMMONS})

find_package(Git REQUIRED)
execute_process(COMMAND "${GIT_EXECUTABLE}" rev-parse HEAD
                OUTPUT_VARIABLE COMMIT_ID OUTPUT_STRIP_TRAILING_WHITESPACE)
string(SUBSTRING ${COMMIT_ID} 0 7 COMMIT_ID_SHORT)

include(GenerateG1A)
include(GenerateG3A)
include(GenerateHH2Bin)
include(Fxconv)

set(ASSETS
  # ...
)
set(ASSETS_fx
  # ...
)
set(ASSETS_cg
  # ...
)

fxconv_declare_assets(${ASSETS} ${ASSETS_fx} ${ASSETS_cg} WITH_METADATA)

configure_file(include/cbasic/config.h.in include/cbasic/config.h)
add_executable(CBASIC ${SOURCES} ${ASSETS} ${ASSETS_${FXSDK_PLATFORM}})

set(INCLUDE_DIRS
    "${CB_SOURCE_BASE}/include"
    "${CB_SOURCE_BASE}/include/cbasic"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/cbasic"
    "${CMAKE_CURRENT_BINARY_DIR}/include/cbasic")
include_directories(CBASIC PRIVATE ${INCLUDE_DIRS})

set(COMPILE_OPTS
    -I ${CB_SOURCE_BASE}/include
    -Wall -Wextra -Os -g -flto=auto -mpretend-cmove)
target_compile_options(CBASIC PRIVATE ${COMPILE_OPTS})

set(LINK_OPTS
    -L ${CMAKE_CURRENT_SOURCE_DIR}/lib -T ${LINKER}
    -Wl,-print-memory-usage -Wl,-Map=map -Wl,-gc-sections)
target_link_options(CBASIC PRIVATE ${LINK_OPTS})

list(APPEND LIBS "-lm" "-lgcc" "-l${CB_PLATFORM}")
target_link_libraries(CBASIC PRIVATE ${LIBS})

if("${FXSDK_PLATFORM_LONG}" STREQUAL fx9860G)
  generate_g1a(TARGET CBASIC VERSION ${CB_VERSION} OUTPUT "CBASIC.g1a"
    NAME "CBASIC" ICON assets-fx/icon.png)
elseif("${FXSDK_PLATFORM_LONG}" STREQUAL fxCG50)
  generate_g3a(TARGET CBASIC VERSION ${CB_VERSION} OUTPUT "CBCG${CB_VERSION_VALUE}.g3a"
    NAME "CBCG${CB_VERSION_VALUE}" ICONS assets-cg/icon-uns.png assets-cg/icon-sel.png)
elseif("${FXSDK_PLATFORM_LONG}" STREQUAL fx9860G_G3A)
  generate_g3a(TARGET CBASIC VERSION ${CB_VERSION} OUTPUT "CBCG${CB_VERSION_VALUE}-fx.g3a"
    NAME "CBCG${CB_VERSION_VALUE}-fx" ICONS assets-cg/icon-uns.png assets-cg/icon-sel.png)
elseif("${FXSDK_PLATFORM_LONG}" STREQUAL fxCP)
  generate_hh2_bin(TARGET CBASIC VERSION ${CB_VERSION} OUTPUT "CBASIC-hh2.bin")
endif()
