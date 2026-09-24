find_package(LayerShellQt REQUIRED)
find_package(Qt6 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
    DBus
    Widgets
)

add_executable(inputactions-overlay
    main.cpp
    DesktopEnvironment.cpp
    MouseStrokeOverlay.cpp
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