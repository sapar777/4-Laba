#pragma once
#include <string>
#include <memory>

class IPriceCalculationStrategy {
public:
    virtual ~IPriceCalculationStrategy() = default;
    virtual double calculateFinalPrice(double basePrice) const = 0;
    virtual std::string getDescription() const = 0;
    virtual std::unique_ptr<IPriceCalculationStrategy> clone() const = 0;
};

class StandardPriceStrategy : public IPriceCalculationStrategy {
public:
    double calculateFinalPrice(double basePrice) const override;
    std::string getDescription() const override;
    std::unique_ptr<IPriceCalculationStrategy> clone() const override;
};

class DiscountPriceStrategy : public IPriceCalculationStrategy {
private:
    double discountPercent_;

public:
    explicit DiscountPriceStrategy(double discountPercent);
    double calculateFinalPrice(double basePrice) const override;
    std::string getDescription() const override;
    double getDiscountPercent() const;
    std::unique_ptr<IPriceCalculationStrategy> clone() const override;
};