#pragma once

#include <memory>
#include <unordered_map>

#include "BellHTTPServer.h"
#include "BerryBind.h"
#include "EventBus.h"
#include "WrappedSemaphore.h"
#include "civetweb.h"
#include "EuphContext.h"

namespace euph {
class HTTPDispatcher {
 private:
  const std::string TAG = "http";
  std::shared_ptr<euph::Context> ctx;

  // Signal used to wait for the HTTP response, while waiting for the scripting layer to respond
  std::unique_ptr<bell::WrappedSemaphore> responseSemaphore;

  // Looparound value for the bind id
  const size_t MAX_CONNECTION_BINDS = 100;

  // Timeout for the HTTP response, in milliseconds
  const int HTTP_RESPONSE_TIMEOUT = 5 * 1000;

  // Map used to store the mappings between the connection id (used in berry) to the mg_connection
  std::unordered_map<uint32_t, struct mg_connection*> bindConnections;

  // Used to keep track of the next bind id
  uint32_t nextBindId = 0;

  std::unique_ptr<bell::BellHTTPServer> server;

 public:
  HTTPDispatcher(std::shared_ptr<euph::Context> ctx);
  ~HTTPDispatcher();

  // HTTP methods supported by the dispatcher
  enum Method : uint8_t { GET = 0, POST = 1 };

  // Event used to notify the scripting layer of a new HTTP request
  class VmEvent : public Event {
   private:
    int handlerId;
    int connId;

   public:
    VmEvent(int handlerId, int connId) {
      this->eventType = EventType::VM_MAIN_EVENT;
      this->subType = "http_request";
    }
    ~VmEvent(){};

    berry::map toBerry() override {
      berry::map m;
      m["handler_id"] = this->handlerId;
      m["conn_id"] = this->connId;
      return m;
    }
  };

  // Initializes the dispatcher
  void initialize();

  // Registers berry bindings to the internal methods
  void setupBindings();

  // Methods bind to the scripting API
  void _registerHandler(int httpMethod, std::string path, int handlerId);
  void _writeResponse(int connId, std::string body, std::string contentType,
                      int statusCode);
  std::string _readBody(int connId);
  size_t _readContentLength(int connId);
};
}  // namespace euph