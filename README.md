# Shadow Run

This is a small C++ game project I made with SFML. It opens a blue window and
shows a character using pictures from sprite sheets.

## What main.cpp does

- Makes the game window and the ground line.
- Loads the run, jump, climb and slide pictures from `assets/characters`.
- Changes the character picture when a key is pressed.
- Shows the animation frames one after another to make the character move.

## How to play

- Press the Right Arrow to run.
- Press Space to jump.
- Press the Up Arrow to climb.
- Press the **Down Arrow** to slide.

## Build and run in MSYS2 UCRT64

Open the MSYS2 UCRT64 terminal and go to the project folder. For example:

```sh
cd /d/Mysfml
```

 build the project with CMake:

```sh
cmake -S . -B build
cmake --build build
```

Then start the game:

```sh
./build/ShadowRun.exe
```

The project needs CMake and SFML 3 to be installed in the UCRT64 environment.
The images are copied beside the game when it builds, so keep the `assets`
folder in the project.
