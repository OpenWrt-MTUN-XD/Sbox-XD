#!/bin/bash

sudo apt-get install fuse -y

cp -r linux64 sboxxd.AppDir

# The file for Appimage

rm sboxxd.AppDir/launcher

cat >sboxxd.AppDir/sbox-xd.desktop <<-EOF
[Desktop Entry]
Name=Sbox-XD
Exec=echo "Sbox-XD started"
Icon=sbox-xd
Type=Application
Categories=Network
EOF

cat >sboxxd.AppDir/AppRun <<-EOF
#!/bin/bash
echo "PATH: \${PATH}"
echo "Sbox-XD runing on: \$APPDIR"
LD_LIBRARY_PATH=\${APPDIR}/usr/lib QT_PLUGIN_PATH=\${APPDIR}/usr/plugins \${APPDIR}/sbox-xd -appdata "\$@"
EOF

chmod +x sboxxd.AppDir/AppRun

# build

curl -fLSO https://github.com/AppImage/AppImageKit/releases/latest/download/appimagetool-x86_64.AppImage
chmod +x appimagetool-x86_64.AppImage
./appimagetool-x86_64.AppImage sboxxd.AppDir

# clean

rm appimagetool-x86_64.AppImage
rm -rf sboxxd.AppDir
