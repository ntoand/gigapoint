#!/bin/sh

echo "Deploy gigapoint module to dist for CAVE demo use"

rm -rf dist/*
mkdir -p dist
echo "copy lib gigapoint.so"
cp build/gigapoint.so dist
echo "copy resource"
cp -r gigapoint_resource dist
#cp omegalib_module_test/gigapoint_menu.py ./dist/
cd dist
ln -s ../../../git/IDR/multiuser_fracturetracing.py .
ln -s ../../../git/IDR/multitarget_MTape.py .
ln -s ../../../git/IDR/onlineupdatedemo.py .
ln -s ../../../git/IDR/onlineupdatedemo_generator.py .
cp ../../../git/IDR/crosshair_* .
echo "Done"
