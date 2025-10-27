@echo off
powershell.exe -NoProfile -ExecutionPolicy Bypass "& {& \"%~dp0scripts\bootstrap.ps1\" %*}"
> vcpkg install pugixml:x64-windows
> vcpkg install geographiclib:x64-windows
> vcpkg install polyclipping:x64-windows
> vcpkg install HDF5:x64-windows
> vcpkg install libxslt:x64-windows
> vcpkg install openssl:x64-windows
> vcpkg install sqlite3:x64-windows