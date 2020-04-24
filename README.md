AnyVNC
======

A universal framework for implementing VNC server and client software:

* Any frontend (UI)
* Any backend (VNC server/client protocol implementation)
* Any platform
* Any framebuffer format
* Any input method
* Any encoding
* Any authentication method
* Any screen configuration

Characteristics
---------------

* Modern
  - Implementation in C++17
  - CMake-based build system
  - Support recent platforms only (Windows 10, Android 8.1+, Wayland)
  - Default frontend implemented using QML/Qt Quick
* Modular
  - All layers implemented via plugins only: frontends, backends, framebuffer I/O, input methods, authentication methods
* Vendor and platform independent
  - Core library written in pure C++
  - Plugin-based platform abstraction with minimal but perfect implementations
  - Full compliance with RFB standard without vendor-specific features per default
  - Any VNC library usable using backend plugins - no explicit dependency on a particular library
  - Optional Qt bindings via libanyvnc-qt-core, libanyvnc-qt-widgets and libanyvnc-qt-quick
* Extensible
  - Additional encodings via plugins
  - Additional authentication methods via plugins
