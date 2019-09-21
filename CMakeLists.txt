
cmake_minimum_required(VERSION 2.8)

project(timebench)

set(CMAKE_CXX_STANDARD 17)

if (CMAKE_BUILD_TYPE EQUAL "DEBUG")
  SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -D_LIBCPP_DEBUG -g")
else()
  SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -DBOOST_EXCEPTION_DISABLE")
endif (CMAKE_BUILD_TYPE EQUAL "DEBUG")

set(TB_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(TB_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)
file(GLOB_RECURSE TB_LIB_LIST ${TB_LIB_DIR}/*.cpp)
set(TB_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(TB_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test)

file(GLOB_RECURSE TB_FULL_LIST ${TB_LIB_DIR}/*.cpp ${TB_INCLUDE_DIR}/*.h ${TB_SRC_DIR}/*.cpp)

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(TB_OS_LIBS pthread)
elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    set(TB_OS_LIBS)
endif()

set(Boost_USE_STATIC_LIBS ON) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost 1.45.0 COMPONENTS program_options) 

if(Boost_FOUND)
  INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
else()
  message(FATAL_ERROR "Boost not found")
endif()

add_executable(timebench ${TB_FULL_LIST})
set_target_properties(timebench PROPERTIES
    INCLUDE_DIRECTORIES "${TB_INCLUDE_DIR}"
    INCLUDE_DIRECTORIES "${Boost_INCLUDE_DIR}"
    LINKER_LANGUAGE CXX
)
target_link_libraries(timebench "${TB_OS_LIBS}" ${Boost_LIBRARIES} )

set(PROJECT_TEST_NAME ${PROJECT_NAME}_test)
file(GLOB_RECURSE TB_TEST_LIST ${TB_TEST_DIR}/*.cpp)

enable_testing()

# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
                 ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)

# The gtest/gtest_main targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
  include_directories("${gtest_SOURCE_DIR}/include")
endif()


include_directories("${TB_INCLUDE_DIR}")

# Now simply link against gtest or gtest_main as needed. Eg
add_executable(${PROJECT_TEST_NAME} ${TB_TEST_LIST} ${TB_LIB_LIST})
target_link_libraries(${PROJECT_TEST_NAME} gtest_main ${Boost_LIBRARIES})
add_test(NAME test COMMAND ${PROJECT_TEST_NAME})