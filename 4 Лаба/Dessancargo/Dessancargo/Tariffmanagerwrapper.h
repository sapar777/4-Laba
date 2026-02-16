// TariffManagedWrapper.h
#pragma once

#include "Tariff.h"
#include "TransportCompany.h"
#include <msclr/marshal_cppstd.h>
#include <memory>

using namespace System;
using namespace msclr::interop;

namespace DessanCargoWinForms {

    public ref class TariffManaged {
    private:
        std::shared_ptr<Tariff>* nativeTariff;

    public:
        TariffManaged(String^ name, double basePrice, String^ strategyType, double discountPercent) {
            std::unique_ptr<IPriceCalculationStrategy> strategy;

            if (strategyType == "Discount") {
                strategy = std::make_unique<DiscountPriceStrategy>(discountPercent);
            }
            else {
                strategy = std::make_unique<StandardPriceStrategy>();
            }

            std::string stdName = marshal_as<std::string>(name);
            nativeTariff = new std::shared_ptr<Tariff>(
                new Tariff(stdName, basePrice, std::move(strategy))
            );
        }

        TariffManaged(std::shared_ptr<Tariff> tariff) {
            nativeTariff = new std::shared_ptr<Tariff>(tariff);
        }

        ~TariffManaged() {
            this->!TariffManaged();
        }

        !TariffManaged() {
            if (nativeTariff) {
                delete nativeTariff;
                nativeTariff = nullptr;
            }
        }

        property String^ Name {
            String^ get() {
                return gcnew String((*nativeTariff)->getName().c_str());
            }
        }

        property double BasePrice {
            double get() {
                return (*nativeTariff)->getBasePrice();
            }
        }

        property double FinalPrice {
            double get() {
                return (*nativeTariff)->getFinalPrice();
            }
        }

        property String^ StrategyDescription {
            String^ get() {
                return gcnew String((*nativeTariff)->getStrategyDescription().c_str());
            }
        }

        std::shared_ptr<Tariff> GetNativeTariff() {
            return *nativeTariff;
        }
    };

    public ref class TransportCompanyManaged {
    private:
        TransportCompany* company;

    public:
        TransportCompanyManaged() {
            company = new TransportCompany();
        }

        ~TransportCompanyManaged() {
            this->!TransportCompanyManaged();
        }

        !TransportCompanyManaged() {
            if (company) {
                delete company;
                company = nullptr;
            }
        }

        void InitializeDemoTariffs() {
            company->initializeDemoTariffs();
        }

        void AddTariff(String^ name, double basePrice, String^ strategyType, double discountPercent) {
            std::unique_ptr<IPriceCalculationStrategy> strategy;

            if (strategyType == "Discount") {
                strategy = std::make_unique<DiscountPriceStrategy>(discountPercent);
            }
            else {
                strategy = std::make_unique<StandardPriceStrategy>();
            }

            std::string stdName = marshal_as<std::string>(name);
            auto tariff = std::make_shared<Tariff>(stdName, basePrice, std::move(strategy));
            company->addTariff(tariff);
        }

        bool RemoveTariff(String^ name) {
            std::string stdName = marshal_as<std::string>(name);
            return company->removeTariff(stdName);
        }

        property int TariffCount {
            int get() {
                return static_cast<int>(company->getTariffCount());
            }
        }

        String^ GetCheapestTariff() {
            try {
                auto cheapest = company->findCheapestTariff();
                if (cheapest) {
                    return gcnew String(cheapest->getName().c_str()) + L" - " +
                        cheapest->getFinalPrice().ToString("N2") + L" руб.";
                }
                return L"Нет тарифов";
            }
            catch (...) {
                return L"Ошибка при поиске";
            }
        }

        String^ GenerateReport() {
            std::string report = company->generateReport();
            return gcnew String(report.c_str());
        }

        // Методы для DataGridView
        cli::array<String^, 2>^ GetTariffsForGrid() {
            auto allTariffs = company->getAllTariffsForGrid();
            int rowCount = static_cast<int>(allTariffs.size());

            if (rowCount == 0) {
                return gcnew cli::array<String^, 2>(0, 4);
            }

            cli::array<String^, 2>^ gridData = gcnew cli::array<String^, 2>(rowCount, 4);

            for (int i = 0; i < rowCount; i++) {
                const auto& row = allTariffs[i];
                for (int j = 0; j < 4; j++) {
                    gridData[i, j] = gcnew String(row[j].c_str());
                }
            }

            return gridData;
        }

        // Сортировка
        void SortByName(bool ascending) {
            company->sortByName(ascending);
        }

        void SortByPrice(bool ascending) {
            company->sortByPrice(ascending);
        }

        void SortByFinalPrice(bool ascending) {
            company->sortByFinalPrice(ascending);
        }

        // Работа с файлами
        bool SaveToFile(String^ fileName) {
            std::string stdFileName = marshal_as<std::string>(fileName);
            return company->saveToFile(stdFileName);
        }

        bool LoadFromFile(String^ fileName) {
            std::string stdFileName = marshal_as<std::string>(fileName);
            return company->loadFromFile(stdFileName);
        }
    };
}