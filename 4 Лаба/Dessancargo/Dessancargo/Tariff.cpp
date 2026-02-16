#include "Tariff.h"
#include <sstream>
#include <iomanip>
#include <string>

using namespace std;

Tariff::Tariff(const string& name, double basePrice,
    unique_ptr<IPriceCalculationStrategy> strategy)
    : name_(name), basePrice_(basePrice), priceStrategy_(move(strategy)) {
    validate();
}

Tariff::Tariff(const Tariff& other)
    : name_(other.name_), basePrice_(other.basePrice_) {
    if (other.priceStrategy_) {
        priceStrategy_ = other.priceStrategy_->clone();
    }
}

Tariff& Tariff::operator=(const Tariff& other) {
    if (this != &other) {
        name_ = other.name_;
        basePrice_ = other.basePrice_;
        if (other.priceStrategy_) {
            priceStrategy_ = other.priceStrategy_->clone();
        }
    }
    return *this;
}

void Tariff::validate() const {
    if (name_.empty()) {
        throw TariffException("Название тарифа не может быть пустым");
    }
    if (basePrice_ < 0) {
        throw TariffException("Цена не может быть отрицательной");
    }
    if (!priceStrategy_) {
        throw TariffException("Стратегия расчета цены не установлена");
    }
}

string Tariff::getName() const {
    return name_;
}

double Tariff::getBasePrice() const {
    return basePrice_;
}

double Tariff::getFinalPrice() const {
    return priceStrategy_->calculateFinalPrice(basePrice_);
}

void Tariff::setPriceStrategy(unique_ptr<IPriceCalculationStrategy> strategy) {
    if (!strategy) {
        throw TariffException("Стратегия не может быть пустой");
    }
    priceStrategy_ = move(strategy);
}

string Tariff::getStrategyDescription() const {
    return priceStrategy_->getDescription();
}

void Tariff::setBasePrice(double price) {
    if (price < 0) {
        throw TariffException("Цена не может быть отрицательной");
    }
    basePrice_ = price;
}

void Tariff::setName(const string& name) {
    if (name.empty()) {
        throw TariffException("Название не может быть пустым");
    }
    name_ = name;
}