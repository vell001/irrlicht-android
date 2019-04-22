cmake_minimum_required(VERSION 3.4.1)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Include)
file(GLOB src ${CMAKE_CURRENT_SOURCE_DIR}/Source/*.c)
set(TARGET_NAME libtess2)

add_library( # Sets the name of the library.
        ${TARGET_NAME}

        # Sets the library as a shared library.
        STATIC

        # Provides a relative path to your source file(s).
        ${src})

target_link_libraries( # Specifies the target library.
        ${TARGET_NAME}
        )

target_include_directories(${TARGET_NAME} INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/Include")