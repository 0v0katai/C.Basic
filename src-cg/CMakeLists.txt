# Configure with [fxsdk build-fx] or [fxsdk build-cg], which provide the
# toolchain file and module path of the fxSDK

set(CB_VERSION "01.47.0200")
cmake_policy(SET CMP0048 NEW)
project(CBASIC LANGUAGES ASM C VERSION ${CB_VERSION})

string(SUBSTRING ${CB_VERSION} 6 2 BUILD)
if (${BUILD} MATCHES "^0.")
  string(SUBSTRING ${BUILD} 1 1 BUILD)
endif()

set(CB_VERSION_VALUE "${CBASIC_VERSION_MAJOR}${CBASIC_VERSION_MINOR}")
set(CB_VERSION_STRING "${CBASIC_VERSION_MAJOR}.${CBASIC_VERSION_MINOR}")

find_package(Git REQUIRED)
execute_process(COMMAND "${GIT_EXECUTABLE}" rev-parse HEAD
                OUTPUT_VARIABLE COMMIT_ID OUTPUT_STRIP_TRAILING_WHITESPACE)
string(SUBSTRING ${COMMIT_ID} 0 7 COMMIT_ID_SHORT)
cmake_minimum_required(VERSION 3.15)

include(GenerateG1A)
include(GenerateG3A)
include(GenerateHH2Bin)
include(Fxconv)

set(SOURCES
  src/CBP_bmp.c
  src/CBP_Color.c
  src/CBP_edit.c
  src/CBP_error.c
  src/CBP_Eval_ext.c
  src/CBP_Eval.c
  src/CBP_file.c
  src/CBP_fileMCS.c
  src/CBP_glib.c
  src/CBP_glib2.c
  src/CBP_GraphFunc.c
  src/CBP_Help.c
  src/CBP_icon.c
  src/CBP_inp.c
  src/CBP_inpcmdF1.c
  src/CBP_inpcmd.c
  src/CBP_inpcmdF2.c
  src/CBP_inpcmdF3.c
  src/CBP_inpcmdF4.c
  src/CBP_inpcmdF5.c
  src/CBP_inpcmdF6.c
  src/CBP_interpreter.c
  src/CBP_interpreterVRAM.c
  src/CBP_io_fkeyicon.c
  src/CBP_io.c
  src/CBP_KeyScan.c
  src/CBP_Kana.c
  src/CBP_ListEval.c
  src/CBP_main.c
  src/CBP_Matrix.c
  src/CBP_MonochromeLib.c
  src/CBP_serial.c
  src/CBP_setup.c
  src/CBP_Str.c
  src/CBP_StrExt.c
  src/CBP_sys.c
  src/CBP_TextConv.c
  src/CBP_Time.c
  src/CBPC_Eval.c
  src/CBPC_ListEval.c
  src/CBPI_Eval.c
  src/CBPI_interpreter.c
  src/CBPI_ListEval.c
  src/MonochromeLibCG.c
  src/Ptune2_direct.c
  src/qsort.c
  src/crt0.s
  src/syscall.s
  src/va_lib.c
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
                    "${CMAKE_CURRENT_SOURCE_DIR}/include"
                    "${CMAKE_CURRENT_SOURCE_DIR}/include/cbasic"
                    "${CMAKE_CURRENT_BINARY_DIR}/include/cbasic")
target_compile_options(CBASIC PRIVATE
                       -nostdlib -I include
                       -Wall -Wextra -Os -g -flto=auto
                       -mpretend-cmove)
target_link_options(CBASIC PRIVATE
                    -nostdlib -L ${CMAKE_CURRENT_SOURCE_DIR}/lib
                    -Wl,--print-memory-usage -Wl,-Map=map -Wl,--gc-sections)
if("${CMAKE_BUILD_TYPE}" STREQUAL "FastLoad")
  target_link_options(CBASIC PRIVATE
                      -T "${CMAKE_CURRENT_SOURCE_DIR}/prizm_fastload.ld")
else()
  target_link_options(CBASIC PRIVATE
                      -T "${CMAKE_CURRENT_SOURCE_DIR}/prizm.ld")
endif()
target_link_libraries(CBASIC PRIVATE -lm -lc -lfxcg -lgcc)

if("${FXSDK_PLATFORM_LONG}" STREQUAL fx9860G)
  generate_g1a(TARGET CBASIC VERSION ${CB_VERSION} OUTPUT "CBASIC.g1a"
    NAME "CBASIC" ICON assets-fx/icon.png)
elseif("${FXSDK_PLATFORM_LONG}" STREQUAL fxCG50)
  generate_g3a(TARGET CBASIC VERSION ${CB_VERSION} OUTPUT "CBCG${CB_VERSION_VALUE}.g3a"
    NAME "CBCG${CB_VERSION_VALUE}" ICONS assets-cg/icon-uns.png assets-cg/icon-sel.png)
elseif("${FXSDK_PLATFORM_LONG}" STREQUAL fx9860G_G3A)
  generate_g3a(TARGET CBASIC VERSION ${CB_VERSION} OUTPUT "CBASIC-fx.g3a"
    NAME "CBASIC-fx" ICONS assets-cg/icon-uns.png assets-cg/icon-sel.png)
elseif("${FXSDK_PLATFORM_LONG}" STREQUAL fxCP)
  generate_hh2_bin(TARGET CBASIC VERSION ${CB_VERSION} OUTPUT "CBASIC-hh2.bin")
endif()
