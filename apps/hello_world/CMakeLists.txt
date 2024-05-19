
# include_directories(${CMAKE_CURRENT_LIST_DIR})

add_executable(hello_world
	main.cpp
)

target_include_directories(hello_world PRIVATE
	${CMAKE_CURRENT_LIST_DIR}
)

target_compile_definitions(hello_world PRIVATE
  DVI_DEFAULT_SERIAL_CONFIG=${DVI_DEFAULT_SERIAL_CONFIG}
  PICO_CORE1_STACK_SIZE=0x400
  DVI_VERTICAL_REPEAT=2
  DVI_N_TMDS_BUFFERS=3
  DVI_1BPP_BIT_REVERSE=1

  # Set the resolution of the menu screen
  PCS_COLS=80
  PCS_ROWS=30
  # Set the DVI timing
  DVI_TIMING=dvi_timing_640x480p_60hz  
)

target_link_libraries(hello_world
	${menu_common_libs}
)

# create map/bin/hex file etc.
pico_add_extra_outputs(hello_world)
