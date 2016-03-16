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
#include "ctclient.h"

#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <resolv.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdlib>

using namespace std;

int ctclient::create(string ip, int port){
	int s, error;
	struct sockaddr_in addr;

	if((s = socket(AF_INET,SOCK_STREAM,0))<0){
		cout<<"Error S01\n";
		return -1;
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	inet_aton(ip.c_str(),&addr.sin_addr);

	error = connect(s,(sockaddr*)&addr,sizeof(addr));
	if(error!=0){
		cout<<"Error S02\n";
		return -1;
	}
	
	socketid = s;
	
	return 0;
}

int ctclient::c_close(){
	shutdown(socketid, 2);
	close(socketid);
	
	return 0;
}
