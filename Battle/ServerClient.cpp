/**
 * The SDL_Net code could not have been so easily written without the examples provided by
 *  Jon C. Atkins, found here: http://jcatki.no-ip.org:8080/SDL_net/
 */

#include "ServerClient.h"

#include "ClientNetworkMultiplayer.h"
#include "Level.h"

#include "log.h"

ServerClient::ServerClient()
	: is_connected_(false),
	  currentState_(ServerClient::State::INITIALIZING),
	  host_("localhost"),
	  port_((Uint16)1099),
	  CommandProcessor(0),
	  lag(INITIAL_LAG_TESTS),
	  game_(NULL),
	  level_(NULL),
	  my_id_(0x00)
{
}

ServerClient::~ServerClient()
{
	SDLNet_Quit();
}

void ServerClient::connect(ClientNetworkMultiplayer &game, Level &level, Player &player)
{
	if (is_connected_)
		return;

	game_ = &game;
	level_ = &level;
	player_ = &player;

	log(format("CONNECTING TO %s:%d...",host_.c_str(),port_), Logger::Priority::CONSOLE);

	/* initialize SDL */
	if(SDL_Init(0)==-1)
	{
		log(format("SDL_Init: %s\n",SDL_GetError()), Logger::Priority::ERROR);
		return;
	}

	/* initialize SDL_net */
	if(SDLNet_Init()==-1)
	{
		log(format("SDLNet_Init: %s\n",SDLNet_GetError()), Logger::Priority::ERROR);
		return;
	}

	set=SDLNet_AllocSocketSet(1);
	if(!set)
	{
		log(format("SDLNet_AllocSocketSet: %s\n", SDLNet_GetError()), Logger::Priority::ERROR);
		return;
	}


	/* Resolve the argument into an IPaddress type */
	if(SDLNet_ResolveHost(&ip,host_.c_str(),port_)==-1)
	{
		log(format("SDLNet_ResolveHost: %s\n",SDLNet_GetError()), Logger::Priority::ERROR);
		return;
	}

	/* open the server socket */
	sock=SDLNet_TCP_Open(&ip);
	if(!sock)
	{
		log(format("SDLNet_TCP_Open: %s\n",SDLNet_GetError()), Logger::Priority::ERROR);
		return;
	}
	
	if(SDLNet_TCP_AddSocket(set,sock)==-1)
	{
		log(format("SDLNet_TCP_AddSocket: %s\n",SDLNet_GetError()), Logger::Priority::ERROR);
		return;
	}
	
	

	log(format("CONNECTION SUCCESFUL",host_.c_str(),port_), Logger::Priority::CONSOLE);

	set_socket(sock);

	is_connected_ = true;
}

void ServerClient::poll(short test)
{
	if (!is_connected_)
		return;

	/* we poll keyboard every 1/10th of a second...simpler than threads */
	/* this is fine for a text application */
		
	/* wait on the socket for 1/10th of a second for data */
	int numready=SDLNet_CheckSockets(set, 0);
	if(numready==-1)
	{
		printf("SDLNet_CheckSockets: %s\n",SDLNet_GetError());
		return;
	}

	/* check to see if the server sent us data */
	if(numready && SDLNet_SocketReady(sock))
	{
		char buffer[512] = {0x00};
		int bytesReceived = SDLNet_TCP_Recv(sock, buffer, sizeof(buffer));

		if (bytesReceived > 0)
		{
			receive(bytesReceived, buffer);
			while (parse())
					;
		}
		else
		{
			// Close the old socket, even if it's dead... 
			SDLNet_TCP_Close(sock);
				
			printf("Disconnected from server\n");
			is_connected_ = false;
			return;
		}

	}
	

}

#include "Commands.hpp"
#include "log.h"
void ServerClient::send(Command &command)
{
	if (!is_connected_)
		return;

	char type = command.getType();
	result = SDLNet_TCP_Send(sock, &type, sizeof(char));

	if(result < sizeof(char)) {
		if(SDLNet_GetError() && strlen(SDLNet_GetError())) /* sometimes blank! */
			printf("SDLNet_TCP_Send: %s\n", SDLNet_GetError());
		return;
	}

	result = SDLNet_TCP_Send(sock, command.getData(), command.getDataLen());

	if(result < sizeof(char)) {
		if(SDLNet_GetError() && strlen(SDLNet_GetError())) /* sometimes blank! */
			printf("SDLNet_TCP_Send: %s\n", SDLNet_GetError());
		return;
	}
}
void ServerClient::test()
{
	CommandPing command;
	command.data.time = SDL_GetTicks();

	this->send(command);
}


bool ServerClient::process(std::unique_ptr<Command> command)
{
	

	switch (command.get()->getType())
	{
		case Command::Types::Ping:
			process(dynamic_cast<CommandPing *>(command.get()));
			break;
		case Command::Types::Pong:
			process(dynamic_cast<CommandPong *>(command.get()));
			break;
		case Command::Types::SetLevel:
			process(dynamic_cast<CommandSetLevel *>(command.get()));
			break;
		case Command::Types::RequestCharacter:
			process(dynamic_cast<CommandRequestCharacter *>(command.get()));
			break;
		//case Command::Types::SetCharacter:
			//process(dynamic_cast<CommandSetCharacter *>(command.get()));
			//break;
		case Command::Types::SetPlayerData:
			process(dynamic_cast<CommandSetPlayerData *>(command.get()));
			break;
		case Command::Types::AddPlayer:
			process(dynamic_cast<CommandAddPlayer *>(command.get()));
			break;
		default:
			log(format("received command with type: %d", command.get()->getType()), Logger::Priority::CONSOLE);
	}

	return true;
}

bool ServerClient::process(CommandPing *command)
{
	CommandPong reply;
	reply.data.time = SDL_GetTicks();

	this->send(reply);

	log(format("PING? PONG!"), Logger::Priority::CONSOLE);

	return true;
}

bool ServerClient::process(CommandPong *command)
{
	float measuredLag = static_cast<float>((SDL_GetTicks() - command->data.time) / 2.0);
	
	lag.add(measuredLag);

	log(format("LAG %f AVG=%f", measuredLag, lag.avg()), Logger::Priority::CONSOLE);
	return true;
}


bool ServerClient::process(CommandSetLevel *command)
{
	my_id_ = command->data.your_id;

	// Default a client gets player with number zero
	player_->number = my_id_;

	level_->load(level_util::get_filename_by_name(command->data.level).c_str());
			
	game_->set_level(level_);
	
	log(format("my id is: %d load level %s!", command->data.your_id, command->data.level), Logger::Priority::CONSOLE);

	setState(ServerClient::State::INITIALIZED);

	return true;
}

bool ServerClient::process(CommandRequestCharacter *command)
{
	log(format("SERVER REQUESTED CHARACTER"), Logger::Priority::CONSOLE);

	CommandSetCharacter response;
	response.data.time = command->data.time;
	// response.data.nickname
	response.data.character = player_->character;

	this->send(response);

	return true;
}


bool ServerClient::process(CommandSetPlayerData *command)
{
	log(format("Server send player data"), Logger::Priority::DEBUG);

	if (my_id_ == command->data.client_id)
	{
		player_->position->x = command->data.x;
		player_->position->y = command->data.y;
		//player_util::set_player_data(player_, *command);
	}
	else
	{
		auto &players = *(game_->players);
		for (auto i=players.begin(); i!=players.end(); i++)
		{
			auto &otherplayer = **i;

			if (otherplayer.number == command->data.client_id)
			{
				player_util::set_player_data(otherplayer, *command);
			}
		}
	}

	return true;
}

bool ServerClient::process(CommandAddPlayer *command)
{
	Player *otherplayer = new Player(command->data.character, command->data.client_id);
	GameInputStub *playerinput = new GameInputStub();
	otherplayer->input = playerinput;

	otherplayer->position->x = command->data.x;
	otherplayer->position->y = command->data.y;
	otherplayer->set_sprite((command->data.facingRight ? SPR_R : SPR_L));
	otherplayer->reset();
			
	game_->add_player(otherplayer);
	return true;
}