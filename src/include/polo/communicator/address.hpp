#ifndef POLO_COMMUNICATOR_ADDRESS_HPP_
#define POLO_COMMUNICATOR_ADDRESS_HPP_

#include <cstdint>
#include <exception>
#include <regex>
#include <string>

#include "curl/curl.h"

namespace polo {
namespace communicator {
struct ip {
  static ip getexternal() { return getexternal("http://checkip.dyndns.org"); }
  static ip getexternal(const char *address) {
    std::string data, addr;
    CURL *handle = curl_easy_init();
    CURLcode rc = curl_easy_setopt(handle, CURLOPT_URL, address);
    rc = curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, ip::callback);
    rc = curl_easy_setopt(handle, CURLOPT_WRITEDATA, &data);
    rc = curl_easy_perform(handle);
    curl_easy_cleanup(handle);
    if (rc == CURLE_OK)
      return parse(data.c_str());
    else
      throw std::runtime_error("cURL did not succeed.");
  }

  static ip parse(const char *text) {
    std::string address{text};
    std::regex mask{"(25[0-5]|(?:2[0-4]|1{0,1}[0-9]){0,1}[0-9])\\.(25[0-5]|(?:"
                    "2[0-4]|1{0,1}[0-9]){0,1}[0-9])\\.(25[0-5]|(?:2[0-4]|1{0,1}"
                    "[0-9]){0,1}[0-9])\\.(25[0-5]"
                    "|(?:2[0-4]|1{0,1}[0-9]){0,1}[0-9])"};
    std::smatch match;
    if (std::regex_search(address, match, mask) && match.size() == 5)
      return ip{std::string{match[0].first, match[0].second}};
    else
      throw std::runtime_error("Could not parse IP address.");
  }

  ip() = default;

  std::string get() const { return address_; }

private:
  static std::size_t callback(char *ptr, std::size_t size, std::size_t nmemb,
                              void *data) {
    std::string *sptr = static_cast<std::string *>(data);
    *sptr = std::string(ptr, size * nmemb);
    return sptr->size();
  };

  explicit ip(std::string address) : address_{std::move(address)} {}

  std::string address_{"0.0.0.0"};
};

struct address {
  address() = default;
  address(communicator::ip ip, std::uint16_t port)
      : ip_{std::move(ip)}, port_{port} {}

  std::string ip() const { return ip_.get(); }
  std::uint16_t port() const noexcept { return port_; }

private:
  communicator::ip ip_;
  std::uint16_t port_;
};
} // namespace communicator
} // namespace polo

#endif
