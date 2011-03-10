/* Copyright (c) 2010 maidsafe.net limited
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
    * Neither the name of the maidsafe.net limited nor the names of its
    contributors may be used to endorse or promote products derived from this
    software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef MAIDSAFE_DHT_TRANSPORT_TCP_CONNECTION_H_
#define MAIDSAFE_DHT_TRANSPORT_TCP_CONNECTION_H_

#ifdef __MSVC__
#pragma warning(disable:4996)
#endif
#include <memory>
#ifdef __MSVC__
#pragma warning(default:4996)
#endif

#include <string>
#include <vector>
#include "boost/asio/deadline_timer.hpp"
#include "boost/asio/io_service.hpp"
#include "boost/asio/ip/tcp.hpp"
#include "boost/asio/strand.hpp"
#include "maidsafe-dht/transport/transport.h"

namespace maidsafe {

namespace transport {

class TcpTransport;

class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
 public:
  TcpConnection(const std::shared_ptr<TcpTransport> &tcp_transport,
                const boost::asio::ip::tcp::endpoint &remote);
  ~TcpConnection();
  void Close();
  boost::asio::ip::tcp::socket &Socket();
  void StartReceiving();
  void Send(const std::string &data, const Timeout &timeout, bool is_response);

 private:
  TcpConnection(const TcpConnection&);
  TcpConnection &operator=(const TcpConnection&);
  void DoClose();
  void DoStartReceiving();
  void StartConnect();
  void StartRead();
  void StartWrite(DataSize msg_size);

  void CheckTimeout(const boost::system::error_code& ec);
  void HandleSize(const boost::system::error_code& ec);
  void HandleRead(const boost::system::error_code& ec);
  void HandleConnect(const boost::system::error_code& ec);
  void HandleWrite(const boost::system::error_code &ec);

  void DispatchMessage();

  std::weak_ptr<TcpTransport> transport_;
  boost::asio::io_service::strand strand_;
  boost::asio::ip::tcp::socket socket_;
  boost::asio::deadline_timer timer_;
  boost::asio::ip::tcp::endpoint remote_endpoint_;
  std::vector<unsigned char> size_buffer_, data_buffer_;
  Timeout timeout_for_response_;
};

}  // namespace transport

}  // namespace maidsafe

#endif  // MAIDSAFE_DHT_TRANSPORT_TCP_CONNECTION_H_
