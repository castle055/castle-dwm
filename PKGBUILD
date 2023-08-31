pkgname=cyd-wm
pkgver=r$(git rev-parse --short HEAD)
pkgrel=1
pkgdesc="Cydonia Window Manager"
arch=('any')
license=('GPL3')

depends=(
    libx11 libxrender fontconfig libxft libxinerama yaml-cpp libjpeg-turbo
    lightdm lightdm-gtk-greeter accountsservice
 )

build() {
  cd ..

  cmake -DCMAKE_BUILD_TYPE=Release -G Ninja -S . -B ./cmake-build-release
  cmake --build ./cmake-build-release --target castle-dwm
}

package() {
  cd ..

  # Install static library binary
  install -Dm755 cmake-build-release/castle-dwm "$pkgdir/usr/bin/castle-dwm"
  install -Dm755 start_cwm "$pkgdir/usr/bin/start_cwm"

  # Install xsession file
  install -Dm644 cdwm.desktop "$pkgdir/usr/share/xsessions/cdwm.desktop"
}
