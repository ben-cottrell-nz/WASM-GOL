# WASM-GOL
An implementation of Conway's Game of Life using a WebAssembly module and JavaScript. This repository contains a pre-built web assembly module, otherwise
you can build this project using 'build.sh'. Make sure you have a recent version of the Emscripten SDK, which can be installed according to this guide: https://webassembly.org/getting-started/developers-guide/.

It works by performing most of the computation for each generation in the WebAssembly module, and then using Emscripten's port of SDL, it copies the rendered frame buffer into the pixel data for the canvas element on the page.

I've limited the frame rate to 15FPS, as the default frame rate of 60FPS was too fast.
