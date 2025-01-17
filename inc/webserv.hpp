#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <sys/socket.h>
# include <sys/types.h>
# include <sys/select.h>
# include <arpa/inet.h>
# include <stdlib.h>
# include <fcntl.h>
# include <unistd.h>

# include <string>
# include <cstring>
# include <iostream>
# include <fstream>
# include <sstream>

# include <vector>
# include <map>
# include <list>
# include <set>
# include <queue>

# define SERVER_BACKLOG 1000
# define BUFSIZE 50

# define DEFAULT "\033[0m"
# define BOLD "\033[1m"
# define RED "\033[31m"
# define GREEN "\033[32m"
# define LGREEN "\033[92m"
# define BLUE "\033[34m"

typedef std::vector<std::map<std::string, std::string> > Locations;
typedef std::vector<std::map<std::string, std::string> > Redirection;
typedef std::map<std::string, std::string> MapStr;

class	Server;
class	Config;
class	Request;
class	Response;
void    error(std::string msg);
std::string trim(std::string string, char to_erase);
std::vector<std::string>	split(std::string string, char delimiter);
#endif
