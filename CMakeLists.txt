cmake_minimum_required(VERSION 3.1)

project(LevelEditor)

include_directories(LevelEditor PUBLIC include)

#find_package(SFML COMPONENTS system window graphics REQUIRED)

add_subdirectory("extlibs/SFML-2.5.1/")

add_executable(LevelEditor
		src/imgui.cpp
		src/imgui_draw.cpp
		src/imgui_tables.cpp
		src/main.cpp
		src/imgui-SFML.cpp
		src/imgui_widgets.cpp)

#target_link_libraries(LevelEditor
#		${SFML_DIR}/lib/libsfml-system-s.a
#		${SFML_DIR}/lib/libsfml-window-s.a
#		${SFML_DIR}/lib/libsfml-graphics-s.a)
target_link_libraries(LevelEditor sfml-system sfml-window sfml-graphics)
