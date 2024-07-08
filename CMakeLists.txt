# Configure with [fxsdk build-fx] or [fxsdk build-cg], which provide the
# toolchain file and module path of the fxSDK

set(CBASIC_VERSION "02.47.2000")

cmake_minimum_required(VERSION 3.15)
project(CBFXGCC LANGUAGES ASM C VERSION ${CBASIC_VERSION})

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
  assets-fx/example.png
  # ...
)
set(ASSETS_cg
  assets-cg/example.png
  # ...
)

fxconv_declare_assets(${ASSETS} ${ASSETS_fx} ${ASSETS_cg} WITH_METADATA)

add_executable(CBFXGCC ${SOURCES} ${ASSETS} ${ASSETS_${FXSDK_PLATFORM}})
include_directories(CBFXGCC PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_compile_options(CBFXGCC PRIVATE
                        -mrenesas -nostdlib -I include
                        -Wall -Wextra -Os -g -flto
                        # -Wno-sign-compare
                        -Wno-unused-parameter
                        -Wno-unused-label
                        -Wno-unused-variable
                        -Wno-unused-but-set-variable
                        )
target_link_options(CBFXGCC PRIVATE
                        -nostdlib -T "${CMAKE_CURRENT_SOURCE_DIR}/addin.ld" -L ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(CBFXGCC PRIVATE -lfx -lgcc)


if("${FXSDK_PLATFORM_LONG}" STREQUAL fx9860G)
  generate_g1a(TARGET CBFXGCC VERSION ${CBASIC_VERSION} OUTPUT "CBFXGCC.g1a"
    NAME "CBFXGCC" ICON assets-fx/icon.png)
elseif("${FXSDK_PLATFORM_LONG}" STREQUAL fxCG50)
  generate_g3a(TARGET CBFXGCC VERSION ${CBASIC_VERSION} OUTPUT "CBFXGCC.g3a"
    NAME "CBFXGCC" ICONS assets-cg/icon-uns.png assets-cg/icon-sel.png)
elseif("${FXSDK_PLATFORM_LONG}" STREQUAL fx9860G_G3A)
  generate_g3a(TARGET CBFXGCC VERSION ${CBASIC_VERSION} OUTPUT "CBFXGCC-fx.g3a"
    NAME "CBFXGCC-fx" ICONS assets-cg/icon-uns.png assets-cg/icon-sel.png)
elseif("${FXSDK_PLATFORM_LONG}" STREQUAL fxCP)
  generate_hh2_bin(TARGET CBFXGCC VERSION ${CBASIC_VERSION} OUTPUT "CBFXGCC-hh2.bin")
endif()
