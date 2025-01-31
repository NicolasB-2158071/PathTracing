## Install

[vcpkg](https://learn.microsoft.com/en-us/vcpkg/users/buildsystems/cmake-integration) is used for the majority of dependencies with the exception of Embree4:

1. Install the platform depended executables, found [here](https://github.com/RenderKit/embree/releases)

2. Place the entire unzipped folder in the **Lib** directory

3. Change the location in CMakeLists.txt

## Usage

```
./Raytracer
--scene ../Res/Geometry/Bunny.obj ../Res/Materials/BunnyMicrofacet.json
--depth 4
--spp 128
--save ../Renders/BunnyMicrofacet.jpeg
--integrator IndirectIlluminationIntegrator
--gui
```
