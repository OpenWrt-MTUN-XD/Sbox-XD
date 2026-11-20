#!/bin/bash

version="$1"

mkdir -p sboxxd/DEBIAN
mkdir -p sboxxd/opt
cp -r linux64 sboxxd/opt/
mv sboxxd/opt/linux64 sboxxd/opt/sbox-xd
rm -rf sboxxd/opt/sbox-xd/usr
rm -f sboxxd/opt/sbox-xd/launcher

# basic
cat >sboxxd/DEBIAN/control <<-EOF
Package: sbox-xd
Version: $version
Architecture: amd64
Maintainer: internetdroid
Depends: libxcb-xinerama0, libqt5core5a, libqt5gui5, libqt5network5, libqt5widgets5, libqt5svg5, libqt5x11extras5, desktop-file-utils
Description: Qt based cross-platform GUI proxy configuration manager (backend: sing-box)
EOF

cat >sboxxd/DEBIAN/postinst <<-EOF
if [ ! -s /usr/share/applications/sbox-xd.desktop ]; then
    cat >/usr/share/applications/sbox-xd.desktop<<-END
[Desktop Entry]
Name=Sbox-XD
Comment=Qt based cross-platform GUI proxy configuration manager (backend: sing-box)
Exec=sh -c "PATH=/opt/sbox-xd:\$PATH /opt/sbox-xd/sbox-xd -appdata"
Icon=/opt/sbox-xd/sbox-xd.png
Terminal=false
Type=Application
Categories=Network;Application;
END
fi

setcap cap_net_admin=ep /opt/sbox-xd/sboxxd_core

update-desktop-database
EOF

sudo chmod 0755 sboxxd/DEBIAN/postinst

# desktop && PATH

sudo dpkg-deb -Zxz --build sboxxd
