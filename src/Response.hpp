#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "webserv.hpp"
# include "Request.hpp"

class	Response : public Request {

public:

	Response(void);
	Response(std::string request, Server& server_conf);
	virtual	~Response(void);

	void	getHeaders(void);

	void	createHeaders(void);

	std::string	buildPath(void);

	void		appendHeaders(std::string & str);
	int			readStaticPage(void);
	std::string	renderString(void);

private:

	std::string	_status_code;
	std::string	_status_text;
	
	std::map<std::string, std::string>	_response_headers;

	std::string	_response_body;

};

#endif
