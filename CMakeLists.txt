cmake_minimum_required(VERSION 3.31)
project(DepthMapConverter)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# GLFW3
FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.4
        GIT_SHALLOW TRUE
)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

# GLEW
FetchContent_Declare(
        glew
        URL https://github.com/nigels-com/glew/releases/download/glew-2.2.0/glew-2.2.0.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(glew)

    set(GLEW_DIR ${glew_SOURCE_DIR})
    set(GLEW_INCLUDE_DIR ${GLEW_DIR}/include)

    set(GLEW_SOURCE ${GLEW_DIR}/src/glew.c)

    add_library(glew_static STATIC ${GLEW_SOURCE})

    target_include_directories(glew_static PUBLIC
            $<BUILD_INTERFACE:${GLEW_INCLUDE_DIR}>
            $<INSTALL_INTERFACE:include>
    )

    target_compile_definitions(glew_static PUBLIC GLEW_STATIC)

    if(WIN32)
        target_link_libraries(glew_static PUBLIC opengl32)
    else()
        find_package(OpenGL REQUIRED)
        target_link_libraries(glew_static PUBLIC OpenGL::GL)
    endif()


# GLM
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 0.9.9.8
        GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(glm)

set(SOURCES
        src/main.cpp
        src/DepthMap.cpp
        src/Mesh.cpp
        src/VRMLExporter.cpp
        src/Renderer.cpp
        src/ShaderLoader.cpp
)

set(HEADERS
        include/DepthMap.h
        include/Mesh.h
        include/VRMLExporter.h
        include/Renderer.h
        include/ShaderLoader.h
        include/RenderConfig.h
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME} PRIVATE
        glfw
        glew_static
        glm::glm
)

if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE
            gdi32
            user32
            kernel32
    )

    if(MSVC)
        set_target_properties(${PROJECT_NAME} PROPERTIES
                WIN32_EXECUTABLE $<CONFIG:Release>
        )
    endif()
endif()

if(UNIX)
    find_package(X11 REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE
            ${X11_LIBRARIES}
            ${CMAKE_DL_LIBS}
            pthread
    )
endif()

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/shaders
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders
)