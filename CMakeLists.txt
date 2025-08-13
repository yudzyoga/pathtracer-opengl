cmake_minimum_required(VERSION 3.10)
project(OpenGLPT)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

# Add GLFW from ext/
add_subdirectory(ext/glfw)
add_subdirectory(ext/glm)


# Create executable from your source files
# add_executable(${PROJECT_NAME}
#     main.cpp
# )

file(GLOB SRC_FILES
    "*.cpp"
    "*.h"
)

add_library(stb_image 
    STATIC
    ext/stb_image/stb_image.cpp
)

target_include_directories(stb_image
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/ext/stb_image
)

# target_include_directories(${PROJECT_NAME}
#     PRIVATE
#         ${CMAKE_CURRENT_SOURCE_DIR}/ext/glm
# )

add_executable(${PROJECT_NAME} ${SRC_FILES})

# Link GLFW to your executable
target_link_libraries(${PROJECT_NAME}
    glfw
    GLEW::GLEW
    OpenGL::GL
    stb_image
    glm::glm
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
