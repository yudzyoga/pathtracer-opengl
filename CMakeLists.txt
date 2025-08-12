cmake_minimum_required(VERSION 3.10)
project(OpenGLPT)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

# Add GLFW from ext/
add_subdirectory(ext/glfw)


# Create executable from your source files
add_executable(${PROJECT_NAME}
    main.cpp
)

# Link GLFW to your executable
target_link_libraries(${PROJECT_NAME}
    glfw
    GLEW::GLEW
    OpenGL::GL
)

# # For macOS, link Cocoa and OpenGL frameworks automatically
# if(APPLE)
#     target_link_libraries(${PROJECT_NAME} "-framework Cocoa" "-framework OpenGL" "-framework IOKit")
# endif()

# On Linux, you’ll also need to link OpenGL
if(UNIX AND NOT APPLE)
    set(OpenGL_GL_PREFERENCE GLVND)
    find_package(OpenGL REQUIRED)
    target_link_libraries(${PROJECT_NAME} OpenGL::GL)
endif()
