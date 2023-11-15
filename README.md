# Evolution_Simulator
(THIS PROJECT IS NOT FINISHED) Senior project for CS which simulates an evolving predator-prey system. Each entity is controlled by a neural network which mutates every generation if it reproduces.
---------------------------------------------------
Created by Colin M Wareham
---------------------------------------------------
This project introduced me to several new concepts in programming as well as further developed specific skills:
1. My first implementation of a Neural Network from scratch.
2. My first project to use multiple machine learning techniques.
3. My largest project to date.
4. My most extensive implementation of tests.
5. My first project where performance was at the forefront of development.
6. Better software development principles than my first RayLib project. Improved readability, seperation of concerns, and more loose coupling have allowed me to comfortably scale the project.


Description:

For my senior project at Marian University, I would return to RayLib in attempt to simulate nature's process of evolution using artifical neural networks. At the start of the simulation, hundreds of predators and prey are thrown into an arena where they can only see, and preform 5 different actions: turn left, turn right, move forward, attack, and reproduce. Their behavior is controlled by a neural network, which has their sight and health as input, and outputs angular velocity, rotation direction, linear velocity, and attack/not attack. For each entity in the starting generation, their neural networks are completely randomized, and only the lucky ones who stumble upon reproductive success will pass a slightly mutated version of their neural network parameters to their children. After hundreds of generations, the predators and prey have become highly calculated and intentional organisms, who employ novel tactics for either hunting or avoiding being eaten.
