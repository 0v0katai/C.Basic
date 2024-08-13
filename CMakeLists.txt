# Configure with [fxsdk build-fx] or [fxsdk build-cg], which provide the
# toolchain file and module path of the fxSDK

cmake_minimum_required(VERSION 3.15)
project(CBASIC LANGUAGES ASM CXX C)

include(GenerateG1A)
include(GenerateG3A)
include(GenerateHH2Bin)
include(Fxconv)

set(SOURCES
  # src/LIB/source/strcat.s
  # src/LIB/source/memmove.s
  # src/LIB/source/memset.s
  # src/LIB/source/memsetZero.s
  # src/LIB/source/strchr.s
  # src/LIB/source/strcmp.s
  # src/LIB/source/strcpy.s
  # src/LIB/source/strncpy.s

  src/LIB/source/STATUS_ICONS.cpp
  src/CBP_bmp.cpp
  src/CBP_Color.cpp
  src/CBP_edit.cpp
  src/CBP_error.cpp
  src/CBP_Eval_ext.cpp
  src/CBP_Eval.cpp
  src/CBP_file.cpp
  src/CBP_fileMCS.cpp
  src/CBP_glib.cpp
  src/CBP_glib2.cpp
  src/CBP_Help.cpp
  src/CBP_icon.cpp
  src/CBP_inp.cpp
  src/CBP_inpcmdF1.cpp
  src/CBP_inpcmd.cpp
  src/CBP_inpcmdF2.cpp
  src/CBP_inpcmdF3.cpp
  src/CBP_inpcmdF4.cpp
  src/CBP_inpcmdF5.cpp
  src/CBP_inpcmdF6.cpp
  src/CBP_interpreter.cpp
  src/CBP_interpreterVRAM.cpp
  src/CBP_io_fkeyicon.cpp
  src/CBP_io.cpp
  src/CBP_KeyScan.cpp
  src/CBP_Kana.cpp
  src/CBP_ListEval.cpp
  src/CBP_main.cpp
  src/CBP_Matrix.cpp
  src/CBP_MonochromeLib.cpp
  src/CBP_serial.cpp
  src/CBP_setup.cpp
  src/CBP_Str.cpp
  src/CBP_sys.cpp
  src/CBP_TextConv.cpp
  src/CBP_Time.cpp
  src/CBPC_Eval.cpp
  src/CBPC_ListEval.cpp
  src/CBPI_Eval.cpp
  src/CBPI_interpreter.cpp
  src/CBPI_ListEval.cpp
  src/MonochromeLibCG.cpp
  src/Ptune2_direct.cpp
  src/qsort.cpp
  src/snprintf.cpp
  src/crt0.s
  src/CG_syscall.s
  src/LIB/minisdk.s
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

add_executable(CBASIC ${SOURCES} ${ASSETS} ${ASSETS_${FXSDK_PLATFORM}})
include_directories(CBASIC PRIVATE
                    "${CMAKE_CURRENT_SOURCE_DIR}/header"
                    "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_compile_options(CBASIC PRIVATE
                       -nostdlib -I include
                       -Wall -Wextra -Os -g -flto=auto
                       -Wno-narrowing)
target_link_options(CBASIC PRIVATE
                    -nostdlib -T "${CMAKE_CURRENT_SOURCE_DIR}/prizm.ld" -L ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(CBASIC PRIVATE -lm -lc -lfxcg -lgcc)

if("${FXSDK_PLATFORM_LONG}" STREQUAL fx9860G)
  generate_g1a(TARGET CBASIC OUTPUT "CBASIC.g1a"
    NAME "CBASIC" ICON assets-fx/icon.png)
elseif("${FXSDK_PLATFORM_LONG}" STREQUAL fxCG50)
  generate_g3a(TARGET CBASIC OUTPUT "CBASIC.g3a"
    NAME "CBASIC" ICONS assets-cg/icon-uns.png assets-cg/icon-sel.png)
elseif("${FXSDK_PLATFORM_LONG}" STREQUAL fx9860G_G3A)
  generate_g3a(TARGET CBASIC OUTPUT "CBASIC-fx.g3a"
    NAME "CBASIC-fx" ICONS assets-cg/icon-uns.png assets-cg/icon-sel.png)
elseif("${FXSDK_PLATFORM_LONG}" STREQUAL fxCP)
  generate_hh2_bin(TARGET CBASIC OUTPUT "CBASIC-hh2.bin")
endif()
