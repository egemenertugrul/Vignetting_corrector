cmake_minimum_required(VERSION 3.13)

project(VignettingCorrector VERSION 0.1
                  DESCRIPTION "Vignetting correction"
                LANGUAGES CXX)

###

# add src subdirectory
# add_subdirectory("${Project_SOURCE_DIR}/src" )

# set (CMAKE_CXX_FLAGS "-lstdc++fs -std=c++17")

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
#set(OPENMP_LIBRARIES "/usr/local/Cellar/llvm/8.0.0/lib")
#set(OPENMP_INCLUDES "/usr/local/Cellar/llvm/8.0.0/include")

set(CMAKE_C_COMPILER "/usr/bin/clang")
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
set(OPENMP_LIBRARIES "/usr/lib/")
set(OPENMP_INCLUDES "/usr/include")

OPTION (USE_OpenMP "Use OpenMP to enamble <omp.h>" ON)

if(USE_OpenMP)
  find_package(OpenMP REQUIRED)
endif(USE_OpenMP)

#if (OPENMP_FOUND)
    include_directories("${OPENMP_INCLUDES}")
    link_directories("${OPENMP_LIBRARIES}")
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    # set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
#endif(OPENMP_FOUND)


include_directories(/usr/lib/llvm-6.0/include)
link_directories(/usr/lib/llvm-6.0/lib)



# set sources
set(SRCS ${PROJECT_SOURCE_DIR}/src/matrix.cpp
	${PROJECT_SOURCE_DIR}/src/vignettingCorrectionUsingRG.cpp
	${PROJECT_SOURCE_DIR}/src/vignettingCorrection.cpp
	)

set(HDRS ${PROJECT_SOURCE_DIR}/include/vignettingCorrectionUsingRG.hpp
	${PROJECT_SOURCE_DIR}/include/vignettingCorrection.hpp
	${PROJECT_SOURCE_DIR}/include/matrix.h)

find_package(OpenCV REQUIRED)

# create libraries
add_library(vigCorrectorLibs STATIC ${SRCS} ${HDRS})

target_include_directories(vigCorrectorLibs PUBLIC include)
target_link_libraries(vigCorrectorLibs PUBLIC ${OpenCV_LIBS})
#target_compile_features(vigCorrectorLibs PUBLIC cxx_std_2a)

# set compiler option -fms-extensions for target (needed for _declspec(dllimport))
target_compile_options(vigCorrectorLibs PUBLIC -fopenmp)

# create executable
add_executable(vignettingCorrection ${PROJECT_SOURCE_DIR}/app/main.cpp)

# find openCV
#set(OpenCV_DIR "/usr/local/Cellar/opencv@3/3.4.5/share/OpenCV")

# PATHS ${OpenCV_DIR})


# ${OpenCV_LIBS}
#message(STATUS "This is the Path to OpenCV ${OpenCV_DIR}")

# link openCV libraries
target_link_libraries(vignettingCorrection PUBLIC vigCorrectorLibs  ${OpenCV_LIBS})
#target_compile_features(vignettingCorrection PUBLIC cxx_std_2a)

target_link_libraries(vignettingCorrection PUBLIC stdc++fs)
