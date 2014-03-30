#pragma once

#include <string>

#include "AbstractRestBase.h"

#define SKIP_TOKEN_PROCESS


namespace rest {

class ClientToken : public AbstractRestBase {
public:

	ClientToken();

	std::string get();

};

}