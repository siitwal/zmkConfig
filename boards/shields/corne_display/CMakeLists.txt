if(CONFIG_ZMK_DISPLAY)
	
	zephyr_library_include_directories(${CMAKE_SOURCE_DIR}/include)
	zephyr_library_sources(custom_status_screen.c)
	zephyr_library_sources(widgets/layer_status.c)
	
endif()
