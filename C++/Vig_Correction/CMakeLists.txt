cmake_minimum_required(VERSION 3.13)

project(VignettingCorrector VERSION 0.1
                  DESCRIPTION "Vignetting correction"
                LANGUAGES CXX)

###

# add src subdirectory
# add_subdirectory("${Project_SOURCE_DIR}/src" )

# set sources
set(SRCS ${PROJECT_SOURCE_DIR}/src/ColorCorrection.cpp 
	${PROJECT_SOURCE_DIR}/src/commonfile.cpp 
	${PROJECT_SOURCE_DIR}/src/Matrix.cpp)

set(HDRS ${PROJECT_SOURCE_DIR}/include/ColorCorrection.hpp 
	${PROJECT_SOURCE_DIR}/include/commondata.h 
	${PROJECT_SOURCE_DIR}/include/commonfile.h 
	${PROJECT_SOURCE_DIR}/include/Matrix.h)

# create libraries
add_library(vigCorrectorLibs STATIC ${SRCS} ${HDRS})

target_include_directories(vigCorrectorLibs PUBLIC include)
target_compile_features(vigCorrectorLibs PUBLIC cxx_std_11)

# set compiler option -fms-extensions for target (needed for _declspec(dllimport))
target_compile_options(vigCorrectorLibs PUBLIC -fms-extensions)

# create executable
add_executable(Vignetting_Corrector ${PROJECT_SOURCE_DIR}/app/Vig_Correction.cpp)

# find openCV
set(OpenCV_DIR "/usr/local/Cellar/opencv@3/3.4.5/share/OpenCV")

find_package(OpenCV REQUIRED PATHS ${OpenCV_DIR})


# ${OpenCV_LIBS}
message(STATUS "This is the Path to OpenCV ${OpenCV_DIR}")

# link openCV libraries
target_link_libraries(Vignetting_Corrector PUBLIC vigCorrectorLibs  ${OpenCV_LIBS})


