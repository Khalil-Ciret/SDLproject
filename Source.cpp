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

	void setColor(Uint8 red, Uint8 green, Uint8 blue);

	void setBlendMode(SDL_BlendMode blending);

	void setAlpha(Uint8 alpha);

	void render(int x, int y, SDL_Rect* clip = NULL);

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

//Scene sprites
SDL_Rect gSpriteClips[4];
LTexture gSpriteSheetTexture;

//Modulated texture
LTexture gModulatedTexture;

//Alpha as fuck textures
LTexture gModulatedAlphaTexture;
LTexture gBackgroundAlphaTexture;

//animation Stuff
const int WALKING_ANIMATION_FRAMES = 4;
SDL_Rect gSpriteClipsAnim[WALKING_ANIMATION_FRAMES];
LTexture gSpriteSheetAnimTexture;

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

void LTexture::setBlendMode(SDL_BlendMode blending)
{
	//Set blending function
	SDL_SetTextureBlendMode(mTexture, blending);
}

void LTexture::setAlpha(Uint8 alpha)
{
	//Modulate texture alpha
	SDL_SetTextureAlphaMod(mTexture, alpha);
}

void LTexture::setColor(Uint8 red, Uint8 green, Uint8 blue)
{
	SDL_SetTextureColorMod(mTexture, red, green, blue);
}

void LTexture::render(int x, int y, SDL_Rect* clip)
{
	SDL_Rect renderQuad = { x,y,mWidth,mHeight };

	if (clip != NULL)
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}
	SDL_RenderCopy(gRenderer, mTexture, clip, &renderQuad);
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
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
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
	if (!gSpriteSheetTexture.loadFromFile("dots.png"))
	{
		printf("Failed to load sprite sheet texture!\n");
		return false;
	}
	else
	{
		//Set top left sprite
		gSpriteClips[0].x = 0;
		gSpriteClips[0].y = 0;
		gSpriteClips[0].w = 100;
		gSpriteClips[0].h = 100;

		//Set top right sprite
		gSpriteClips[1].x = 100;
		gSpriteClips[1].y = 0;
		gSpriteClips[1].w = 100;
		gSpriteClips[1].h = 100;

		//Set bottom left sprite
		gSpriteClips[2].x = 0;
		gSpriteClips[2].y = 100;
		gSpriteClips[2].w = 100;
		gSpriteClips[2].h = 100;

		//Set bottom right sprite
		gSpriteClips[3].x = 100;
		gSpriteClips[3].y = 100;
		gSpriteClips[3].w = 100;
		gSpriteClips[3].h = 100;
	}
	//Load modulated texture
	if (!gModulatedAlphaTexture.loadFromFile("colors.png")) {
		printf("Go fuck yourself, go fuck yourself, please go go fuck yourself");
	}
	if (!gModulatedAlphaTexture.loadFromFile("fadeout.png"))
	{
		printf("Failed to load front texture!\n");
		return false;
	}
	else
	{
		//Set standard alpha blending
		gModulatedAlphaTexture.setBlendMode(SDL_BLENDMODE_BLEND);
	}

	//Load background texture
	if (!gBackgroundAlphaTexture.loadFromFile("fadein.png"))
	{
		printf("Failed to load background texture!\n");
		return false;
	}


	//Animation time
	if (!gSpriteSheetAnimTexture.loadFromFile("anim.png"))
	{
		printf("Je sais que ça faisait très longtemps mais il semblerait que nous ayons sous les yeux un autre message d'erreur de qualité");
		return false;
	}
	else
	{
		gSpriteClipsAnim[0].x = 0;
		gSpriteClipsAnim[0].y = 0;
		gSpriteClipsAnim[0].w = 64;
		gSpriteClipsAnim[0].h = 205;

		gSpriteClipsAnim[1].x = 64;
		gSpriteClipsAnim[1].y = 0;
		gSpriteClipsAnim[1].w = 64;
		gSpriteClipsAnim[1].h = 205;

		gSpriteClipsAnim[2].x = 128;
		gSpriteClipsAnim[2].y = 0;
		gSpriteClipsAnim[2].w = 64;
		gSpriteClipsAnim[2].h = 205;

		gSpriteClipsAnim[3].x = 196;
		gSpriteClipsAnim[3].y = 0;
		gSpriteClipsAnim[3].w = 64;
		gSpriteClipsAnim[3].h = 205;

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

	int frame = 0;
	SDL_Event e;

	Uint8 r = 255;
	Uint8 g = 255;
	Uint8 b = 255;
	Uint8 a = 255;
	bool isFuckingAroundWithColors = false;
	bool isFuckingAroundWithTransparency = false;
	bool isTryingToAnimateShit = false;
	gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT];


	while (!quit)
	{

		bool isTexture = false;

		while (SDL_PollEvent(&e) != 0) {

			if (e.type == SDL_QUIT)
				quit = true;
			else
				if (e.type == SDL_KEYDOWN)
				{
					switch (e.key.keysym.sym)
					{
					case SDLK_UP:
						isFuckingAroundWithColors = false;
						isFuckingAroundWithTransparency = false;
						isTexture = false;
						gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_UP];
						break;


					case SDLK_DOWN:
						isFuckingAroundWithColors = false;
						isFuckingAroundWithTransparency = false;
						isTexture = false;
						gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_DOWN];
						break;

					case SDLK_LEFT:
						isFuckingAroundWithColors = false;
						isFuckingAroundWithTransparency = false;
						isTexture = false;
						gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_LEFT];
						break;

					case SDLK_RIGHT:
						isFuckingAroundWithColors = false;
						isFuckingAroundWithTransparency = false;
						isTexture = false;
						gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_RIGHT];
						break;

					case SDLK_p:
						isFuckingAroundWithColors = false;
						isFuckingAroundWithTransparency = false;
						isTexture = false;;
						gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_PNG];
						break;

					case SDLK_t:
						isFuckingAroundWithColors = false;
						isFuckingAroundWithTransparency = false;
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
						isFuckingAroundWithColors = false;
						isFuckingAroundWithTransparency = false;
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



					case SDLK_s:
						if (!isFuckingAroundWithTransparency) {
							isTexture = true;
							SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
							SDL_RenderClear(gRenderer);

							//render top left i guess
							gSpriteSheetTexture.render(0, 0, &gSpriteClips[0]);

							//Render top right sprite
							gSpriteSheetTexture.render(SCREEN_WIDTH - gSpriteClips[1].w, 0, &gSpriteClips[1]);

							//Render bottom left sprite
							gSpriteSheetTexture.render(0, SCREEN_HEIGHT - gSpriteClips[2].h, &gSpriteClips[2]);

							//Render bottom right sprite
							gSpriteSheetTexture.render(SCREEN_WIDTH - gSpriteClips[3].w, SCREEN_HEIGHT - gSpriteClips[3].h, &gSpriteClips[3]);

							//Update screen
							SDL_RenderPresent(gRenderer);
						}
						else
						{
							if (a - 32 < 0)
							{
								a = 0;
							}
							else
							{
								a -= 32;
							}
						}
						break;


					case SDLK_r:
						isFuckingAroundWithColors = true;
						r += 32;
						break;

					case SDLK_w:
						//Cap if over 255
						if (a + 32 > 255)
						{
							a = 255;
						}
						//Increment otherwise
						else
						{
							a += 32;
						}
						break;


					case SDLK_a:
						isFuckingAroundWithTransparency = true;
						break;

					case SDLK_m:
						isFuckingAroundWithColors = false;
						isFuckingAroundWithTransparency = false;
						isTexture = true;
						isTryingToAnimateShit = true;


					default:
						isTexture = false;
						gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT];
						break;
					}


					if (isFuckingAroundWithColors) {
						SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
						SDL_RenderClear(gRenderer);

						gModulatedTexture.setColor(r, g, b);
						gModulatedTexture.render(0, 0);

						SDL_RenderPresent(gRenderer);
					}

					if (isFuckingAroundWithTransparency) {
						SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
						SDL_RenderClear(gRenderer);

						gBackgroundAlphaTexture.render(0, 0);
						gModulatedAlphaTexture.setAlpha(a);
						gModulatedAlphaTexture.render(0, 0);

						SDL_RenderPresent(gRenderer);

					}





				}
		}



		if (isTryingToAnimateShit)
		{
			SDL_RenderClear(gRenderer);

			SDL_Rect* currentClip = &gSpriteClips[frame / 4];
			gSpriteSheetAnimTexture.render((SCREEN_WIDTH - currentClip->w) / 2, (SCREEN_HEIGHT - currentClip->h)/2, currentClip);

			SDL_RenderPresent(gRenderer);

			++frame;

			if (frame / 4 >= WALKING_ANIMATION_FRAMES)
			{
				frame = 0;
			}
			//SDL_BlitSurface(gCurrentSurface, NULL, gScreenSurface, currentClip);
			//SDL_UpdateWindowSurface(gWindow);
		}

		else if (!isTexture) {
			SDL_Rect stretchRect;
			stretchRect.x = 0;
			stretchRect.y = 0;
			stretchRect.w = SCREEN_WIDTH;
			stretchRect.h = SCREEN_HEIGHT;
			//SDL_BlitScaled(gCurrentSurface, NULL, gScreenSurface, &stretchRect);
			//SDL_UpdateWindowSurface(gWindow);
		}
	}
	close();
	return 0;
}
