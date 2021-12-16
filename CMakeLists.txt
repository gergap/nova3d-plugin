project(nova3d_plugin C)
cmake_minimum_required(VERSION 3.0)
# Plugin version information
set(VERSION "1.0.0")
# process version info
string(REGEX MATCHALL "[0-9]+" VERSION_PARTS ${VERSION})
list(GET VERSION_PARTS 0 VERSION_MAJOR)
list(GET VERSION_PARTS 1 VERSION_MINOR)
list(GET VERSION_PARTS 2 VERSION_PATCH)
configure_file(config.h.in config.h)

if ("${CMAKE_C_COMPILER_ID}" MATCHES "^(GNU|Clang)$")
    # increase warning level
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wformat=2 -Wunknown-pragmas -Wstrict-aliasing")
endif()

# add current binarr/source dir to search path
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# create our simple C binary
add_executable(nova3d_plugin main.c map.c stringutils.c)

# enable CMake testing framework
include(CTest)
enable_testing()

add_test(
    NAME sample
    COMMAND $<TARGET_FILE:nova3d_plugin> test/input test/output/test.CWS
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

# install targets
install(FILES plugin.json LICENSE DESTINATION .)
install(TARGETS nova3d_plugin DESTINATION .)

# Create package using CPack
set(CPACK_PACKAGE_VENDOR "Gerhard G.")
set(CPACK_PACKAGE_VERSION_MAJOR ${VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${VERSION_PATCH})
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/LICENSE")

set(CPACK_OUTPUT_FILE_PREFIX "")
set(CPACK_GENERATOR "ZIP" CACHE STRING "Generators to support. semi-colon delimited list")
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)

include(CPack)
set(CPACK_PACKAGE_FILE_NAME ${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_SYSTEM_NAME})
MESSAGE(STATUS "CPACK_PACKAGE_FILE_NAME: ${CPACK_PACKAGE_FILE_NAME}")

# get cpack path from CMAKE_COMMAND variable
get_filename_component(CPACK_COMMAND ${CMAKE_COMMAND} PATH)
set(CPACK_COMMAND ${CPACK_COMMAND}/cpack)

add_custom_target(package_plugin
    COMMAND ${CPACK_COMMAND} -G ZIP
    COMMAND ${CMAKE_COMMAND} -E rename ${CPACK_PACKAGE_FILE_NAME}.zip ../${CPACK_PACKAGE_FILE_NAME}.CHplugin
    COMMAND ${CMAKE_COMMAND} -E echo "Successfully created ${CPACK_PACKAGE_FILE_NAME}.CHplugin"
    WORKING_DIRECTORY ${CMAKE_BUILD_DIR})
