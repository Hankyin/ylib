#pragma once

/**
 * @file httpclient.h
 * @author yin 
 * @brief http请求客户端。
 * @details url和http没有必然联系，在发起http请求前需要先使用URL等类解析相关信息，然后构造http请求。
 *          当然本类也提供了一些方便函数集成了url解析。
 * @version 0.1
 * @date 2021-05-20
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include "httpmessage.h"
#include "tcpsocket.h"

#include <string>
#include <stdexcept>

namespace ylib
{

    class HTTPClient
    {
    public:
        HTTPClient();
        ~HTTPClient();

        void connect(const std::string &ip, uint16_t port);
        void send_request(const HTTPRequestMsg &req, HTTPResponseMsg &resp);
        void close();

        void set_firstline_max_size(size_t si) { _firstline_max_size = si; }
        void set_header_max_size(size_t si) { _header_max_size = si; }

        //方便使用的几个简单接口
        static HTTPResponseMsg GET(const std::string &url);
        static HTTPResponseMsg POST(const std::string &url, const std::string &body);

    private:
 
        TCPSocket _socket;

        size_t _firstline_max_size = 4096;
        size_t _header_max_size = 4096;
        //0 断开
        //1 连接
        int _stat = 0;
    };

} // namespace ylib