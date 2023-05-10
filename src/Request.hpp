#ifndef REQUEST_HPP
# define REQUEST_HPP

# include "webserv.hpp"
# include "config/Config.hpp"


class Request {

public:
	
	Request();
	Request(std::string request, Server& server_conf);
	virtual ~Request();
	
	int 	parseRequest(std::string &Request);
//	bool	check_path(std::string s);
	int 	parseHeaders(std::string &Request);

	std::string	buildPath(void);

	std::string getStatus() const;
	std::string getBody() const;
	std::string getPath() const;
	void 		checkMethod();
	void 		checkPath();
	void		setStatus(std::string statusCode);

	std::map<std::string, std::string> getRequestHeaders(void) { return _request_headers; }

protected:

	std::vector< std::map<std::string, std::string> >	_locations;
	std::string		_path;

private:

	std::string 						_method;
	std::string 						_requestPath;
	std::string 						_version;
	std::map<std::string, std::string>	_request_headers;
	std::string							_request_body;
	std::string 						_status;
};
#endif
