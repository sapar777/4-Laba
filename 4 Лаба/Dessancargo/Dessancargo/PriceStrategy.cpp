#include "PriceStrategy.h"
#include "Exceptions.h"
#include <string>

using namespace std;

double StandardPriceStrategy::calculateFinalPrice(double basePrice) const {
    if (basePrice < 0) {
        throw TariffException("Базовая цена не может быть отрицательной");
    }
    return basePrice;
}

string StandardPriceStrategy::getDescription() const {
    return "Без скидки";
}

unique_ptr<IPriceCalculationStrategy> StandardPriceStrategy::clone() const {
    return make_unique<StandardPriceStrategy>(*this);
}

DiscountPriceStrategy::DiscountPriceStrategy(double discountPercent)
    : discountPercent_(discountPercent) {
    if (discountPercent < 0 || discountPercent > 100) {
        throw TariffException("Скидка должна быть в диапазоне 0-100%");
    }
}

double DiscountPriceStrategy::calculateFinalPrice(double basePrice) const {
    if (basePrice < 0) {
        throw TariffException("Базовая цена не может быть отрицательной");
    }
    return basePrice * (1 - discountPercent_ / 100.0);
}

string DiscountPriceStrategy::getDescription() const {
    return "Со скидкой " + to_string(static_cast<int>(discountPercent_)) + "%";
}

double DiscountPriceStrategy::getDiscountPercent() const {
    return discountPercent_;
}

unique_ptr<IPriceCalculationStrategy> DiscountPriceStrategy::clone() const {
    return make_unique<DiscountPriceStrategy>(*this);
}