# JWT Server Module for MTA:SA
[![Build Status](https://dev.azure.com/eXo-OpenSource/ml_redis/_apis/build/status/eXo-OpenSource.ml_redis?branchName=master)](https://dev.azure.com/eXo-OpenSource/ml_redis/_build/latest?definitionId=1&branchName=master)

## Developing
1. Download and install the EditorConfig plugin for Visual Studio: [https://github.com/editorconfig/editorconfig-visualstudio#readme](https://github.com/editorconfig/editorconfig-visualstudio#readme)
2. Launch `premake.bat`
3. Open `Build/Redis.sln`

### Debugging
1. Create a directory junktion to MTA-Server directory in the root of the repo
    * `mklink /J mta-server "PATH_TO_INSTALL_DIR\MTA San Andreas 1.5\server"`
2. Create a symlink to the build output dll in the modules directory
    * `cd PATH_TO_INSTALL_DIR\MTA San Andreas 1.5\server\x64\modules` for x64
    * `cd PATH_TO_INSTALL_DIR\MTA San Andreas 1.5\server\mods\deathmatch\modules` for Win32
    * `mklink ml_redis.dll "PATH_TO_REPO\Bin\Debug\ml_redis.dll"`
3. Press `F5` in Visual Studio and the MTA-Server should start

## Installing
1. Put the resulting `Bin/ml_redis.dll/.so` into your MTA:SA server `modules` folder.
2. Add `<module src="ml_redis.dll"/>` (or `.so` for Linux) to your `mtaserver.conf`.
3. Start the server

### Linux
In order to run the module you'll need to compile and install `cpp_redis` yourself.  
See [https://github.com/Cylix/cpp_redis/wiki/Mac-&-Linux-Install](https://github.com/Cylix/cpp_redis/wiki/Mac-&-Linux-Install) on how to do that.

## Depdencies
* https://github.com/cpp-redis/cpp_redis

## API
## Globals
*Todo*

## Functions
*Todo*

## Download
via https://github.com/Contextualist/glare
* [Linux x64](https://glare.now.sh/eXo-OpenSource/ml_redis/ml_redis.so)
* [Windows x64](https://glare.now.sh/eXo-OpenSource/ml_redis/ml_redis_x64.dll)
* [Windows x86](https://glare.now.sh/eXo-OpenSource/ml_redis/ml_redis_win32.dll)

## Contributors
* Stefan K.
