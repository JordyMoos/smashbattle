#pragma once

namespace network {
class CommandGeneratePowerup;
}

class AmmoPowerUp : public GameplayObject {
public:
	AmmoPowerUp(SDL_Surface * surface, SDL_Rect * clip, SDL_Rect * position, int ammo, Main &main);
	~AmmoPowerUp();

	virtual void move(Level * level);
	virtual void process();

	virtual void hit_player(Player * player);
	virtual void hit_npc(NPC * npc);

	virtual void draw(SDL_Surface * screen, int frames_processed = 0);

	virtual void copyTo(network::CommandGeneratePowerup &powerup);
	
	SDL_Surface * surface;
	SDL_Rect * clip;
	int ammo;

	Main &main_;
};
