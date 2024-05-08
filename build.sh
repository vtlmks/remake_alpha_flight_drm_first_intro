#!/bin/bash

OUT_FILE="af-mabuse_first"

pushd data > /dev/null

# Function to extract filename without extension and clean it
get_clean_filename() {
    local fullpath="$1"
    local filename=$(basename -- "$fullpath")
    filename="${filename%.*}" # Remove extension
    filename="${filename//./_}" # Replace dots
    filename="${filename//-/_}" # Replace dashes
    filename="${filename// /_}" # Replace spaces
    echo "$filename"
}

# BMP files
for bmp in ../graphics/*.bmp; do
    base_name=$(get_clean_filename "$bmp")
    bmp2h -i "$bmp" -o "$base_name"
done

bin2h -i ../music/music.raw -o music

popd > /dev/null


# Build Linux version
gcc -O2 -shared -fPIC -o "remake_$OUT_FILE.so" remake.c -I../../include

# Build Windows version
x86_64-w64-mingw32-gcc -O2 -shared -o "remake_$OUT_FILE.dll" remake.c -I../../include

[ -e "remake_$OUT_FILE.so" ] && mv "remake_$OUT_FILE.so" ../../bin/remakes
[ -e "remake_$OUT_FILE.dll" ] && mv "remake_$OUT_FILE.dll" ../../bin/remakes
