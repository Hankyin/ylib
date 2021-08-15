#include "httpclient.h"
#include "network.h"
#include "httpmessage.h"
#include "httpexcept.h"

#include "core/stringhelper.h"

#include <iostream>

namespace ylib
{

    HTTPResponseMsg HTTPClient::GET(const std::string &url_str)
    {
        std::string http_ip;
        URL _url(url_str);
        if (is_ipv4(_url.host))
        {
            http_ip = _url.host;
        }
        else
        {
            http_ip = resolve_hostname(_url.host);
        }
        HTTPClient cli;
        HTTPRequestMsg req;
        HTTPResponseMsg resp;
        req.method = HTTPMethod::GET;
        req.path = _url.path;
        cli.connect(http_ip, _url.port);
        cli.send_request(req, resp);
        return resp;
    }

    HTTPResponseMsg HTTPClient::POST(const std::string &url, const std::string &body)
    {
    }

    HTTPClient::HTTPClient()
    {
    }

    HTTPClient::~HTTPClient()
    {
    }

    void HTTPClient::connect(const std::string &ip, uint16_t port)
    {

        _socket.connect(ip, port);
        _stat = 1;
    }

    void HTTPClient::close()
    {
        _socket.close();
        _stat = 0;
    }

    void HTTPClient::send_request(const HTTPRequestMsg &req, HTTPResponseMsg &resp)
    {
        std::string head_line;
        std::string header_part;
        head_line = HTTPMethod_to_str(req.method);
        head_line += ' ';
        head_line += req.path;
        head_line += " HTTP/1.1\n";

        for (auto &&kv : req.headers)
        {
            std::string h;
            h = kv.first;
            h += ": ";
            h += kv.second;
            h += "\n";
            header_part += h;
        }
        header_part.push_back('\n');

        _socket.write_str(head_line);
        _socket.write_str(header_part);
        _socket.write_str(req.body);

        std::string http_buf;

        bool paser_working = true;
        HTTPRespHeaderParser rp(resp);

        while (paser_working)
        {
            std::string pack = _socket.read(4096);
            rp.buffer_add(pack);
            if (rp.parser())
            {
                //头部解析完成，读取body
                std::string LEN = "Content-Length";
                size_t body_len = 0;
                if (resp.headers.find(LEN) != resp.headers.end())
                {
                    long blen = ::atol(resp.headers[LEN].c_str());
                    if (blen < 0)
                    {
                        throw HTTPFormatException("HTTP body length error", resp.headers[LEN]);
                    }
                    body_len = blen;
                }

                resp.body = rp.get_left_str(); //读取剩余的全部字符串
                size_t left_len = body_len - resp.body.size();
                if (left_len > 0)
                {
                    std::string left_body_content = _socket.readn(left_len);
                    resp.body += left_body_content;
                }
                break;
            }
        }
    }

} // namespace ylib
