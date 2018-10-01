# WASM-GOL
An implementation of Conway's Game of Life using a WebAssembly module and JavaScript.
Building this project requires the Emscripten SDK.

It works by performing most of the computation for each generation in the WebAssembly module, and then using Emscripten's port of SDL, it copies the rendered frame buffer into the pixel data for the canvas element on the page.

I've limited the frame rate to 15FPS, as the default frame rate of 60FPS was too fast.
