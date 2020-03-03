#define MAX_FILE_BUFFER_SIZE 1024
#define MAX_TIME_BUFFER_SIZE 256

#include "response.h"
#include "boost/filesystem.hpp"
#include <mutex>

std::string HTTPResponse::startProcessing(std::string &method, std::string& document_root, std::string &uri, char &version) {
    std::string response_buffer = "HTTP/1.";
    response_buffer.push_back(version);
    response_buffer.push_back(' ');

    std::vector<header> headers;
    initHeaders(headers);

    std::string response_code;

    if (method == "GET" || method == "HEAD") {
        response_code = processMethod(method, document_root, uri, version, headers);
    } else {
        response_code = processUnknownMethod();
    }

    response_buffer += response_code + "\r\n";
    writeHeaders(method, response_code, uri, response_buffer, headers);

    return response_buffer;
}

std::string HTTPResponse::processMethod(std::string &method, std::string& document_root, std::string &uri, char &version, std::vector<header>& headers) {
    std::string full_path = document_root + uri;

    size_t last_slash_index = uri.find_last_of('/');

    if (last_slash_index == uri.length() - 1) {
        full_path += "index.html";
    }

    size_t query_index = full_path.find('?');

    if (query_index != std::string::npos) {
        full_path = full_path.substr(0, query_index);
    }

    size_t escape_index = uri.find("../");

    if (boost::filesystem::exists(full_path) && escape_index == std::string::npos) {
        size_t dot_position = full_path.find_last_of('.');
        std::string file_extension = full_path.substr(dot_position + 1, full_path.length() - dot_position);
        std::string content_type = getContentType(file_extension);

        headers.push_back(header {"Content-Type", content_type});
        headers.push_back(header {"Content-Length", std::to_string(boost::filesystem::file_size(full_path))});

        uri = full_path;
        return "200 OK";
    } else if (escape_index != std::string::npos || (last_slash_index == uri.length() - 1)) {
        return "403 Forbidden";
    }

    return "404 Not Found";
}

std::string HTTPResponse::processUnknownMethod() {
    return "405 Method Not Allowed";
}

void HTTPResponse::initHeaders(std::vector<header>& headers) {
    headers.push_back(header {"Server", "gdinx v.1.0.1"});
    headers.push_back(header {"Date", getDate()});
    headers.push_back(header {"Connection", "Closed"});
}

void HTTPResponse::writeHeaders(std::string& method, std::string& code, std::string& path, std::string& response_buffer, std::vector<header>& headers) {
    for (auto& header : headers) {
        response_buffer += header.key + ": " + header.value + "\r\n";
    }

    if (method == "HEAD") {
        response_buffer += "\r\n";
    }

    if (method == "GET" && code == "200 OK") {
        response_buffer += "\r\n";

        std::mutex mu;

        mu.lock();
        std::ifstream file(path.c_str(), std::ios::in | std::ios::binary);
        char file_buffer[MAX_FILE_BUFFER_SIZE];

        while (file.read(file_buffer, sizeof(file_buffer)).gcount() > 0) {
            response_buffer.append(file_buffer, file.gcount());
        }
        mu.unlock();
    }
}

std::string HTTPResponse::getDate() {
    std::time_t timer = std::time(nullptr);
    char buffer_time[MAX_TIME_BUFFER_SIZE];
    auto time_now = std::strftime(buffer_time, sizeof(buffer_time), "%a, %d %b %Y %H:%M:%S GMT", std::localtime(&timer));

    return std::string(buffer_time, time_now);
}

std::string HTTPResponse::getContentType(std::string& extension) {
    if (extension == "html") return "text/html";
    if (extension == "css") return "text/css";
    if (extension == "js") return "application/javascript";
    if (extension == "jpg") return "image/jpeg";
    if (extension == "jpeg") return "image/jpeg";
    if (extension == "png") return "image/png";
    if (extension == "gif") return "image/gif";
    if (extension == "swf") return "application/x-shockwave-flash";
    if (extension == "txt") return "text/plain";

    return "";
}
