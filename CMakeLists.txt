# ============================================================================
# CMake 配置文件 - MyMinecraft
# ============================================================================

# 指定 CMake 最低版本要求
cmake_minimum_required(VERSION 3.16)

# 项目名称、版本和使用的语言
project(MyMinecraft
    VERSION 1.0.0
    LANGUAGES CXX C
    DESCRIPTION "A Minecraft clone built with C++ and OpenGL"
)

# ============================================================================
# 编译器设置
# ============================================================================

# 设置 C++ 标准为 C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 设置 C 标准（用于 GLAD）
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# 设置默认构建类型为 Release（如果未指定）
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# 根据构建类型设置编译选项
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Building in Debug mode")
    add_compile_definitions(DEBUG)
else()
    message(STATUS "Building in Release mode")
endif()

# ============================================================================
# 查找依赖库
# ============================================================================

# OpenGL - 必需
find_package(OpenGL REQUIRED)
if(OpenGL_FOUND)
    message(STATUS "Found OpenGL: ${OPENGL_LIBRARIES}")
endif()

# GLFW3 - 窗口和输入管理
find_package(glfw3 REQUIRED)
if(glfw3_FOUND)
    message(STATUS "Found GLFW3")
endif()

# GLM - 数学库（header-only）
find_package(glm REQUIRED)
if(glm_FOUND)
    message(STATUS "Found GLM")
endif()

# FreeType - 字体渲染
find_package(Freetype REQUIRED)
if(Freetype_FOUND)
    message(STATUS "Found FreeType: ${FREETYPE_INCLUDE_DIRS}")
endif()

# ============================================================================
# 源文件
# ============================================================================

# 主程序源文件
set(MAIN_SOURCES
    main.cpp
)

# 核心模块源文件
set(CORE_SOURCES
    src/core/game.cpp
    src/core/camera.cpp
)

# 世界/地形模块源文件
set(WORLD_SOURCES
    src/world/chunk.cpp
    src/world/terrain.cpp
    src/world/block.cpp
)

# 实体模块源文件
set(ENTITY_SOURCES
    src/entity/player.cpp
    src/entity/collision.cpp
)

# 渲染模块源文件
set(RENDER_SOURCES
    src/render/texture.cpp
)

# UI模块源文件
set(UI_SOURCES
    src/ui/HUDpainter.cpp
    src/ui/toolBar.cpp
    src/ui/itemSelection.cpp
)

# GLAD 库源文件（OpenGL 加载器）
set(GLAD_SOURCES
    lib/glad/glad.c
)

# 第三方库源文件
set(THIRDPARTY_SOURCES
    src/utils/stb_image.cpp
)

# 汇总所有源文件
set(ALL_SOURCES
    ${MAIN_SOURCES}
    ${CORE_SOURCES}
    ${WORLD_SOURCES}
    ${ENTITY_SOURCES}
    ${RENDER_SOURCES}
    ${UI_SOURCES}
    ${GLAD_SOURCES}
    ${THIRDPARTY_SOURCES}
)

# ============================================================================
# 创建可执行文件
# ============================================================================

add_executable(${PROJECT_NAME} ${ALL_SOURCES})

# ============================================================================
# 包含目录
# ============================================================================

target_include_directories(${PROJECT_NAME} PRIVATE
    # 项目内的 include 目录（GLAD, KHR 头文件）
    ${CMAKE_SOURCE_DIR}/include

    # src 目录（方便头文件相互引用）
    ${CMAKE_SOURCE_DIR}/src

    # FreeType 头文件目录
    ${FREETYPE_INCLUDE_DIRS}
)

# ============================================================================
# 链接库
# ============================================================================

target_link_libraries(${PROJECT_NAME} PRIVATE
    # OpenGL 库
    OpenGL::GL

    # GLFW 窗口库
    glfw

    # GLM 数学库（header-only，但仍需链接以获取编译定义）
    glm::glm

    # FreeType 字体库
    ${FREETYPE_LIBRARIES}

    # dl 库（用于动态加载，Linux 需要）
    ${CMAKE_DL_LIBS}
)

# ============================================================================
# 资源文件处理
# ============================================================================

# 定义资源目录
set(RESOURCE_DIRS
    shaders
    Textures
)

# 复制资源文件到构建目录
foreach(RESOURCE_DIR ${RESOURCE_DIRS})
    # 检查资源目录是否存在
    if(EXISTS ${CMAKE_SOURCE_DIR}/${RESOURCE_DIR})
        # 创建自定义命令，在构建后复制资源
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_SOURCE_DIR}/${RESOURCE_DIR}
                $<TARGET_FILE_DIR:${PROJECT_NAME}>/${RESOURCE_DIR}
            COMMENT "Copying ${RESOURCE_DIR} to build directory"
        )
        message(STATUS "Will copy ${RESOURCE_DIR} to build directory")
    else()
        message(WARNING "Resource directory not found: ${RESOURCE_DIR}")
    endif()
endforeach()

# ============================================================================
# 编译选项（针对不同编译器）
# ============================================================================

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    # GCC/Clang 编译选项
    # 注意：不使用 -Wpedantic，因为第三方库（GLAD、stb_image）会产生大量警告
    target_compile_options(${PROJECT_NAME} PRIVATE
        -Wall                   # 开启常见警告
        -Wextra                 # 开启额外警告
        -Wno-unused-function    # 忽略未使用函数警告（stb_image.h）
        -Wno-unused-variable    # 忽略未使用变量警告（stb_image.h）
        $<$<CONFIG:Debug>:-g>   # Debug 模式下生成调试信息
        $<$<CONFIG:Release>:-O3> # Release 模式下优化
    )
elseif(MSVC)
    # MSVC 编译选项
    target_compile_options(${PROJECT_NAME} PRIVATE
        /W3                     # 警告级别 3（避免第三方库警告）
        $<$<CONFIG:Release>:/O2> # Release 模式下优化
    )
    # MSVC 需要定义一些宏
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        _CRT_SECURE_NO_WARNINGS
        NOMINMAX
    )
endif()

# ============================================================================
# 输出信息
# ============================================================================

message(STATUS "")
message(STATUS "========================================")
message(STATUS "Project: ${PROJECT_NAME} v${PROJECT_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "========================================")
message(STATUS "")

# ============================================================================
# 安装规则（可选）
# ============================================================================

# 安装可执行文件
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

# 安装资源文件
install(DIRECTORY shaders Textures
    DESTINATION bin
)
