project(nova3d_plugin C)
cmake_minimum_required(VERSION 3.0)

if ("${CMAKE_C_COMPILER_ID}" MATCHES "^(GNU|Clang)$")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wformat=2 -Wunknown-pragmas -Wstrict-aliasing")
endif()

add_executable(nova3d_plugin main.c map.c)

include(CTest)
enable_testing()

add_test(
    NAME sample
    COMMAND $<TARGET_FILE:nova3d_plugin> test/input test/output/test.CWS
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

install(FILES plugin.json DESTINATION .)
install(TARGETS nova3d_plugin DESTINATION .)

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
