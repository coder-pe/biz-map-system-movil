// Archivo: http_client.h (interno, no parte de la API pública)
// Cliente HTTP usando cpp-httplib
#pragma once

#include <string>
#include <map>
#include <functional>
#include <memory>
#include "../third_party/httplib.h"

namespace bizmap {
namespace internal {

/**
 * Cliente HTTP interno para comunicación con el backend
 * Wrapper alrededor de cpp-httplib
 */
class HttpClient {
public:
    using Headers = std::map<std::string, std::string>;
    using ResponseCallback = std::function<void(int status, const std::string& body)>;
    using ErrorCallback = std::function<void(const std::string& error)>;

    explicit HttpClient(const std::string& host);
    ~HttpClient();

    /**
     * Realiza una petición GET
     */
    void get(
        const std::string& path,
        const Headers& headers,
        ResponseCallback on_response,
        ErrorCallback on_error
    );

    /**
     * Realiza una petición POST
     */
    void post(
        const std::string& path,
        const std::string& body,
        const Headers& headers,
        ResponseCallback on_response,
        ErrorCallback on_error
    );

    /**
     * Realiza una petición PUT
     */
    void put(
        const std::string& path,
        const std::string& body,
        const Headers& headers,
        ResponseCallback on_response,
        ErrorCallback on_error
    );

    /**
     * Realiza una petición DELETE
     */
    void del(
        const std::string& path,
        const Headers& headers,
        ResponseCallback on_response,
        ErrorCallback on_error
    );

    /**
     * Establece el timeout para las peticiones
     */
    void setTimeout(int seconds);

private:
    std::string host_;
    int port_;
    std::string scheme_; // http o https
    std::unique_ptr<httplib::Client> client_;
    int timeout_seconds_;

    void parseHost(const std::string& host);
    void ensureClient();
};

} // namespace internal
} // namespace bizmap
