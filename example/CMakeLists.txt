cmake_minimum_required(VERSION 3.8)
project(edpilot_example)

add_executable(${PROJECT_NAME} main.cc)

target_link_directories(
    ${PROJECT_NAME}
    PUBLIC
)
target_include_directories(
    ${PROJECT_NAME} 
    PUBLIC 
		../src/
)
target_link_libraries(
    ${PROJECT_NAME} 
	PUBLIC
		edpilot
)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_11)

if(WIN32)
	target_link_libraries(
		${PROJECT_NAME} 
	)
endif(WIN32)

if(APPLE)
endif(APPLE)

