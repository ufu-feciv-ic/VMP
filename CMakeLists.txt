cmake_minimum_required(VERSION 3.20)
project(VMP CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Inclui a pasta include/ globalmente para que qualquer arquivo encontre os headers do motor
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Arquivos de implementação do motor de cálculo
set(MOTOR_SOURCES
    src/geometria.cpp
    src/rotinas_nbr8800.cpp
    src/rotinas_vmp.cpp
    src/verificador.cpp
)

# ============================================================================
# 1. EXECUTÁVEL DE CONSOLE / MEMÓRIA DE CÁLCULO
# ============================================================================
add_executable(vmp_demo
    src/main.cpp
    ${MOTOR_SOURCES}
)

# ============================================================================
# 2. SUÍTE DE TESTES UNITÁRIOS (TDD)
# ============================================================================
enable_testing()

add_executable(vmp_tests
    tests/test_main.cpp
    tests/test_rotinas_nbr8800.cpp
    tests/test_rotinas_vmp.cpp
    ${MOTOR_SOURCES}
)

target_include_directories(vmp_tests PRIVATE
    tests
)

add_test(NAME AllUnitTests COMMAND vmp_tests)

# ============================================================================
# 3. INTERFACE GRÁFICA INTERATIVA (RAYLIB + DEAR IMGUI)
# ============================================================================
if (EXISTS "C:/raylib/raylib/src/raylib.h" OR EXISTS "C:/raylib/w64devkit/include/raylib.h")
    set(RAYLIB_INCLUDE_DIR "C:/raylib/raylib/src")
    if (EXISTS "C:/raylib/w64devkit/lib/libraylib.a")
        set(RAYLIB_LIB "C:/raylib/w64devkit/lib/libraylib.a")
    else()
        set(RAYLIB_LIB "C:/raylib/raylib/src/libraylib.a")
    endif()

    add_executable(vmp_gui
        gui/app_gui.cpp
        ${MOTOR_SOURCES}
        gui/third_party/imgui/imgui.cpp
        gui/third_party/imgui/imgui_draw.cpp
        gui/third_party/imgui/imgui_tables.cpp
        gui/third_party/imgui/imgui_widgets.cpp
        gui/third_party/rlImGui/rlImGui.cpp
    )

    target_include_directories(vmp_gui PRIVATE
        ${RAYLIB_INCLUDE_DIR}
        gui/third_party/imgui
        gui/third_party/rlImGui
    )

    target_link_libraries(vmp_gui PRIVATE
        ${RAYLIB_LIB}
        opengl32
        gdi32
        winmm
    )
endif()
