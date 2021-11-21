Download: https://github.com/ZHU-MONEY/EAE-6320/archive/refs/heads/Final_Project_file.zip

Controls: XBOX controller left stick to move Homer

The Goal is to reach the donut without getting hit by car and not bumping into walls

Systems I used: My XBOX controller input system and Josh Butner’s Collision system

Josh Butner’s Collision system: https://www.dropbox.com/s/5s3ar7f4elxb1yv/EAE%206320%20Engine%20System%20Final%20Writeup.pdf?dl=0

![](Capture.PNG)

The game is just a simplified frogger, the goal is to move Homer to the other side and pickup the donut while not touching the “red parts”. I chose to make this game because, it is very simple to make and can be done in a very short time while still being able to showcase the systems I and the other person built.

The game was relatively simple to make, just based on what I already have for my engine. But the game code could have been a lot cleaner if the system was done with an Entity Component System fashion there was a lot of boilerplate code that could have been avoided.
My classmate’s system was very easy to use and integrate, there was no binary file involved, initialization and object creation is just processed within the engine without having to worry about writing LUA file and then loading them.
The interface is also very straightforward, it is similar to how Unity handles collision events.
One suggestion I would make is probably adding support for 2D collision resolution, because my game is just 2D in 3D engine, but the colliders must be in Vector3 and all values must be non ZERO, otherwise there might be unexpected behavior for collision detection.


What I have learned this semester is mainly about API design.

The purpose of assignments in this semester are also mainly targeting API designs, but there are also few parts that target how to debug code.

My professional goal has changed from working in game industry. So, the parts I learned about game engine specifics don’t really interest me anymore. Therefore, what I learned are mainly generic software engineering practices, for example API design and software architecture.


Regarding the topic of architecture and design of the Engine, I personally prefer spending more time up front to make sure each system is highly modular and scalable. Because spending time to design a good skeleton can make later development to be just filling up the skeleton without having to change the design which would make later development to be streamlined. Having a modular system would allow additional system to be incorporated easily and opening the possibility for open-sourced collaborations and 3rd party plug-in development.

In my opinion, scalability and modularity are the fundamental features of a good software architecture. I have worked previously on projects that are not scalable and modular, and the experience of working on such projects were frustrating, whenever I wanted to add new features, the entire code base had to make changes; therefore, doing anything was just time consuming. And sometimes it was extremely difficult implement new features because of limited time budget. 

Therefore, when I was assigned to work on a new project, I tried to spend a large amount of time to design a scalable and modular system, so the people working on the project after me would be able to implement changes and implementing new features more efficiently.
