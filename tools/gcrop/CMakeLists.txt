cmake_minimum_required(VERSION 3.8)
project(gcrop VERSION 1.0)

add_executable(${PROJECT_NAME} src/gcrop.cc)

install (TARGETS ${PROJECT_NAME}  
	RUNTIME DESTINATION bin
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib
)

target_include_directories(
    ${PROJECT_NAME} 
    PUBLIC 
		src
)

target_link_libraries(
    ${PROJECT_NAME} 
	PUBLIC
		edpilot
        ggframe
)

if(WIN32)
	target_compile_definitions(
		${PROJECT_NAME} 
		PRIVATE
			WIN32=1
	)
endif(WIN32)

if(APPLE)
	target_compile_definitions(
		${PROJECT_NAME} 
		PRIVATE
			APPLE=1
	)
endif(APPLE)
