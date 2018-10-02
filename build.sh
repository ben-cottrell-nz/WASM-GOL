#emcc core.cpp -s WASM=1 -s EXPORTED_FUNCTIONS='["reset"]' -s EXTRA_EXPORTED_RUNTIME_METHODS='["ccall", "cwrap"]' -s USE_SDL=2 -O3 -o index.js
#emcc core.cpp -s WASM=1 -s EXTRA_EXPORTED_RUNTIME_METHODS='["ccall", "cwrap"]' -s USE_SDL=2 -O3 -o index.js
emcc core.cpp -s WASM=1 -s USE_SDL=2 -O3 --bind -o gol.js
