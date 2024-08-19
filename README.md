
# SFML Physics Simulation

This is a simple physics simulation using the SFML library. The program demonstrates basic physics principles, including gravity, collision detection, and effects of external forces like a storm. It allows users to interact with the simulation by creating and manipulating balls on the screen.

## Features


- **Gravity**: Simulates gravity affecting the balls.
- **Ball Creation**: Balls can be generated by clicking the left mouse button.
- **Storm Effect**: Apply a storm effect by holding down the right mouse button, which pushes the balls towards the mouse pointer.
- **Collision Detection**: Handles collisions between balls, with basic restitution.
- **Screen Boundaries**: Balls bounce off the edges of the window.
- **Fullscreen Toggle**: Toggle fullscreen mode by pressing `F11`.

## Requirements

- SFML library

## Building and Running

1. **Clone the Repository:**

    ```sh
    git clone https://github.com/bencheikh-amir/SFML_Physics_simulation.git
    cd SFML_Physics_simulation
    ```

2. **Install SFML:**

    Follow the installation instructions on the [SFML website](https://www.sfml-dev.org/download.php) to set up the SFML library for your development environment.

3. **Compile the Code:**

    Ensure you have a C++ compiler and SFML installed. Compile the code with the following command:

    ```sh
    g++ -o simulation main.cpp -lsfml-graphics -lsfml-window -lsfml-system
    ```

4. **Run the Program:**

    ```sh
    ./simulation
    ```

## Usage

- **Left Click**: Creates a new ball at the mouse position.
- **Right Click**: Activates the storm effect, applying a force to balls towards the mouse pointer.
- **F11**: Toggle fullscreen mode.

## Contributing

Feel free to contribute by submitting issues or pull requests