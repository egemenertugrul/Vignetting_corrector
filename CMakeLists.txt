cmake_minimum_required(VERSION 3.13)

project(VignettingCorrector VERSION 0.1
                  DESCRIPTION "Vignetting correction"
                LANGUAGES CXX)

###

# add src subdirectory
# add_subdirectory("${Project_SOURCE_DIR}/src" )


set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
#set(OPENMP_LIBRARIES "/usr/local/Cellar/llvm/8.0.0/lib")
#set(OPENMP_INCLUDES "/usr/local/Cellar/llvm/8.0.0/include")

set(CMAKE_C_COMPILER "/usr/local/Cellar/llvm/8.0.0/bin/clang")
set(CMAKE_CXX_COMPILER "/usr/local/Cellar/llvm/8.0.0/bin/clang++")
set(OPENMP_LIBRARIES "/usr/local/Cellar/llvm/8.0.0/lib")
set(OPENMP_INCLUDES "/usr/local/Cellar/llvm/8.0.0/include")

OPTION (USE_OpenMP "Use OpenMP to enamble <omp.h>" ON)

# Find OpenMP
if(APPLE AND USE_OpenMP)
    if(CMAKE_C_COMPILER_ID MATCHES "Clang")
        set(OpenMP_C "${CMAKE_C_COMPILER}")
        set(OpenMP_C_FLAGS "-fopenmp=libomp -Wno-unused-command-line-argument")
        set(OpenMP_C_LIB_NAMES "libomp" "libgomp" "libiomp5")
        set(OpenMP_libomp_LIBRARY ${OpenMP_C_LIB_NAMES})
        set(OpenMP_libgomp_LIBRARY ${OpenMP_C_LIB_NAMES})
        set(OpenMP_libiomp5_LIBRARY ${OpenMP_C_LIB_NAMES})
    endif()
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
      set(OpenMP_CXX "${CMAKE_CXX_COMPILER}")
      set(OpenMP_CXX_FLAGS "-lc++fs -std=c++2a -fopenmp -Wno-unused-command-line-argument")
#      set(OpenMP_CXX_LIB_NAMES "libomp" "libgomp" "libiomp5")
      set(OpenMP_libomp_LIBRARY ${OpenMP_CXX_LIB_NAMES})
      set(OpenMP_libgomp_LIBRARY ${OpenMP_CXX_LIB_NAMES})
      set(OpenMP_libiomp5_LIBRARY ${OpenMP_CXX_LIB_NAMES})
    endif()
endif()

# if(USE_OpenMP)
#   find_package(OpenMP REQUIRED)
# endif(USE_OpenMP)

#if (OPENMP_FOUND)
    include_directories("${OPENMP_INCLUDES}")
    link_directories("${OPENMP_LIBRARIES}")
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    # set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
#endif(OPENMP_FOUND)


include_directories(/usr/local/opt/llvm/include /Library/Developer/CommandLineTools/SDKs/MacOSX10.14.sdk/usr/include)
link_directories(/usr/local/opt/llvm/lib)



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
