#include "Response.hpp"
#include "MimeTypes.hpp"
#include <dirent.h>

Response::Response(void)
{
}

Response::Response(std::string request, Server& server_conf) : Request(request, server_conf)
{

	if (getStatus() != "200")
	{
		_path = "home/www/error_404.html";
		//std::cout << "new path is error_404" << std::endl;
		_locIndex = _locations.end();
	}

	if (_locIndex != _locations.end() && (*_locIndex)["bin"] != "")
		cgi(server_conf);
	else if (_isDir)
		directoryListing();
	else if(_method == "POST")
		postMethod();
	else if(_method == "DELETE")
		deleteMethod();
	else
		readStaticPage();
	
	createHeaders();
	_status_code = getStatus();
	_status_text = getStatusText();
	return;
}

Response::~Response(void)
{
}

// create a file in /images
// read into a buffer
// append into the new file

std::string Response::getUploadFilename() {

    std::string filename;
    size_t      pos;
    size_t      end_pos;

    filename.clear();
    pos = _request_body.find(std::string("filename")) ;
    if (pos != std::string::npos) {
        pos += 10;
        end_pos = _request_body.find('\"', pos);
        if (end_pos != std::string::npos)
            filename = _request_body.substr(pos, end_pos - pos);
    }

    return filename;
}

void Response::eraseBodyBoundaries() {

    std::string::size_type bodyStart = 0;
    bodyStart = _request_body.find(std::string("filename"), bodyStart);
    bodyStart = _request_body.find(std::string("\r\n\r\n"), bodyStart);
    _request_body.erase(0, bodyStart + 4);

    std::string::size_type bodyEnd = _request_body.find(std::string("--"));
    if (bodyEnd != std::string::npos)
        _request_body.erase(bodyEnd, _request_body.size());
	_request_body.erase(_request_body.find_last_of("\r\n") - 1, 2);
}

void Response::uploadFile() {

    Locations::iterator upload;
    std::string filename;
    size_t      max_body_size;
    size_t      content_length;

    // Get config file attributes for the /images folder
    for (upload = _locations.begin(); upload != _locations.end(); upload++) {
        if ((*upload)["path"] == "/images") {
            break ;
        }
    }

    if (upload == _locations.end())
        return ;

    max_body_size = std::stoi((*upload)["client_max_body_size"]);
    content_length = std::stoi(_request_headers["Content-Length"]);
//    if (content_length > max_body_size) {
//        std::cerr << "Max upload file is " << max_body_size << "MB." << std::endl;
//        return ;
//    }

    // File creation
    filename = getUploadFilename();
    if (filename.empty()) {
		std::cerr << "Failure opening upload file." << std::endl;
        return ;
	}

    std::string		path = (*upload)["root"] + (*upload)["path"] + "/" + filename;
    std::ofstream	ofs(path, std::fstream::out | std::fstream::binary);

    if (!ofs.good() || !ofs.is_open()) {
        std::cerr << "Failure opening file at " << path << std::endl;
        return ;
    }

    eraseBodyBoundaries();

    // Write to file
    // 1
//    ofs.write(_request_body.c_str(), _request_body.size());

    // 2
    std::string             tmpBody = _request_body;
    std::string::size_type  bodySize = tmpBody.find(std::string("\n"));
	while (bodySize != std::string::npos) {
        ofs.write(tmpBody.c_str(), bodySize);
        tmpBody.erase(0, bodySize);
        bodySize = tmpBody.find(std::string("\n"));
		if (bodySize != std::string::npos)
			bodySize +=1;
		else
			break;
		
    }
    ofs.write(tmpBody.c_str(), tmpBody.size());
    ofs.close();
	(void) content_length;
	(void) max_body_size;
}

void Response::postMethod()
{
	if (_request_headers["Content-Type"].compare(0, 19, "multipart/form-data") == 0)
		uploadFile();
    else {
		std::fstream inputstream;

		inputstream.open(_path,std::fstream::app);
		inputstream << _request_body;
    }

}


void Response::deleteMethod()
{
	if ((remove(_path.c_str()) == 0))
		_status_code = ("204");
	else
	{
		_status_text = "Delete failed";
		_status_code = ("200");
	}
}

void	Response::getContentType(void)
{
	MimeTypes	mt;

	if (mt.getMap().find("html") == mt.getMap().end())
		return ;
	
	std::string	extension;

	size_t	start = _path.find_last_of('.');
	if (start == _path.npos || start == _path.size() - 1)
		return ;
	extension = _path.substr(start + 1);
	_response_headers.insert(std::make_pair("Content-Type", mt.getMap()[extension]));





	//std::cout << "mimetype: " << mt.getMap()["html"] << std::endl;
	//_response_headers.insert(std::make_pair("Content-Type", mt.getMap()["html"]));
}

void	Response::getContentLength(void)
{
	std::stringstream	sstream;
	std::string	len;

	sstream << _response_body.size();
	sstream >> len;

	_response_headers.insert(std::make_pair("Content-Length", len));
}

std::string	Response::getStatusText(void)
{
	MapStr	texts;

	texts.insert(std::make_pair("200", "OK"));
	texts.insert(std::make_pair("201", "Created"));
	texts.insert(std::make_pair("301", "Moved Permanently"));
	texts.insert(std::make_pair("400", "Bad Request"));
	texts.insert(std::make_pair("404", "Not found"));
	texts.insert(std::make_pair("405", "Method Not Allowed"));
	texts.insert(std::make_pair("409", "Conflict"));
	texts.insert(std::make_pair("500", "Internal Server Error"));
	texts.insert(std::make_pair("501", "Not Implemented"));

	return (texts[getStatus()]);

}

void	Response::createHeaders(void)
{
	int size;
	char buf[100];
	time_t now = time(0);
	struct tm tm = *gmtime(&now);
	tm.tm_zone = (char *)"GMT";
	size = ::strftime(buf, sizeof buf, "%a, %d %b %Y %H:%M:%S %Z", &tm);
	_response_headers.insert(std::make_pair("Server", "42-WebServ"));
	_response_headers.insert(std::pair<std::string, std::string>("Date", buf));

	getContentType();
	getContentLength();
	(void)size;
}

void	Response::appendHeaders(std::string & str)
{
	for (MapStr::iterator it = _response_headers.begin() ;
		it != _response_headers.end() ; it++)
	{
		str.append(it->first + ": " + it->second + "\r\n");
	}
	str += "\r\n";
}

int	Response::readStaticPage(void)
{
	std::ifstream	file;
	std::stringstream	sstream;

	file.open(_path.c_str(), std::fstream::in);
	if (file.is_open() == true)
	{
		sstream << file.rdbuf();
		_response_body = sstream.str();
		file.close();
		//std::cout << "PAGE:" << std::endl << _page << std::endl;
		return (200);
	}
	else
	{
		std::cout << "bro nul" << std::endl;
		//todo default error page 404
		return (404);
	}
}

void	Response::cgi(Server& server_conf)
{
	std::cout << "cgi called" << std::endl;

	Cgi	cgi(*this, server_conf);
	_response_body = cgi.getRes();
}

void	Response::directoryListing(void)
{
	std::cout << "directory listing called" << std::endl;

	//std::string	path = ".";
	DIR*	dir;
	struct dirent*				ent;
	std::vector<struct dirent>	entries;

	dir = opendir(_path.c_str());
	if (!dir)
		return ;
	while ((ent = readdir(dir)) != NULL)
		entries.push_back(*ent);
	closedir(dir);

	std::sort(entries.begin(), entries.end(), comp);

	std::string	html("<!DOCTYPE html>\r\n"
					"<html lang=\"en\">\r\n"
					"<head>\r\n"
					"</head>\r\n"
					"<body>\r\n"
					"<h1>Index of " + getRequestPath() + "</h1>\r\n");

	for (unsigned long i = 0 ; i < entries.size() ; i++)
	{
		std::string line;

		line += "<a href=\"";
		line += getRequestPath();
		if (getRequestPath()[getRequestPath().size() - 1] != '/')
			line += '/';
		line += entries[i].d_name;
		line += "\">";
		line += entries[i].d_name;
		line += "</a></br>\r\n";
		html += line;
	}


	html += "</body>\r\n";
	_response_body += html;
}


std::string	Response::renderString(void)
{
	std::string	str;
	_response_head = "HTTP/1.1 " + _status_code + " " + _status_text  + "\r\n";
	appendHeaders(_response_head);
	str = _response_head + _response_body;
	return (str);
}

bool	comp(struct dirent x, struct dirent y)
{
	return (std::strcmp(x.d_name, y.d_name) > 0);
}
