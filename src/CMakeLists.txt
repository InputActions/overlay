find_package(LayerShellQt REQUIRED)
find_package(Qt6 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
    DBus
    Widgets
)

add_executable(inputactions-overlay
    dbus/ContextMenuOverlayInterface.cpp
    dbus/MouseStrokeOverlayInterface.cpp
    overlays/ContextMenuOverlay.cpp
    overlays/MouseStrokeOverlay.cpp
    overlays/Overlay.cpp
    DesktopEnvironment.cpp
    main.cpp
    OverlayManager.cpp
    OverlayWidget.cpp
)
target_link_libraries(inputactions-overlay PRIVATE
    LayerShellQt::Interface
    Qt6::DBus
    Qt6::Widgets
)

install(
    TARGETS inputactions-overlay
    COMPONENT overlay
)