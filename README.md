# Ship simulation

This is a program to simulate our ships for our Research Project: `Applying swarm and modular robotics in maritime shipping`

## Requirements

-   gcc
-   make
-   SDL2 - installed using homebrew

    ```bash
    brew install sdl2
    ```

## How to run

1. Clone the repository
2. Build the box2d library

    ```bash
    cd box2d
    sh build.sh
    cd ..
    ```

3. Run:

    ```bash
    make
    ./simulation

    # or

    make rerun
    ```

4. Enjoy!
