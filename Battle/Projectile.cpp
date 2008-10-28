#include "SDL/SDL.h"
#include "SDL/SDL_ttf.h"

#include "Projectile.h"

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

Projectile::Projectile() {
	speedx = 0;
	speedy = 0;
	distance_traveled = 0;
	hit = false;

	position = new SDL_Rect();
}

Projectile::Projectile(const char * sprite_file, SDL_Rect * clip) {
	speedx = 0;
	speedy = 0;
	distance_traveled = 0;
	max_distance = WINDOW_WIDTH >> 1;
	hit = false;

	this->clip = clip;
	load_images(sprite_file);

	position = new SDL_Rect();
	position->x = 0;
	position->y = 0;
	position->w = clip->w;
	position->h = clip->h;
}

Projectile::~Projectile() {
	free_images();

	delete position;
	delete clip;
}

void Projectile::show(SDL_Surface * screen) {
	SDL_Rect rect;

	rect.x = position->x;
	rect.y = position->y;

	SDL_BlitSurface(sprite, clip, screen, &rect);

	// If the player is going out the side of the screen, we want it to
	// appear on the other side.
	if(position->x >= WINDOW_WIDTH - clip->w) {
		rect.x = position->x - WINDOW_WIDTH;
		rect.y = position->y;
		SDL_BlitSurface(sprite, clip, screen, &rect);
	}
	if(position->x <= 0) {
		rect.x = position->x + WINDOW_WIDTH;
		rect.y = position->y;
		SDL_BlitSurface(sprite, clip, screen, &rect);
	}
}

void Projectile::load_images(const char * sprite_file) {
	SDL_Surface * loaded;
	Uint32 colorkey;
	
	loaded = SDL_LoadBMP(sprite_file);
	if(loaded == NULL) {

	}
	sprite = SDL_DisplayFormat(loaded);
	SDL_FreeSurface(loaded);

	colorkey = SDL_MapRGB(sprite->format, 0, 255, 255); 
	SDL_SetColorKey(sprite, SDL_SRCCOLORKEY, colorkey);
}

void Projectile::free_images() {
	SDL_FreeSurface(sprite);
}