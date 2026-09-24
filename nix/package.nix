{
  cmake,
  extra-cmake-modules,
  layer-shell-qt,
  lib,
  qttools,
  stdenv,
  wrapQtAppsHook,
  ...
}:

stdenv.mkDerivation rec {
  pname = "inputactions-overlay";
  version = "0.10.0.0";

  src = ./..;

  nativeBuildInputs = [
    cmake
    extra-cmake-modules
    wrapQtAppsHook
  ];

  buildInputs = [
    layer-shell-qt
    qttools
  ];

  meta = with lib; {
    description = "InputActions overlay";
    license = licenses.gpl3;
    homepage = "https://github.com/InputActions/overlay";
  };
}
