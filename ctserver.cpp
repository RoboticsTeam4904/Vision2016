/*
  Copyright 2016 Carter Turnbaugh

  This file is part of Terca C++ Sockets.

  Terca C++ Sockets is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Terca C++ Sockets is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Terca C++ Sockets.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "ctserver.h"

#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <resolv.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdlib>

using namespace std;

int ctserver::create(int port){
		serversocketid = socket(AF_INET, SOCK_STREAM, 0);
	
		sockaddr_in serverAddr;
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_port = htons(port);
		serverAddr.sin_addr.s_addr = INADDR_ANY;
	
		bind(serversocketid, (struct sockaddr*)&serverAddr, sizeof(struct sockaddr));
		listen(serversocketid, 4);
	
		return 0;
}

int ctserver::getconn(){
	
		sockaddr_in clientAddr;
		socklen_t sin_size=sizeof(struct sockaddr_in);
		socketid = accept(serversocketid, (struct sockaddr*)&clientAddr, &sin_size);
	
		return 0;
}


int ctserver::c_close(){
		shutdown(socketid, 2);
		close(socketid);
	
		return 0;
}
