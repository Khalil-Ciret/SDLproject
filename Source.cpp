#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include <stdio.h>



//Key press surfaces constants
enum KeyPressSurfaces
{
	KEY_PRESS_SURFACE_DEFAULT,
	KEY_PRESS_SURFACE_UP,
	KEY_PRESS_SURFACE_DOWN,
	KEY_PRESS_SURFACE_LEFT,
	KEY_PRESS_SURFACE_RIGHT,
	KEY_PRESS_SURFACE_PNG,
	KEY_PRESS_SURFACE_TOTAL
};

using namespace std;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

class LTexture
{
public:

	//Constructor
	LTexture();

	//DESTRUCTOR
	~LTexture();

	//loads image at specified path
	bool loadFromFile(std::string path);

	void free();

	void render(int x, int y);

	int getWidth();
	int getHeight();

private:

	SDL_Texture* mTexture;

	int mWidth;
	int mHeight;

};

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

bool checkLoaded(SDL_Surface* shitToCheck);

//Loads individual image
SDL_Surface* loadSurface(std::string path);

//Loads individual image as texture
SDL_Texture* loadTexture(std::string path);

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The surface contained by the window
SDL_Surface* gScreenSurface = NULL;

//The images that correspond to a keypress
SDL_Surface* gKeyPressSurfaces[KEY_PRESS_SURFACE_TOTAL];

//Current displayed image
SDL_Surface* gCurrentSurface = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

//Current displayed texture
SDL_Texture* gTexture = NULL;

//Scene textures
LTexture gFooTexture;
LTexture gBackgroundTexture;

LTexture::LTexture()
{
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture::~LTexture()
{
	free();
}

bool LTexture::loadFromFile(std::string path)
{
	free();

	SDL_Texture* newTexture = NULL;

	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL)
	{
		printf("Unable to load the bloody fucking image ffs why does it keep happening oh wait it didn't happen even once i guess we're fine here");
	}
	else
	{
		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		if (newTexture == NULL)
		{
			printf("oh no");
		}
		else
		{
			//Get image dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		//Get rid of this stuff
		SDL_FreeSurface(loadedSurface);
	}

	mTexture = newTexture;
	return mTexture != NULL;
}


void LTexture::free()
{
	if (mTexture != NULL)
	{
		SDL_DestroyTexture(mTexture);
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

void LTexture::render(int x, int y)
{
	SDL_Rect renderQuad = { x,y,mWidth,mHeight };
	SDL_RenderCopy(gRenderer, mTexture, NULL, &renderQuad);
}

int LTexture::getWidth()
{
	return mWidth;
}

int LTexture::getHeight()
{
	return mHeight;
}







bool init() {

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL is acting up! Error: %s\n", SDL_GetError());
		return false;
	}
	else
	{
		gWindow = SDL_CreateWindow("SDL", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL) {
			printf("Error %c\n", SDL_GetError);
			return false;
		}
		else {
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
			if (gRenderer == NULL)
			{
				printf("It shouldn't be NULL");
				return false;
			}

			SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

			int imgFlags = IMG_INIT_PNG;
			if (!(IMG_Init(imgFlags) & imgFlags))
			{
				printf("SDL Image est en train de chier dans la colle");
				return false;
			}
			else
			gScreenSurface = SDL_GetWindowSurface(gWindow);
		}
	}
	return true;

}

bool loadMedia() {

	gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT] = loadSurface("ron.bmp");
	if (gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT] == NULL)
	{
		printf("Default image loading failed");
		return false;
	}
	gKeyPressSurfaces[KEY_PRESS_SURFACE_UP] = loadSurface("up.bmp");
	gKeyPressSurfaces[KEY_PRESS_SURFACE_DOWN] = loadSurface("down.bmp");
	gKeyPressSurfaces[KEY_PRESS_SURFACE_LEFT] = loadSurface("left.bmp");
	gKeyPressSurfaces[KEY_PRESS_SURFACE_RIGHT] = loadSurface("right.bmp");
	gKeyPressSurfaces[KEY_PRESS_SURFACE_PNG] = loadSurface("png.png");

	gTexture = loadTexture("texture.png");
	if (gTexture == NULL) {
		printf("Failure when loading the texture");
		return false;
	}

	//Loading success flag
	if (!gFooTexture.loadFromFile("./foo.png"))
	{
		printf("Hey mom look it's another useless error message.");
		return false;
	}

	//Load background texture
	if (!gBackgroundTexture.loadFromFile("./background.png"))
	{

		printf("FAILED TO PRINT SHIT");
		return false;
	}

	return (checkLoaded(gKeyPressSurfaces[KEY_PRESS_SURFACE_UP]) && checkLoaded(gKeyPressSurfaces[KEY_PRESS_SURFACE_DOWN]) && checkLoaded(gKeyPressSurfaces[KEY_PRESS_SURFACE_LEFT]) && checkLoaded(gKeyPressSurfaces[KEY_PRESS_SURFACE_RIGHT]));
}



bool checkLoaded(SDL_Surface* shitToCheck)
{
	return shitToCheck != NULL;
}

void close()
{
	SDL_FreeSurface(gCurrentSurface);
	gCurrentSurface = NULL;

	SDL_DestroyWindow(gWindow);
	gWindow = NULL;

	gFooTexture.free();
	gBackgroundTexture.free();

	SDL_DestroyRenderer(gRenderer);
	gRenderer = NULL;

	IMG_Quit();
	SDL_Quit();
}

SDL_Surface* loadSurface(string path)

{
	//Load image at specified path
	SDL_Surface* optimizedSurface = NULL;

	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
	}
	else
	{

		optimizedSurface = SDL_ConvertSurface(loadedSurface, gScreenSurface->format, 0);
		if (optimizedSurface == NULL)
		{
			printf("Unable to optimize");
		}

		SDL_FreeSurface(loadedSurface);
	}

	return optimizedSurface;
}

SDL_Texture* loadTexture(std::string path)
{
	SDL_Texture* newTexture = NULL;

	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	}
	else
	{
		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		if (newTexture == NULL)
		{
			printf("Unable to load texture from %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
		}

		SDL_FreeSurface(loadedSurface);
	}
	return newTexture;
}



int main(int argc, char* args[]) {

	if (!init())
	{
		printf("Heavy fail");
	}
	else
	{
		if (!loadMedia()) {
			printf("Can't load media for some reason");
		}
		else
		{
			SDL_BlitSurface(gCurrentSurface, NULL, gScreenSurface, NULL);
			SDL_UpdateWindowSurface(gWindow);
		}
	}

	bool quit = false;

	SDL_Event e;

	gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT];
	while (!quit)
	{

		bool isTexture = false; 
		while (SDL_PollEvent(&e) != 0)
			if (e.type == SDL_QUIT)
				quit = true;
			else
				if (e.type == SDL_KEYDOWN)
				{
					switch (e.key.keysym.sym)
					{
					case SDLK_UP:
						isTexture = false;
						gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_UP];
						break;


					case SDLK_DOWN:
						isTexture = false;
						gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_DOWN];
						break;

					case SDLK_LEFT:
						isTexture = false;
						gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_LEFT];
						break;

					case SDLK_RIGHT:
						isTexture = false;
						gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_RIGHT];
						break;

					case SDLK_p:
						isTexture = false;;
						gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_PNG];
						break;

					case SDLK_t:
						isTexture = true;
						//Top-right 
						SDL_Rect topRightViewPort;
						topRightViewPort.x = SCREEN_WIDTH / 2;
						topRightViewPort.y = 0;
						topRightViewPort.w = SCREEN_WIDTH / 2;
						topRightViewPort.h = SCREEN_HEIGHT / 2;
						SDL_RenderSetViewport(gRenderer, &topRightViewPort);

						SDL_RenderCopy(gRenderer, gTexture, NULL, NULL);

						//Top-left
						SDL_Rect topLeftViewPort;
						topLeftViewPort.x = 0;
						topLeftViewPort.y = 0;
						topLeftViewPort.w = SCREEN_WIDTH / 2;
						topLeftViewPort.h = SCREEN_HEIGHT / 2;
						SDL_RenderSetViewport(gRenderer, &topLeftViewPort);

						SDL_RenderCopy(gRenderer, gTexture, NULL, NULL);

						//Bottom viewport
						SDL_Rect bottomViewport;
						bottomViewport.x = 0;
						bottomViewport.y = SCREEN_HEIGHT / 2;
						bottomViewport.w = SCREEN_WIDTH;
						bottomViewport.h = SCREEN_HEIGHT / 2;
						SDL_RenderSetViewport(gRenderer, &bottomViewport);

						SDL_RenderCopy(gRenderer, gTexture, NULL, NULL);

						//Render texture to screen
						SDL_RenderPresent(gRenderer);
						break;

					case SDLK_k:

						isTexture = true;
						//Clear the fucking screen
						SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
						SDL_RenderClear(gRenderer);

						//Render background texture to the fucking screen
						gBackgroundTexture.render(0, 0);

						//Render the other fucking texture to the fucking screen

						gFooTexture.render(240, 190);

						//UPDATE THE FUCKING SCREEN
						SDL_RenderPresent(gRenderer);

						break;




					default:
						isTexture = false;
						gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT];
						break;
					}

					if (!isTexture) {
						SDL_Rect stretchRect;
						stretchRect.x = 0;
						stretchRect.y = 0;
						stretchRect.w = SCREEN_WIDTH;
						stretchRect.h = SCREEN_HEIGHT;
						SDL_BlitScaled(gCurrentSurface, NULL, gScreenSurface, &stretchRect); 
						SDL_UpdateWindowSurface(gWindow);
					}
				}
	}
	close();
	return 0;
}
