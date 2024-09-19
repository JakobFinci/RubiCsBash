# RubiCsBash

Ian Eykamp, Kelsey McClung, Eliyahu Suskind, and Brooke Wager

A project to create and render a virtual Rubix Cube. The cube can either be viewed on a single device or through a network of 6 machines. Each machine will display a face of the Rubix cube and provide controls to rotate that face clockwise or counterclockwise.

## Requirements

This project uses the ncurses library and the criterion library. To install in Debian/Ubuntu Linux, run the following commands:

```
$ sudo apt-get install libncurses5-dev libncursesw5-dev
$ sudo apt-get install libcriterion-dev
```

## Compilation Instructions

To compile this program, download this GitHub repo and unzip it. Inside the unzipped folder, create and make a `build` directory by running the commands:

```
$ mkdir build
$ cd build
$ cmake ..
$ make
```

## Run on a Single Device

In the `build` directory, run the command:

```
$ ./src/rubiks_view
```

To choose which face to view, type in a number (0-5). The screen will update as soon as the character is pressed. Press `d` (must be lowercase) to rotate the current face clockwise, and press `a` (also must be lowercase) to rotate the face counterclockwise. Pressing space-bar while the cube is not solved will auto-solve the cube. To reshuffle the cube, press space again. To exit ncurses, press `q` or `Q`.

## Run on Multiple Devices

If you are planning to connect the server to multiple devices, you must edit the `ADDRESS` variable in `src/server/server_util.c` to the IP address the server is running on. To find the IP address, run the following command on the device the server will be running on:

```
$ ip address
```

Be sure to update the `ADDRESS` variable on ALL devices that will be running a client. After making this change, be sure to update the executables by entering the following command in the `build` directory:

```
$ make clean; make
```

### Start up the server

In the `build` directory, run the command:

```
$ ./src/server/run_server
```

The server is now running and accepting clients!

### Connect clients

You can connect up to 6 separate clients to the server. To connect a client, run the following command in the `build` directory.

```
$ ./src/server/run_client
```

If nothing is displaying, press `0`. The white face of the cube should now be displaying on your screen! If still nothing is happening, be sure to double check that the `ADDRESS` variable in `src/server/server_util.c` is correct and that the `PORT` variables of the server and the client match.

### Arrange the cube

If you (for some reason) actually have 6 clients running on 6 separate devices, you can arrange the devices in a cube shape. Ensure that each device is displaying a different face. The computers that will be standing normally are white (0), red (1), yellow (5), and orange (4). Arrange them counter-clockwise in that order with their screens facing out. The blue (2) screen goes on the top facing up with the bottom edge of the screen touching the red display. The orange (3) screen goes on the bottom facing down with the bottom edge of the screen touching the orange display. Voila, you now have a very cursed Rubik's cube.

### Solve the cube

Each device has its own controller. Pressing `d` (must be lowercase) on a device rotates that devices face clockwise, and `a` (also must be lowercase) rotates the face counterclockwise. Pressing space-bar on any device while the cube is not solved will auto-solve the cube. To reshuffle the cube, press space again on any device. To exit ncurses on a device, press `q` or `Q`.
