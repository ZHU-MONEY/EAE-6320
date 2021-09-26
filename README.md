Download: https://github.com/ZHU-MONEY/EAE-6320/archive/refs/heads/Assignment_05_file.zip
WASD to the camera
Up Down Left Right keys to move the game object 

![](Animation.gif)

![](gameobject.PNG)

The game object class would need a renderable part that is a mesh and an effect pair, plus it would need a coordinate system that is the rigid body state, which contains the physics for movement and location, rotation, orientation.
![](submit.PNG)

GL size:
![](glsize.PNG)
D3D size:
![](d3dsize.PNG)

extrapolation/prediction is necessary to prevent jerky movement which can be caused by different update time for 2 system, so the render system may lag behind the physics simulation
