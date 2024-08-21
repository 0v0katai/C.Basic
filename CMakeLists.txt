# Configure with [fxsdk build-fx] or [fxsdk build-cg], which provide the
# toolchain file and module path of the fxSDK

cmake_minimum_required(VERSION 3.15)
project(CBASICPS LANGUAGES ASM C)

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
src/snprintf.c
src/crt0.s
src/syscall.s
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

add_executable(CBASICPS ${SOURCES} ${ASSETS} ${ASSETS_${FXSDK_PLATFORM}})
include_directories(CBASICPS PRIVATE
                    "${CMAKE_CURRENT_SOURCE_DIR}/include"
                    "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_compile_options(CBASICPS PRIVATE
                       -nostdlib -I include
                       -Wall -Wextra -Os -g -flto=auto
                       -Wno-narrowing)
target_link_options(CBASICPS PRIVATE
                    -nostdlib -T "${CMAKE_CURRENT_SOURCE_DIR}/prizm.ld" -L ${CMAKE_CURRENT_SOURCE_DIR}
                    -Wl,--print-memory-usage -Wl,-Map=map)
target_link_libraries(CBASICPS PRIVATE -lm #[[-lc2]] -lc -lfxcg -lgcc)

if("${FXSDK_PLATFORM_LONG}" STREQUAL fx9860G)
  generate_g1a(TARGET CBASICPS OUTPUT "CBASICPS.g1a"
    NAME "CBASICPS" ICON assets-fx/icon.png)
elseif("${FXSDK_PLATFORM_LONG}" STREQUAL fxCG50)
  generate_g3a(TARGET CBASICPS OUTPUT "CBASICPS.g3a"
    NAME "CBASICPS" ICONS assets-cg/icon-uns.png assets-cg/icon-sel.png)
elseif("${FXSDK_PLATFORM_LONG}" STREQUAL fx9860G_G3A)
  generate_g3a(TARGET CBASICPS OUTPUT "CBASICPS-fx.g3a"
    NAME "CBASICPS-fx" ICONS assets-cg/icon-uns.png assets-cg/icon-sel.png)
elseif("${FXSDK_PLATFORM_LONG}" STREQUAL fxCP)
  generate_hh2_bin(TARGET CBASICPS OUTPUT "CBASICPS-hh2.bin")
endif()
