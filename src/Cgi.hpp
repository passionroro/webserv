#ifndef CGI_HPP
# define CGI_HPP

# include "webserv.hpp"
# include "Request.hpp"

class Cgi {

public:
	
	Cgi(Request& request, Server& server_conf);
	virtual ~Cgi(void);

	void	initEnv(Server& conf);
	void	execute(void);

	char	**getEnvv(void) const;

	std::string	readRes(int fd);

	std::string	getRes(void) const { return _res; }
	
private:

	Request	_r;

	std::map<std::string, std::string>	_headers;
	std::map<std::string, std::string>	_env;
	std::string	_body;
	std::string	_res;

private:

	Cgi(void);

};

#endif