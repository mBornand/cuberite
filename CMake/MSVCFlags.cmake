if(MSVC)
	# Make build use multiple threads under MSVC:
	add_compile_options("/MP")

	# Make build use Unicode:
	add_compile_definitions(UNICODE _UNICODE)
endif()
