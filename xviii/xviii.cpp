#include "stdafx.h"
#include "Game.h"

#include <Windows.h>

/*
TODO:
	-Have a basic readable config file where several simple parameters can be set (resolution, deployment points,
	tile sizes, etc.)

	-Script units. There would only be 4 basic unit types (infantry, artillery, cavalry, mortar); all units would
	be scripted in LUA or something

	-Implement specialised MudAnt and HillAnt classes
*/

/*
IMPORTANT:
If you are using a custom tile set, make sure tile sizes are set correctly in TextureManager
*/

//Release version

/*
int CALLBACK WinMain(_In_  HINSTANCE hInstance, _In_  HINSTANCE hPrevInstance, _In_  LPSTR lpCmdLine, _In_  int nCmdShow)
{
	Game game;	
	return 0;
}	
*/

//Debug version:
int main(int argc, char* argv[]){
	Game game;
	return 0;
}

