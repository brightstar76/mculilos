# MCU-lilOS

Minimalistic pseudo-OS designed to simplify making IoT applications.

# Multi-tasking

Not multi-threading! This "OS" operates by running cycles of tasks in a loop. And even though MCU-lilOS relies on tasks using non-blocking calls and returning from tasks as soon as they have nothing to do, many calls on MCUs are actually blocking. This disrupts the OS loop.

# Device manager

MCU-lilOS aims to be a wrapper providing unified API for all sorts of devices accessible from tasks.

# IPC or System Bus

Tasks can communicate with each other by sending each other messages and then optionally await for a response.

# TODOs / FIXMEs

This software is in the eartly stage of development and doesn't yet provide API in the form it's intended to do.

Feel free to explore the code, but don't count on that it'll have all you need to set up an applications.

Future versions will show.

# ESP8266

Support for AVR devices has been dropped, as their techinical parameters are too weak to run an IoT. ESP8266 will run MCU-lilOS just fine.


