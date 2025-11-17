// Archivo: http_client.cpp
#include "http_client.h"
#include <stdexcept>
#include <regex>
#include <sstream>

namespace bizmap {
namespace internal {

HttpClient::HttpClient(const std::string& host)
    : timeout_seconds_(30)
{
    parseHost(host);
    ensureClient();
}

HttpClient::~HttpClient() {
}

void HttpClient::parseHost(const std::string& host) {
    // Regex para parsear URL: scheme://host:port
    std::regex url_regex(R"(^(https?)://([^:/]+)(?::(\d+))?(.*)$)");
    std::smatch matches;

    if (std::regex_match(host, matches, url_regex)) {
        scheme_ = matches[1].str();
        host_ = matches[2].str();

        if (matches[3].matched) {
            port_ = std::stoi(matches[3].str());
        } else {
            port_ = (scheme_ == "https") ? 443 : 80;
        }
    } else {
        // Si no es una URL completa, asumir http://host:80
        scheme_ = "http";
        host_ = host;
        port_ = 80;
    }
}

void HttpClient::ensureClient() {
    if (!client_) {
        if (scheme_ == "https") {
#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
            client_ = std::make_unique<httplib::SSLClient>(host_, port_);
#else
            throw std::runtime_error("HTTPS no soportado. Recompilar con OpenSSL");
#endif
        } else {
            client_ = std::make_unique<httplib::Client>(host_, port_);
        }

        client_->set_read_timeout(timeout_seconds_, 0);
        client_->set_write_timeout(timeout_seconds_, 0);
    }
}

void HttpClient::setTimeout(int seconds) {
    timeout_seconds_ = seconds;
    if (client_) {
        client_->set_read_timeout(timeout_seconds_, 0);
        client_->set_write_timeout(timeout_seconds_, 0);
    }
}

void HttpClient::get(
    const std::string& path,
    const Headers& headers,
    ResponseCallback on_response,
    ErrorCallback on_error
) {
    ensureClient();

    httplib::Headers http_headers;
    for (const auto& [key, value] : headers) {
        http_headers.emplace(key, value);
    }

    auto result = client_->Get(path.c_str(), http_headers);

    if (result) {
        on_response(result->status, result->body);
    } else {
        std::string error_msg = "Error de red: ";
        auto err = result.error();
        switch (err) {
            case httplib::Error::Connection:
                error_msg += "No se pudo conectar al servidor";
                break;
            case httplib::Error::BindIPAddress:
                error_msg += "Error al vincular dirección IP";
                break;
            case httplib::Error::Read:
                error_msg += "Error al leer respuesta";
                break;
            case httplib::Error::Write:
                error_msg += "Error al escribir petición";
                break;
            case httplib::Error::ExceedRedirectCount:
                error_msg += "Demasiadas redirecciones";
                break;
            case httplib::Error::Canceled:
                error_msg += "Petición cancelada";
                break;
            case httplib::Error::SSLConnection:
                error_msg += "Error de conexión SSL";
                break;
            case httplib::Error::SSLLoadingCerts:
                error_msg += "Error al cargar certificados SSL";
                break;
            case httplib::Error::SSLServerVerification:
                error_msg += "Error de verificación del servidor SSL";
                break;
            case httplib::Error::UnsupportedMultipartBoundaryChars:
                error_msg += "Caracteres no soportados en boundary multipart";
                break;
            default:
                error_msg += "Error desconocido";
                break;
        }
        on_error(error_msg);
    }
}

void HttpClient::post(
    const std::string& path,
    const std::string& body,
    const Headers& headers,
    ResponseCallback on_response,
    ErrorCallback on_error
) {
    ensureClient();

    httplib::Headers http_headers;
    for (const auto& [key, value] : headers) {
        http_headers.emplace(key, value);
    }

    auto result = client_->Post(path.c_str(), http_headers, body, "application/json");

    if (result) {
        on_response(result->status, result->body);
    } else {
        on_error("Error de red: no se pudo completar la petición POST");
    }
}

void HttpClient::put(
    const std::string& path,
    const std::string& body,
    const Headers& headers,
    ResponseCallback on_response,
    ErrorCallback on_error
) {
    ensureClient();

    httplib::Headers http_headers;
    for (const auto& [key, value] : headers) {
        http_headers.emplace(key, value);
    }

    auto result = client_->Put(path.c_str(), http_headers, body, "application/json");

    if (result) {
        on_response(result->status, result->body);
    } else {
        on_error("Error de red: no se pudo completar la petición PUT");
    }
}

void HttpClient::del(
    const std::string& path,
    const Headers& headers,
    ResponseCallback on_response,
    ErrorCallback on_error
) {
    ensureClient();

    httplib::Headers http_headers;
    for (const auto& [key, value] : headers) {
        http_headers.emplace(key, value);
    }

    auto result = client_->Delete(path.c_str(), http_headers);

    if (result) {
        on_response(result->status, result->body);
    } else {
        on_error("Error de red: no se pudo completar la petición DELETE");
    }
}

} // namespace internal
} // namespace bizmap
