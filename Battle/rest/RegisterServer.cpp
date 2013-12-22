#include "RegisterServer.h"

#include <iostream>
#include <cstring>
#include "util/json.h"
#include "Server.h"

namespace rest {

RegisterServer::RegisterServer(const std::string &serverToken)
    : serverToken_(serverToken)
{
}

std::string RegisterServer::put()
{
	json::Object postData;
	postData["servername"] = Server::getInstance().getName();
	// Host will be determined by the API server for now
	//  postData["host"] = Server::getInstance().getHost();
	postData["port"] = Server::getInstance().getPort();
	postData["level"] = Server::getInstance().getLevelName();
	postData["required_players"] = 2;
	
	std::cout << "post data generated is; " << json::Serialize(postData) << std::endl;
	json::Object obj = request("PUT", "http://battle.cppse.nl/server", serverToken_, json::Serialize(postData));
	std::string tokenString(obj["random_string"]);

	return tokenString;
}

}