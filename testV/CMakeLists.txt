cmake_minimum_required(VERSION 3.13)

project(testV VERSION 0.1
                  DESCRIPTION "testV"
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


include_directories("${OPENMP_INCLUDES}")
link_directories("${OPENMP_LIBRARIES}")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")


include_directories(/usr/local/opt/llvm/include /Library/Developer/CommandLineTools/SDKs/MacOSX10.14.sdk/usr/include)
link_directories(/usr/local/opt/llvm/lib)


find_package(OpenCV REQUIRED)



# create executable
add_executable(testV ${PROJECT_SOURCE_DIR}/testV.cpp)

# link openCV libraries
target_link_libraries(testV PUBLIC ${OpenCV_LIBS})
#target_compile_features(vignettingCorrection PUBLIC cxx_std_2a)
