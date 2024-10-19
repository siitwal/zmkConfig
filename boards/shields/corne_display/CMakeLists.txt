if(CONFIG_ZMK_DISPLAY)
	
	zephyr_library_include_directories(${CMAKE_SOURCE_DIR}/include)
	zephyr_library_sources(custom_status_screen.c)
	
	if(CONFIG_ZMK_SPLIT_ROLE_CENTRAL)
		zephyr_library_sources(widgets/label_layer_status.c)
		zephyr_library_sources(widgets/kb_status.c)
		zephyr_library_sources(widgets/bongo_cat.c)
		zephyr_library_sources(widgets/bongo_cat_images.c)
	else()
	endif()
	
endif()
