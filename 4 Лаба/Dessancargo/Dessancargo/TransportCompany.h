#pragma once
#include "Tariff.h"
#include "Exceptions.h"
#include <vector>
#include <memory>
#include <algorithm>
#include <string>
#include <fstream>

class TransportCompany {
private:
    std::string name_;
    std::vector<std::shared_ptr<Tariff>> tariffs_;

public:
    TransportCompany();

    void addTariff(std::shared_ptr<Tariff> tariff);
    std::shared_ptr<Tariff> findCheapestTariff() const;
    std::vector<std::string> getAllTariffsInfo() const;
    std::shared_ptr<Tariff> findTariffByName(const std::string& name) const;
    std::shared_ptr<Tariff> findTariffByNameIgnoreCase(const std::string& name) const;
    bool removeTariff(const std::string& name);

    std::string getName() const;
    size_t getTariffCount() const;
    std::string generateReport() const;

    // Методы для работы с файлами
    bool saveToFile(const std::string& filename) const;
    bool loadFromFile(const std::string& filename);

    // Методы для сортировки
    void sortByName(bool ascending = true);
    void sortByPrice(bool ascending = true);
    void sortByFinalPrice(bool ascending = true);

    // Метод для получения всех тарифов в формате для DataGridView
    std::vector<std::vector<std::string>> getAllTariffsForGrid() const {
        std::vector<std::vector<std::string>> gridData;

        for (const auto& tariff : tariffs_) {
            gridData.push_back(tariff->getGridData());
        }

        return gridData;
    }

    void initializeDemoTariffs();
};