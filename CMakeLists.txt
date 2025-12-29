cmake_minimum_required(VERSION 3.15)
project(CBASIC LANGUAGES ASM C)

if("${FXSDK_PLATFORM_LONG}" STREQUAL fx9860G)
  set(CB_BUILD_TYPE "fx")
  set(CB_VERSION "02.50.0500")
elseif("${FXSDK_PLATFORM_LONG}" STREQUAL fxCG50 OR
       "${FXSDK_PLATFORM_LONG}" STREQUAL fx9860G_G3A)
  set(CB_BUILD_TYPE "cg")
  set(CB_VERSION "01.50.0100")
endif()
project(CBASIC VERSION ${CB_VERSION})
set(CB_SOURCE_BASE "${CMAKE_CURRENT_SOURCE_DIR}/src-${CB_BUILD_TYPE}")
file(GLOB_RECURSE SOURCES "${CB_SOURCE_BASE}/src/*.[c|s]")

math(EXPR BUILD "${CBASIC_VERSION_PATCH}/100")
set(CB_VERSION_VALUE "${CBASIC_VERSION_MAJOR}${CBASIC_VERSION_MINOR}")
set(CB_VERSION_STRING "${CBASIC_VERSION_MAJOR}.${CBASIC_VERSION_MINOR}")

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
    "${CMAKE_CURRENT_BINARY_DIR}/include/cbasic")
include_directories(CBASIC PRIVATE ${INCLUDE_DIRS})

set(COMPILE_OPTS
    -nostdlib -I ${CB_SOURCE_BASE}/include
    -Wall -Wextra -Os -g -flto=auto -mpretend-cmove)
target_compile_options(CBASIC PRIVATE ${COMPILE_OPTS})

set(LINK_OPTS
    -nostdlib -L ${CB_SOURCE_BASE}/lib
    -Wl,-print-memory-usage -Wl,-Map=map -Wl,-gc-sections)
if("${CB_BUILD_TYPE}" STREQUAL fx)
  list(APPEND LINK_OPTS -T ${CB_SOURCE_BASE}/fx9860.ld)
elseif("${CB_BUILD_TYPE}" STREQUAL cg)
  if("${CMAKE_BUILD_TYPE}" STREQUAL "FastLoad")
    list(APPEND LINK_OPTS -T ${CB_SOURCE_BASE}/prizm_fastload.ld)
  else()
    list(APPEND LINK_OPTS -T ${CB_SOURCE_BASE}/prizm.ld)
  endif()
endif()
target_link_options(CBASIC PRIVATE ${LINK_OPTS})

set(LIBS -lm -lgcc)
if("${CB_BUILD_TYPE}" STREQUAL fx)
  list(APPEND LIBS -lfx)
elseif("${CB_BUILD_TYPE}" STREQUAL cg)
  list(APPEND LIBS -lc -lfxcg -lqsort)
endif()
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
