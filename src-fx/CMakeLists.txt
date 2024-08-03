# Configure with [fxsdk build-fx] or [fxsdk build-cg], which provide the
# toolchain file and module path of the fxSDK

set(CBASIC_VERSION "02.47.2000")

if(${CBASIC_VERSION} MATCHES "^0.*")
  string(SUBSTRING ${CBASIC_VERSION} 1 1 MAJOR)
else()
  string(SUBSTRING ${CBASIC_VERSION} 0 2 MAJOR)
endif()

string(SUBSTRING ${CBASIC_VERSION} 3 2 MINOR)

if(${CBASIC_VERSION} MATCHES "${MAJOR}.${MINOR}.0.*")
  string(SUBSTRING ${CBASIC_VERSION} 7 1 BUILD)
else()
  string(SUBSTRING ${CBASIC_VERSION} 6 2 BUILD)
endif()

set(CBASIC_VERSION_VALUE "${MAJOR}${MINOR}")
set(CBASIC_VERSION_STRING "${MAJOR}.${MINOR}")

find_package(Git REQUIRED)
execute_process(COMMAND "${GIT_EXECUTABLE}" rev-parse HEAD
                OUTPUT_VARIABLE COMMIT_ID OUTPUT_STRIP_TRAILING_WHITESPACE)
string(SUBSTRING ${COMMIT_ID} 0 7 COMMIT_ID_SHORT)
cmake_minimum_required(VERSION 3.15)
project(CBASIC LANGUAGES ASM C VERSION ${CBASIC_VERSION})

include(GenerateG1A)
include(GenerateG3A)
include(GenerateHH2Bin)
include(Fxconv)

set(SOURCES
  src/CB_interpreter.c
  src/CB_Eval.c
  src/CBI_interpreter.c
  src/CBI_Eval.c
  src/CBC_Eval.c
  src/CBI_ListEval.c
  src/CB_ListEval.c
  src/CBC_ListEval.c
  src/CB_Matrix.c
  src/CB_glib.c
  src/CB_interpreterVRAM.c
  src/MonochromeLib.c
  src/CB_MonochromeLib.c
  src/CB_Str.c
  src/CB_io.c
  src/CB_Kana.c
  src/KeyScan.c
  src/fx_syscall.s
  src/CB_glib2.c
  src/CB_Eval_ext.c
  src/CB_GraphFunc.c
  src/CBZ_Time.c
  src/CBZ_inp.c
  src/CBZ_edit.c
  src/CBZ_error.c
  src/CBZ_file.c
  src/CBZ_setup.c
  src/CBZ_TextConv.c
  src/CBZ_main.c
  src/CBZ_inpcmd.c
  src/CBZ_inpcmdF1.c
  src/CBZ_inpcmdF6.c
  src/CBZ_inpcmdF5.c
  src/CBZ_inpcmdF4.c
  src/CBZ_inpcmdF3.c
  src/CBZ_inpcmdF2.c
  src/CB_io_fkeyicon.c
  src/CBZ_bmp.c
  src/CBZ_serial.c
  src/CBZ_fileMCS.c
  src/CBZ_Help.c
  src/CB_StrExt.c
  src/crt0.s
  # ...
)
# Shared assets, fx-9860G-only assets and fx-CG-50-only assets
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
include_directories(CBASIC PRIVATE
                        "${CMAKE_CURRENT_SOURCE_DIR}/include/cbasic"
                        "${CMAKE_CURRENT_BINARY_DIR}/include/cbasic"
                        "${CMAKE_CURRENT_SOURCE_DIR}/include/cbasic/shc")
target_compile_options(CBASIC PRIVATE
                        -nostdlib -I include
                        -Wall -Wextra -Os -g -flto=auto
                        )
target_link_options(CBASIC PRIVATE
                        -nostdlib -T "${CMAKE_CURRENT_SOURCE_DIR}/addin.ld" -L ${CMAKE_CURRENT_SOURCE_DIR}
                        -Wl,--print-memory-usage -Wl,-Map=map)
target_link_libraries(CBASIC PRIVATE -lm -lfx -lgcc)


if("${FXSDK_PLATFORM_LONG}" STREQUAL fx9860G)
  generate_g1a(TARGET CBASIC VERSION ${CBASIC_VERSION} OUTPUT "CBASIC.g1a"
    NAME "CBASIC" ICON assets-fx/icon.png)
elseif("${FXSDK_PLATFORM_LONG}" STREQUAL fxCG50)
  generate_g3a(TARGET CBASIC VERSION ${CBASIC_VERSION} OUTPUT "CBASIC.g3a"
    NAME "CBASIC" ICONS assets-cg/icon-uns.png assets-cg/icon-sel.png)
elseif("${FXSDK_PLATFORM_LONG}" STREQUAL fx9860G_G3A)
  generate_g3a(TARGET CBASIC VERSION ${CBASIC_VERSION} OUTPUT "CBASIC-fx.g3a"
    NAME "CBASIC-fx" ICONS assets-cg/icon-uns.png assets-cg/icon-sel.png)
elseif("${FXSDK_PLATFORM_LONG}" STREQUAL fxCP)
  generate_hh2_bin(TARGET CBASIC VERSION ${CBASIC_VERSION} OUTPUT "CBASIC-hh2.bin")
endif()
