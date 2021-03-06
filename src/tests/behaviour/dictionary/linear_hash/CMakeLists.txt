cmake_minimum_required(VERSION 3.5)
project(test_behaviour_linear_hash)

set(SOURCE_FILES
		test_behaviour_linear_hash.c
		test_behaviour_linear_hash.h
)

if(USE_ARDUINO)
	set(${PROJECT_NAME}_BOARD       ${BOARD})
	set(${PROJECT_NAME}_PROCESSOR   ${PROCESSOR})
	set(${PROJECT_NAME}_MANUAL      ${MANUAL})
	set(${PROJECT_NAME}_PORT        ${PORT})
	set(${PROJECT_NAME}_SERIAL      ${SERIAL})

	set(${PROJECT_NAME}_SKETCH behaviour_linear_hash.ino)
	set(${PROJECT_NAME}_SRCS        ${SOURCE_FILES})
	set(${PROJECT_NAME}_LIBS        behaviour_dictionary)

	generate_arduino_firmware(${PROJECT_NAME})
else()
	add_executable(${PROJECT_NAME}          ${SOURCE_FILES} run_behaviour_linear_hash.c)

	target_link_libraries(${PROJECT_NAME}   behaviour_dictionary)

	# Use cmake -DCOVERAGE_TESTING=ON to include coverage testing information.
	if (CMAKE_COMPILER_IS_GNUCC AND COVERAGE_TESTING)
		set(GCC_COVERAGE_COMPILE_FLAGS "-g -O0 -fprofile-arcs -ftest-coverage")
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")
		set(CMAKE_C_OUTPUT_EXTENSION_REPLACE 1)
	endif()
endif()

