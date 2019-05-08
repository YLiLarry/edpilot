cmake_minimum_required(VERSION 3.8)
project(edpilot VERSION 1.0)

add_subdirectory(deps/ggframe)

option(build_edpilot_example "build examples?" ON)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

file(GLOB SOURCES src/*.cc)
file(GLOB HEADERS src/*.h)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

install (TARGETS ${PROJECT_NAME}  
	RUNTIME DESTINATION bin
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib
)

install(FILES ${HEADERS} 
	DESTINATION 
	include/${PROJECT_NAME}
)

target_include_directories(
    ${PROJECT_NAME} 
	PUBLIC
		include
)
target_link_libraries(
    ${PROJECT_NAME} 
	PUBLIC
		ggframe
)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

if(WIN32)
endif(WIN32)

if(APPLE)
	target_compile_definitions(
		${PROJECT_NAME} 
		PRIVATE
			APPLE=1
	)
endif(APPLE)

if(build_edpilot_example)
	add_subdirectory(example)
endif(build_edpilot_example)
