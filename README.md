# EverDawn-2D-Engine
Everdawn es un proyecto personal que utilizo para aprender SDL2 y C.
No tengo idea de como seguir su desarrollo, habilitare los pull request para que la comunidad se encargue de ello.
Este codigo siempre estara disponible en este repositorio para todo el que quiera hecharle un vistazo.

Requisitos : 
SDL2
SDL2_image
SDL2_mixer
SDL2_net
SDL2_ttf

Debe haber una carpeta SDL2 en external/include donde se incluyan los headers. 
las librerias (libSDL2) van en external/lib
los DLL deben ir en la carpeta bin.

Compilacion : 
Windows : 
MSYS2
en la raiz del proyecto ejecuta make.

Linux : 
No lo he probado realmente, supongo que sera suficiente con quitar -lmingw32 del makefile y utilizar make.
