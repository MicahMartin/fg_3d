# to build
download, go to root of repo

```
mkdir buikd
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_INSTALL_PREFIX=./install -G Ninja
cmake --build build
./build/src/fg_3d
```

