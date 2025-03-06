emcc -o3 \
  Core/*.cpp \
  FilmicCurve/*.cpp \
  main.cpp \
  -o ./out/filmic.js \
  -s WASM=1 \
  -s MODULARIZE=1 \
  -s EXPORT_NAME="createFilmicModule" \
  -s ALLOW_MEMORY_GROWTH=1 \
  -std=c++17 \
  --bind