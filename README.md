# Evolution_Simulator
(THIS PROJECT IS NOT FINISHED) Senior project for CS which simulates an evolving predator-prey system. Each entity is controlled by a neural network which mutates every generation if it reproduces.
---------------------------------------------------
Created by Colin M Wareham
---------------------------------------------------
This project introduced me to several new projects in programming as well as further developed specific skills:
1. My firt implementation of a Neural Network from scratch.
2. My first project to use multiple machine learning techniques.
3. My largest project to date.
4. My most extensive implementation of tests.
5. My first project where performance was at the forefront of development.
6. Better software development principles than my first RayLib project. Improved readability, seperation of concerns, and more loose coupling have allowed me to comfortably scale the project.


Description:

Neural network is implimented from scratch and uses no libraries outside of the STL.
At any point in time in the simulation, an entity is highlighted and it's neural network is displayed in real-time.
Predators reproduce when they eat a prey, and prey reproduce every few hundred game ticks.
Entities have 2D vision, specified field of view, and a limited distance at which they can see.
