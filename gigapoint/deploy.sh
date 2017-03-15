#!/bin/sh

echo "Deploy gigapoint module to dist for CAVE demo use"

rm -rf dist/*
mkdir -p dist
echo "copy lib gigapoint.so"
cp build/gigapoint.so dist
echo "copy resource"
cp -r gigapoint_resource dist

echo "Done"
