if(CONFIG_ZMK_DISPLAY)
	
	zephyr_library_include_directories(${CMAKE_SOURCE_DIR}/include)
	zephyr_library_sources(custom_status_screen.c)
	
	if(CONFIG_ZMK_SPLIT_ROLE_CENTRAL)
		zephyr_library_sources(central_widgets/label_layer_status.c)
		zephyr_library_sources(central_widgets/kb_status.c)
		zephyr_library_sources(central_widgets/bongo_cat.c)
		zephyr_library_sources(central_widgets/bongo_cat_images.c)
	else()
		zephyr_library_sources(subordinate_widgets/image.c)
		zephyr_library_sources(subordinate_widgets/data_image.c)
		zephyr_library_sources(subordinate_widgets/battery.c)
	endif()
	
endif()
