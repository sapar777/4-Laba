#pragma once
#include "PriceStrategy.h"
#include "Exceptions.h"
#include <string>
#include <memory>
#include <iomanip>
#include <sstream>
#include <vector>

class Tariff {
private:
    std::string name_;
    double basePrice_;
    std::unique_ptr<IPriceCalculationStrategy> priceStrategy_;

public:
    Tariff(const std::string& name, double basePrice,
        std::unique_ptr<IPriceCalculationStrategy> strategy);
    Tariff(const Tariff& other);
    Tariff& operator=(const Tariff& other);

    void validate() const;
    std::string getName() const;
    double getBasePrice() const;
    double getFinalPrice() const;
    void setPriceStrategy(std::unique_ptr<IPriceCalculationStrategy> strategy);
    std::string getStrategyDescription() const;
    void setBasePrice(double price);
    void setName(const std::string& name);

    // Метод для получения данных в формате для DataGridView
    std::vector<std::string> getGridData() const {
        std::vector<std::string> rowData;
        rowData.push_back(name_);

        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << basePrice_;
        rowData.push_back(oss.str());

        rowData.push_back(getStrategyDescription());

        oss.str("");
        oss << std::fixed << std::setprecision(2) << getFinalPrice();
        rowData.push_back(oss.str());

        return rowData;
    }
};