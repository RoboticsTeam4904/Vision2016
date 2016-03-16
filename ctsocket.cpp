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
#include "ctsocket.h"

#include <unistd.h>
#include <sys/socket.h>
#include <cstring>

using namespace std;

string ctsocket::c_read(){

		string data = "";
		bool reading = true;
		while(reading){
				char reccstr[9]; //Space for \0 termination character JIC
				bzero(reccstr, 9);
				int retval = read(socketid, reccstr, 9);
				for(int i = 1; i < 9; i++){
						data += reccstr[i];
				}
				if(reccstr[0] == 'e') reading = false;
		}

		return data;
}

int ctsocket::c_write(string data){

        if(data.length() > 8){
				while(data.length() > 0){
						string temp = data.substr(0, 8);
						data.erase(0, 8);
						char msg[9];
						bzero(msg, 9);
						if(data.length() > 0) msg[0] = 'c';
						else msg[0] = 'e';
						for(int i = 0; i < temp.length(); i++){
								msg[i+1] = temp[i];
						}
						send(socketid, msg, sizeof(msg), 0);
				}
		}
		else{
				char msg[9];
				bzero(msg, 9);
				msg[0] = 'e';
				for(int i = 0; i < data.length(); i++){
						msg[i+1] = data[i];
				}
				send(socketid, msg, sizeof(msg), 0);
		}
		
		return 0;
}
