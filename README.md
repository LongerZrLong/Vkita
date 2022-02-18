# Vkita
A vulkan game engine for learning purpose.

## Dependencies
#### Vulkan
Download Vulkan-SDK from https://vulkan.lunarg.com/sdk/home and install.

## Get Started
#### MacOS
```
git clone --recursive https://github.com/LongerZrLong/Vkita.git
cd Vkita
mkdir build
cd build
cmake ..
make all
```

After build finishing, take a look at the `Test` directory in `build` directory. There should be some test executables that can be 
started in terminal. e.g.
```
cd Test
./EditorCameraTest
```

#### Linux and Windows
Although it is only tested on MacOS. The build and run process for Linux and Windows should be similar.