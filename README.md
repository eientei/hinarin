# Bulding

Support
=======
Currently supported OS:

- Windows 10
- Archlinux
- ... maybe more in the future

Getting sources
===============

```
git clone https://github.com/eientei/hinarin
cd hinarin
git submodule init
git submodule update
mkdir build
cd build
```

Configuring (Native)
=================

```
cmake ../
```

Configuring (Crosscompile)
=================

```
cmake -DCMAKE_TOOLCHAIN_FILE=../toolchain/mingw64.cmake ../ #or so
```

Building
=================

```
make
```

As result -- single executable `hinarin` (or `hinarin.exe`).
