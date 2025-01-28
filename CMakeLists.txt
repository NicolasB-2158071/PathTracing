cmake_minimum_required(VERSION 3.8)

# if mvsc, doesnt work
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest")

project("Raytracer")

# Find cmake-config for embree
set(embree_DIR ${CMAKE_SOURCE_DIR}/Lib/embree-4.3.3.x64.windows/lib/cmake/embree-4.3.3)
find_package(embree 4 REQUIRED)
find_package(tinyobjloader REQUIRED)
find_package(spdlog REQUIRED)
find_package(glm REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glad REQUIRED)
find_package(OpenMP REQUIRED)
find_package(jsoncpp REQUIRED)

file(GLOB ImGUI "Lib/ImGUI/*.h" "Lib/ImGUI/*.cpp")

add_executable(Raytracer
        ${ImGUI}
        "Lib/toojpeg.h" "Lib/toojpeg.cpp"
        "Src/main.cpp" "Src/Scene/Scene.h" "Src/Scene/Scene.cpp" "Src/Scene/Shape/BxDF/BxDF.h" "Src/Scene/Shape/Shape.h" "Src/Renderer.h" "Src/Renderer.cpp"
        "Src/Image/Image.h" "Src/Image/Image.cpp"  "Src/Camera/Camera.h" "Src/Camera/Camera.cpp"
        "Src/PostProcessing/ToneMapTypes/ClampToneMap.h" "Src/PostProcessing/ToneMapTypes/ClampToneMap.cpp" "Src/PostProcessing/ToneMapTypes/ReinhardToneMap.h" "Src/PostProcessing/ToneMapTypes/ReinhardToneMap.cpp"
        "Src/Utility/Math.h" "Src/Utility/Ray.h" "Src/Exception/ParseException.h" "Src/Utility/SmoothingNormals.h" "Src/Scene/Light/Light.h" 
        "Src/Integrator/Integrator.h" "Src/Integrator/Integrator.cpp"
        "Src/Integrator/DirectIlluminationIntegrator.h" "Src/Integrator/DirectIlluminationIntegrator.cpp" "Src/Scene/Light/PointLight.h" "Src/Scene/Light/PointLight.cpp"
        "Src/Scene/Light/AreaLight.h" "Src/Scene/Light/AreaLight.cpp" "Src/Scene/Shape/BxDF/LambertianDiffuseBRDF.h" "Src/Scene/Shape/BxDF/LambertianDiffuseBRDF.cpp"
        "Src/Sampler/Sampler.h" "Src/Sampler/Sampler.cpp" "Src/Sampler/LightSampler.h" "Src/Sampler/LightSampler.cpp" "Src/Scene/Shape/SurfaceIntersection.h" "Src/Exception/ArgsException.h" "Src/GUI/GUI.h" "Src/GUI/GUI.cpp" "Src/Integrator/IndirectIlluminationIntegrator.h" "Src/Integrator/IndirectIlluminationIntegrator.cpp" "Src/Scene/Shape/BxDF/BlinnPhongBRDF.h" "Src/Scene/Shape/BxDF/BlinnPhongBRDF.cpp" "Src/Sampler/Sampling.h" "Src/Integrator/WhittedIntegrator.h" "Src/Integrator/WhittedIntegrator.cpp" "Src/Scene/Shape/BxDF/BxDF.cpp" "Src/Scene/Shape/BxDF/PhongBRDF.h" "Src/Scene/Shape/BxDF/PhongBRDF.cpp" "Src/Integrator/RandomWalkIntegrator.h"  "Src/Integrator/RandomWalkIntegrator.cpp" "Src/Utility/CLIParser.h" "Src/Scene/Shape/BxDF/ConductorBxDF.h" "Src/Scene/Shape/BxDF/DielectricBxDF.h" "Src/Scene/Shape/BxDF/ConductorBxDF.cpp" "Src/Scene/Shape/BxDF/DielectricBxDF.cpp" "Src/Sampler/GGX.h" "Src/Sampler/GGX.cpp")

target_link_libraries(Raytracer embree)
target_link_libraries(Raytracer tinyobjloader::tinyobjloader)
target_link_libraries(Raytracer spdlog::spdlog)
target_link_libraries(Raytracer glm::glm)
target_link_libraries(Raytracer glfw)
target_link_libraries(Raytracer glad::glad)
target_link_libraries(Raytracer jsoncpp_lib)
if(OpenMP_CXX_FOUND)
    target_link_libraries(Raytracer OpenMP::OpenMP_CXX)
endif()

# Add embree dll's to output folder
add_custom_command(TARGET Raytracer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:Raytracer>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/lib/embree-4.3.3.x64.windows/bin/embree4.dll"
        "${CMAKE_SOURCE_DIR}/lib/embree-4.3.3.x64.windows/bin/tbb12.dll"
        "${CMAKE_SOURCE_DIR}/lib/embree-4.3.3.x64.windows/bin/tbbmalloc.dll"
        $<TARGET_FILE_DIR:Raytracer>
)