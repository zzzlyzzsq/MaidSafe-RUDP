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

#include "maidsafe/transport/message_handler.h"
#include "boost/lexical_cast.hpp"
#ifdef __MSVC__
#  pragma warning(push)
#  pragma warning(disable: 4127 4244 4267)
#endif
#include "maidsafe/transport/transport.pb.h"
#ifdef __MSVC__
#  pragma warning(pop)
#endif
#include "maidsafe/transport/log.h"


namespace maidsafe {

namespace transport {

void MessageHandler::OnMessageReceived(const std::string &request,
                                       const Info &info,
                                       std::string *response,
                                       Timeout *timeout) {
  if (request.empty())
    return;
  SecurityType security_type = request.at(0);
  std::string serialised_message(request.substr(1));
  protobuf::WrapperMessage wrapper;
  if (wrapper.ParseFromString(serialised_message) && wrapper.IsInitialized()) {
    ProcessSerialisedMessage(wrapper.msg_type(), wrapper.payload(),
                             security_type, wrapper.message_signature(),
                             info, response, timeout);
  }
}

bool MessageHandler::UnwrapWrapperMessage(const std::string& serialised_message,
                                          int* msg_type,
                                          std::string* payload,
                                          std::string* message_signature) {
  protobuf::WrapperMessage wrapper;
  if (wrapper.ParseFromString(serialised_message) && wrapper.IsInitialized()) {
    *msg_type = wrapper.msg_type();
    *payload = wrapper.payload();
    *message_signature = wrapper.message_signature();
    return true;
  } else {
    return false;
  }
}

std::string MessageHandler::WrapWrapperMessage(const int& msg_type,
    const std::string& payload, const std::string& message_signature) {
  protobuf::WrapperMessage wrapper;
  wrapper.set_msg_type(msg_type);
  wrapper.set_payload(payload);
  if (!message_signature.empty())
    wrapper.set_message_signature(message_signature);
  return wrapper.SerializeAsString();
}

void MessageHandler::OnError(const TransportCondition &transport_condition,
                             const Endpoint &remote_endpoint) {
  (*on_error_)(transport_condition, remote_endpoint);
}

std::string MessageHandler::WrapMessage(
    const protobuf::ManagedEndpointMessage &msg) {
  if (!msg.IsInitialized())
    return "";
  return MakeSerialisedWrapperMessage(kManagedEndpointMessage,
                                      msg.SerializeAsString(), kNone, "");
}

std::string MessageHandler::WrapMessage(
    const protobuf::NatDetectionRequest &msg) {
  if (!msg.IsInitialized())
    return "";
  return MakeSerialisedWrapperMessage(kNatDetectionRequest,
                                      msg.SerializeAsString(), kNone, "");
}

std::string MessageHandler::WrapMessage(
    const protobuf::NatDetectionResponse &msg) {
  if (!msg.IsInitialized())
    return "";
  return MakeSerialisedWrapperMessage(kNatDetectionResponse,
                                      msg.SerializeAsString(), kNone, "");
}

std::string MessageHandler::WrapMessage(
    const protobuf::ProxyConnectRequest &msg) {
  if (!msg.IsInitialized())
    return "";
  return MakeSerialisedWrapperMessage(kProxyConnectRequest,
                                      msg.SerializeAsString(), kNone, "");
}

std::string MessageHandler::WrapMessage(
    const protobuf::ProxyConnectResponse &msg) {
  if (!msg.IsInitialized())
    return "";
  return MakeSerialisedWrapperMessage(kProxyConnectResponse,
                                      msg.SerializeAsString(), kNone, "");
}

std::string MessageHandler::WrapMessage(
    const protobuf::ConnectRequest &msg) {
  if (!msg.IsInitialized())
    return "";
  return MakeSerialisedWrapperMessage(kConnectRequest,
                                      msg.SerializeAsString(), kNone, "");
}

std::string MessageHandler::WrapMessage(
    const protobuf::ConnectResponse &msg) {
  if (!msg.IsInitialized())
    return "";
  return MakeSerialisedWrapperMessage(kConnectResponse,
                                      msg.SerializeAsString(), kNone, "");
}

std::string MessageHandler::WrapMessage(
    const protobuf::ForwardRendezvousRequest &msg) {
  if (!msg.IsInitialized())
    return "";
  return MakeSerialisedWrapperMessage(kForwardRendezvousRequest,
                                      msg.SerializeAsString(), kNone, "");
}

std::string MessageHandler::WrapMessage(
    const protobuf::ForwardRendezvousResponse &msg) {
  if (!msg.IsInitialized())
    return "";
  return MakeSerialisedWrapperMessage(kForwardRendezvousResponse,
                                      msg.SerializeAsString(), kNone, "");
}

std::string MessageHandler::WrapMessage(
    const protobuf::RendezvousRequest &msg) {
  if (!msg.IsInitialized())
    return "";
  return MakeSerialisedWrapperMessage(kRendezvousRequest,
                                      msg.SerializeAsString(), kNone, "");
}

std::string MessageHandler::WrapMessage(
    const protobuf::RendezvousAcknowledgement &msg) {
  if (!msg.IsInitialized())
    return "";
  return MakeSerialisedWrapperMessage(kRendezvousAcknowledgement,
                                      msg.SerializeAsString(), kNone, "");
}

void MessageHandler::ProcessSerialisedMessage(
    const int &message_type,
    const std::string &payload,
    const SecurityType &/*security_type*/,
    const std::string &/*message_signature*/,
    const Info & info,
    std::string *message_response,
    Timeout *timeout) {
  message_response->clear();
  *timeout = kImmediateTimeout;

  switch (message_type) {
    case kManagedEndpointMessage: {
      protobuf::ManagedEndpointMessage request;
      if (request.ParseFromString(payload) && request.IsInitialized()) {
        protobuf::ManagedEndpointMessage response;
        (*on_managed_endpoint_message_)(request, &response, timeout);
        *message_response = WrapMessage(response);
      }
      break;
    }
    case kNatDetectionRequest: {
      protobuf::NatDetectionRequest request;
      if (request.ParseFromString(payload) && request.IsInitialized()) {
        protobuf::NatDetectionResponse nat_detection_response;
        protobuf::RendezvousRequest rendezvous_request;
        (*on_nat_detection_request_)(info, request, &nat_detection_response,
                                     &rendezvous_request, timeout);
        if (nat_detection_response.IsInitialized()) {
          *message_response = WrapMessage(nat_detection_response);
        } else if (rendezvous_request.IsInitialized()) {
          *message_response = WrapMessage(rendezvous_request);
        }
      }
      break;
    }
    case kNatDetectionResponse: {
      protobuf::NatDetectionResponse response;
      if (response.ParseFromString(payload) && response.IsInitialized())
        (*on_nat_detection_response_)(response);
      break;
    }
    case kProxyConnectRequest: {
      protobuf::ProxyConnectRequest request;
      if (request.ParseFromString(payload) && request.IsInitialized()) {
        protobuf::ProxyConnectResponse response;
        (*on_proxy_connect_request_)(info, request, &response, timeout);
        *message_response = WrapMessage(response);
      }
      break;
    }
    case kProxyConnectResponse: {
      protobuf::ProxyConnectResponse response;
      if (response.ParseFromString(payload) && response.IsInitialized())
        (*on_proxy_connect_response_)(response);
      break;
    }
    case kForwardRendezvousRequest: {
      protobuf::ForwardRendezvousRequest request;
      if (request.ParseFromString(payload) && request.IsInitialized()) {
        protobuf::ForwardRendezvousResponse response;
        (*on_forward_rendezvous_request_)(request, &response, timeout);
        *message_response = WrapMessage(response);
      }
      break;
    }
    case kForwardRendezvousResponse: {
      protobuf::ForwardRendezvousResponse response;
      if (response.ParseFromString(payload) && response.IsInitialized())
        (*on_forward_rendezvous_response_)(response);
      break;
    }
    case kRendezvousRequest: {
      protobuf::RendezvousRequest request;
      if (request.ParseFromString(payload) && request.IsInitialized())
        (*on_rendezvous_request_)(request);
      break;
    }
    case kRendezvousAcknowledgement: {
      protobuf::RendezvousAcknowledgement acknowledgement;
      if (acknowledgement.ParseFromString(payload) &&
          acknowledgement.IsInitialized())
        (*on_rendezvous_acknowledgement_)(acknowledgement);
      break;
    }
  }
}

std::string MessageHandler::MakeSerialisedWrapperMessage(
    const int &message_type,
    const std::string &payload,
    SecurityType /*security_type*/,
    const std::string &/*recipient_public_key*/) {
  std::string final_message(1, kNone);
  final_message += WrapWrapperMessage(message_type, payload, "");
  return final_message;
}

}  // namespace transport

}  // namespace maidsafe
