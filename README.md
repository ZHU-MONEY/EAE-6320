System: Xbox Controller support

My system would add Xbox controller support to my game engine.

The system would have an easy-to-use interface that allows users to add function calls to button presses and/or trigger pulls without having to deal with all the complication of Xinput library.

The system would use Xinput library as a base and I would build an interface layer on top of it that exposes only what the end user would need to use. 

I would also add customization with human readable LUA file to modularize:
-	Vibration
-	Button hold time
-	Trigger analog input
-	Etc.

The main focus now would be learning the Xinput API and try testing its functionality and discover if there are other parts that can be customized.

If I end up having extra time, which is unlikely to happen, adding support to PlayStation controller would be nice. But after some research I have learned that to add support for PlayStation controller on Windows would be quite complicated and would require installing other 3rd party SDKs and libraries.
