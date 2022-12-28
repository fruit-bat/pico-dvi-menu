
set(pico_menu_common
	${CMAKE_CURRENT_LIST_DIR}/src/PicoRectXYWH.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/PicoRectXYXY.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/PicoCharScreen.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/PicoClippedScreen.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/PicoWinHidKeyboard.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/PicoPen.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/PicoWin.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/PicoDisplay.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/PicoWinBlock.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/PicoSelect.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/PicoOption.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/PicoOptionText.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/PicoQuickKey.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/PicoQuickKeyAscii.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/PicoWiz.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/PicoWizUtils.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/PicoWizExplorer.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/PicoTextField.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/PicoExplorer.cpp
)

add_library(pico_dvi_menu INTERFACE)

target_sources(pico_dvi_menu INTERFACE
	${pico_menu_common}
	${CMAKE_CURRENT_LIST_DIR}/src/PicoCharRenderer.cpp
)

target_include_directories(pico_dvi_menu INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/src
)

add_library(pico_vga_menu INTERFACE)

target_sources(pico_vga_menu INTERFACE
	${pico_menu_common}
	${CMAKE_CURRENT_LIST_DIR}/src/PicoCharRendererVga.cpp
)

target_include_directories(pico_vga_menu INTERFACE
    src
)

add_library(pico_scanvideo_menu INTERFACE)

target_sources(pico_scanvideo_menu INTERFACE
	${pico_menu_common}
	${CMAKE_CURRENT_LIST_DIR}/src/PicoCharRendererScanvideo.cpp
)

target_include_directories(pico_scanvideo_menu INTERFACE
    src
)

add_library(pico_st7789_menu INTERFACE)

target_sources(pico_st7789_menu INTERFACE
	${pico_menu_common}
	${CMAKE_CURRENT_LIST_DIR}/src/PicoCharRendererSt7789.cpp
)

target_include_directories(pico_st7789_menu INTERFACE
    src
)
