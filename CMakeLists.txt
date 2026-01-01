cmake_minimum_required(VERSION 3.20)

project("testSdlImguiLinux" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ENABLE_IMGUI_DEMO "Build ImGui demo window code" OFF)
option(COPY_ASSETS "Copy assets/ to output dir after build" ON)

set(APP_TARGET "testSdlImguiLinux")

set(IMGUI_SOURCES
    third_party/imgui/imgui.cpp
    third_party/imgui/imgui_draw.cpp
    third_party/imgui/imgui_tables.cpp
    third_party/imgui/imgui_widgets.cpp
)

if (ENABLE_IMGUI_DEMO)
    list(APPEND IMGUI_SOURCES third_party/imgui/imgui_demo.cpp)
    target_compile_definitions(${APP_TARGET} PRIVATE ENABLE_IMGUI_DEMO=1) # (note: set after add_executable)
endif()

add_executable(${APP_TARGET}
    #src/main.cpp
    src/withDocker.cpp
    src/simpleSDL.cpp
    ${IMGUI_SOURCES}
    third_party/imgui/backends/imgui_impl_sdl2.cpp
    third_party/imgui/backends/imgui_impl_sdlrenderer2.cpp
)

# If demo enabled, define macro on target (safe place, after target exists)
if (ENABLE_IMGUI_DEMO)
    target_compile_definitions(${APP_TARGET} PRIVATE ENABLE_IMGUI_DEMO=1)
endif()

target_include_directories(${APP_TARGET} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui/backends
)

if (WIN32)
    set(SDL2_INCLUDE_DIR "D:/Code/SDL_Dev/SDL2-2.30.0/include")
    set(SDL2_LIB_DIR     "D:/Code/SDL_Dev/SDL2-2.30.0/lib/x64")

    set(SDL2IMG_INCLUDE_DIR "D:/Code/SDL_Dev/SDL2_image-2.8.2/include")
    set(SDL2IMG_LIB_DIR     "D:/Code/SDL_Dev/SDL2_image-2.8.2/lib/x64")

    target_include_directories(${APP_TARGET} PRIVATE
        "${SDL2_INCLUDE_DIR}"
        "${SDL2IMG_INCLUDE_DIR}"
    )

    target_link_directories(${APP_TARGET} PRIVATE
        "${SDL2_LIB_DIR}"
        "${SDL2IMG_LIB_DIR}"
    )

    target_link_libraries(${APP_TARGET} PRIVATE
        SDL2main
        SDL2
        SDL2_image
        winmm imm32 version setupapi
    )

    # ---- Slim & fast Release for MSVC ----
    if (MSVC)
        target_compile_options(${APP_TARGET} PRIVATE
            $<$<CONFIG:Release>:/O2 /Ob2 /DNDEBUG /Gy /Zc:inline>
        )
        target_link_options(${APP_TARGET} PRIVATE
            $<$<CONFIG:Release>:/OPT:REF /OPT:ICF>
        )
        set_property(TARGET ${APP_TARGET} PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
    endif()

    # ---- Copy DLLs next to exe ----
    set(_SDL2_DLL "")
    set(_SDL2IMG_DLL "")

    if (EXISTS "${SDL2_LIB_DIR}/SDL2.dll")
        set(_SDL2_DLL "${SDL2_LIB_DIR}/SDL2.dll")
    elseif (EXISTS "D:/Code/SDL_Dev/SDL2-2.30.0/bin/SDL2.dll")
        set(_SDL2_DLL "D:/Code/SDL_Dev/SDL2-2.30.0/bin/SDL2.dll")
    endif()

    if (EXISTS "${SDL2IMG_LIB_DIR}/SDL2_image.dll")
        set(_SDL2IMG_DLL "${SDL2IMG_LIB_DIR}/SDL2_image.dll")
    elseif (EXISTS "D:/Code/SDL_Dev/SDL2_image-2.8.2/bin/SDL2_image.dll")
        set(_SDL2IMG_DLL "D:/Code/SDL_Dev/SDL2_image-2.8.2/bin/SDL2_image.dll")
    endif()

    if (NOT _SDL2_DLL STREQUAL "")
        add_custom_command(TARGET ${APP_TARGET} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${_SDL2_DLL}"
                "$<TARGET_FILE_DIR:${APP_TARGET}>/SDL2.dll"
        )
    else()
        message(WARNING "SDL2.dll not found. Put it near lib\\x64 or bin, or edit CMakeLists.")
    endif()

    if (NOT _SDL2IMG_DLL STREQUAL "")
        add_custom_command(TARGET ${APP_TARGET} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${_SDL2IMG_DLL}"
                "$<TARGET_FILE_DIR:${APP_TARGET}>/SDL2_image.dll"
        )
    else()
        message(WARNING "SDL2_image.dll not found. Put it near lib\\x64 or bin, or edit CMakeLists.")
    endif()

else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL2 REQUIRED sdl2)
    pkg_check_modules(SDL2_IMAGE REQUIRED SDL2_image)

    target_include_directories(${APP_TARGET} PRIVATE
        ${SDL2_INCLUDE_DIRS}
        ${SDL2_IMAGE_INCLUDE_DIRS}
    )

    target_link_libraries(${APP_TARGET} PRIVATE
        ${SDL2_LIBRARIES}
        ${SDL2_IMAGE_LIBRARIES}
    )
endif()

# ---- Copy assets/ next to exe ----
if(COPY_ASSETS)
    set(ASSETS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/assets")

    if(EXISTS "${ASSETS_DIR}" AND IS_DIRECTORY "${ASSETS_DIR}")
        add_custom_command(TARGET ${APP_TARGET} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${ASSETS_DIR}"
                "$<TARGET_FILE_DIR:${APP_TARGET}>/assets"
            COMMENT "Copying assets -> $<TARGET_FILE_DIR:${APP_TARGET}>/assets"
        )
    else()
        message(WARNING "COPY_ASSETS=ON but assets directory not found: ${ASSETS_DIR}")
    endif()
endif()