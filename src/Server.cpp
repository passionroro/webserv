#include "Server.hpp"

Server::Server(unsigned int host, short port) : _port(port), _host(host)
{
}

Server::Server(Object & object)
{

    assignDefaultConfig(object);

	overwriteConfig(object);

}

void Server::assignDefaultConfig(Object & object) {

    _server_name = object.getString()["server_name"];
    _port = object.getInt()["port"];
    _clt_body_size = object.getInt()["client_max_body_size"];
    _auto_index = object.getBool()["auto_index"];

    _address = object.getArray()["address"].getString();
    _disabled_methods = object.getArray()["disabled_methods"].getString();

    _error_pages = object.getObject()["error_pages"].getString();

    std::vector<Object> tmp_location = object.getArray()["locations"].getObject();
    std::vector<Object>::iterator it;
    for (it = tmp_location.begin() ; it != tmp_location.end() ; it++) {
        _locations.push_back(it->getString());
    }

}

void	Server::overwriteConfig(Object & object) {

    (void)object;

}

/* FUNCTIONS */
int	Server::setup(void)
{
    _listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_listen_fd == -1)
    {
        std::cerr << "Error: socket" << std::endl;
        return (-1);
    }
    std::cout << "socket: " << _listen_fd << std::endl;
    _addr.sin_family = AF_INET;
    //_addr.sin_addr.s_addr = htonl(_host);
    _addr.sin_addr.s_addr = INADDR_ANY;
    _addr.sin_port = htons(_port);
    if (bind(_listen_fd, (saddr*)&_addr, sizeof(_addr)) == -1)
    {
        std::cerr << "Error: bind" << std::endl;
        return (-2);
    }
    if (listen(_listen_fd, SERVER_BACKLOG) == -1)
    {
        std::cerr << "Error: bind" << std::endl;
        return (-3);
    }
    return (0);
}

void	Server::accept(void)
{
    int	socket = ::accept(_listen_fd, NULL, NULL); // should we take client addr ?
    if (socket != -1)
        fcntl(socket, F_SETFL, O_NONBLOCK);
    else
        std::cerr << "Error: accept" << std::endl;
    _socket = socket;
}

int	Server::recv(void)
{
    int			tmp = 0;
    int			bytes_read = 0;
    char		buf[BUFSIZE];
    std::string	request;

    std::cout << "Webserv: recv" << std::endl;
    while ((tmp = ::recv(_socket, buf, BUFSIZE - 1, 0)) > 0)
    {
        request += buf;
        bytes_read += tmp;
    }
    buf[BUFSIZE - 1] = '\0';
    if (bytes_read == 0 || bytes_read == -1)
    {
        std::cerr << "Error: recv" << std::endl;
        return (-1);
    }
//    _response = Response(request, _locations);
    std::cout << "Request:" << std::endl << request << std::endl;
    return (0);
}

int	Server::send(void)
{
    std::string	str = _response.renderString();

    std::cout << "Webserv: send" << std::endl;
    //std::cout << "Response:" << std::endl << str << std::endl;
    if ((::send(_socket, str.c_str(), str.size(), 0)) < 0)
        return (-1);
    else
        return (0);
}

void	Server::close(void)
{
    std::cout << "Webserv: close" << std::endl;
    if (_socket > 0)
        ::close(_socket);
}

/* ACCESSORS */
unsigned int	Server::getHost(void) const { return _host; }
short			Server::getPort(void) const { return _port; }
int				Server::getListenFd(void) const { return _listen_fd; }
int				Server::getSocket(void) const { return _socket; }
