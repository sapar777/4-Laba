#pragma once
#include <stdexcept>
#include <string>

class TransportCompanyException : public std::runtime_error {
public:
    TransportCompanyException(const std::string& message)
        : std::runtime_error(message) {}
};

class TariffException : public TransportCompanyException {
public:
    TariffException(const std::string& message)
        : TransportCompanyException(message) {}
};