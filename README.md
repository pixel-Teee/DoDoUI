# DoDoUI
DoDoUI



## Build

```c++
git submodule update --init --recursive
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug 
    
or cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_GENERATOR_TOOLSET=ClangCL
```



# Test

```c++
mkdir BuildTest
cd BuildTest
ctest --build-and-test .. . --build-generator "Visual Studio 17 2022" --build-options -DENABLE_TEST=1 --test-command ctest -C Debug
```

