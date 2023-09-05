# Networked Multiplayer Game System with C++ 

I started this project back in my undergrad senior year under the guidance of Professor Kaliappa Ravindran, and then I later came back to it. Initially, the goal of the project was to meet this key requirement “What You See Is What I See” (WYSIWIS). What this means is that every player needs to maintain the same state and have the same view of the game at all times. This is a core problem, but I expanded this project to other areas by building the GUI from scratch with SDL2 and implementing the physics.

This time I had three main goals: learn C++, learn how to use network protocols with C++, and explore an algorithm design to reduce the effect of network latency which is key in a a fast real-time system such as multiplayer games. This meant implementing algorithms such as client-prediction, server reconciliation, interpolation, etc. The end goal is a smooth and consistent experience for the player. We try to approach a single-player-like environment, by reducing the effect of network latency.

## Key Points

Designed and Implemented a Networked Multiplayer Game System in C++, featuring a centralized server network architecture utilizing UDP socket programming and flatbuffers for efficient data transmission.

Enhanced Real-Time Gameplay Responsiveness by employing synchronization algorithms, including client-side prediction, server reconciliation, and interpolation, to mitigate network latency challenges.

Implemented a Graphical User Interface using SDL2, complete with realistic physics simulations encompassing velocity, friction, and collision handling.

## Requirements

**C++11** - This is the only language used.
**Mac OS** - I have a Mac, but I don’t see why it wouldn’t work on other machines with the right modifications

### External Libraries

**SDL2** - For GUI

**flatbuffers** - serialize and deserialize data over the network

### Files

**main.cpp** - client side game

**server.cpp** - server code