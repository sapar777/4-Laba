#include "TransportCompany.h"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <string>
#include <fstream>

using namespace std;

TransportCompany::TransportCompany() : name_("DessanCargo") {}

void TransportCompany::addTariff(shared_ptr<Tariff> tariff) {
    if (!tariff) {
        throw TariffException("Нельзя добавить пустой тариф");
    }

    auto it = find_if(tariffs_.begin(), tariffs_.end(),
        [&tariff](const shared_ptr<Tariff>& t) {
            return t->getName() == tariff->getName();
        });

    if (it != tariffs_.end()) {
        throw TariffException("Тариф с именем '" + tariff->getName() + "' уже существует");
    }

    tariffs_.push_back(tariff);
}

shared_ptr<Tariff> TransportCompany::findCheapestTariff() const {
    if (tariffs_.empty()) {
        throw TariffException("Список тарифов пуст");
    }

    auto cheapest = min_element(tariffs_.begin(), tariffs_.end(),
        [](const shared_ptr<Tariff>& a, const shared_ptr<Tariff>& b) {
            return a->getFinalPrice() < b->getFinalPrice();
        });

    return *cheapest;
}

vector<string> TransportCompany::getAllTariffsInfo() const {
    vector<string> result;
    if (tariffs_.empty()) {
        result.push_back("Список тарифов пуст.");
        return result;
    }

    for (const auto& tariff : tariffs_) {
        result.push_back(tariff->getGridData()[0] + " - " + tariff->getGridData()[3] + " руб.");
    }
    return result;
}

shared_ptr<Tariff> TransportCompany::findTariffByName(const string& name) const {
    auto it = find_if(tariffs_.begin(), tariffs_.end(),
        [&name](const shared_ptr<Tariff>& t) {
            return t->getName() == name;
        });

    return (it != tariffs_.end()) ? *it : nullptr;
}

shared_ptr<Tariff> TransportCompany::findTariffByNameIgnoreCase(const string& name) const {
    string nameLower = name;
    transform(nameLower.begin(), nameLower.end(), nameLower.begin(), ::tolower);

    for (const auto& tariff : tariffs_) {
        string tariffNameLower = tariff->getName();
        transform(tariffNameLower.begin(), tariffNameLower.end(), tariffNameLower.begin(), ::tolower);

        if (tariffNameLower == nameLower) {
            return tariff;
        }
    }

    return nullptr;
}

bool TransportCompany::removeTariff(const string& name) {
    auto it = remove_if(tariffs_.begin(), tariffs_.end(),
        [&name](const shared_ptr<Tariff>& t) {
            return t->getName() == name;
        });

    if (it != tariffs_.end()) {
        tariffs_.erase(it, tariffs_.end());
        return true;
    }
    return false;
}

string TransportCompany::getName() const {
    return name_;
}

size_t TransportCompany::getTariffCount() const {
    return tariffs_.size();
}

string TransportCompany::generateReport() const {
    ostringstream oss;
    oss << string(60, '=') << "\n"
        << "              ОТЧЕТ КОМПАНИИ " << name_ << "\n"
        << string(60, '=') << "\n";

    if (tariffs_.empty()) {
        oss << "Нет данных для отчета.";
        return oss.str();
    }

    auto cheapest = findCheapestTariff();
    auto mostExpensive = max_element(tariffs_.begin(), tariffs_.end(),
        [](const shared_ptr<Tariff>& a, const std::shared_ptr<Tariff>& b) {
            return a->getFinalPrice() < b->getFinalPrice();
        });

    int withDiscount = 0;
    double totalBase = 0;
    double totalFinal = 0;
    double maxDiscount = 0;

    for (const auto& tariff : tariffs_) {
        totalBase += tariff->getBasePrice();
        totalFinal += tariff->getFinalPrice();

        string desc = tariff->getStrategyDescription();
        if (desc.find("скидк") != string::npos || desc.find("%") != string::npos) {
            withDiscount++;

            double base = tariff->getBasePrice();
            double final = tariff->getFinalPrice();
            if (base > 0) {
                double discount = 100 * (base - final) / base;
                if (discount > maxDiscount) {
                    maxDiscount = discount;
                }
            }
        }
    }

    oss << "Общее количество тарифов: " << tariffs_.size() << "\n"
        << "Тарифов со скидкой: " << withDiscount << "\n"
        << "Самый дешевый тариф: " << cheapest->getName()
        << " (" << cheapest->getFinalPrice() << " руб.)\n"
        << "Самый дорогой тариф: " << (*mostExpensive)->getName()
        << " (" << (*mostExpensive)->getFinalPrice() << " руб.)\n"
        << "Средняя базовая цена: " << fixed << setprecision(2)
        << (totalBase / tariffs_.size()) << " руб.\n"
        << "Средняя итоговая цена: " << (totalFinal / tariffs_.size()) << " руб.\n"
        << "Максимальная скидка: " << fixed << setprecision(1)
        << maxDiscount << "%\n"
        << string(60, '=');

    return oss.str();
}

bool TransportCompany::saveToFile(const std::string& filename) const {
    try {
        ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }

        file << tariffs_.size() << endl;

        for (const auto& tariff : tariffs_) {
            file << tariff->getName() << endl;
            file << tariff->getBasePrice() << endl;

            string strategyDesc = tariff->getStrategyDescription();
            if (strategyDesc.find("скидк") != string::npos) {
                file << "DISCOUNT" << endl;
                size_t pos = strategyDesc.find('%');
                if (pos != string::npos) {
                    string percent = strategyDesc.substr(
                        strategyDesc.find(' ') + 1,
                        pos - strategyDesc.find(' ') - 1
                    );
                    file << percent << endl;
                }
            }
            else {
                file << "STANDARD" << endl;
            }
        }

        file.close();
        return true;
    }
    catch (...) {
        return false;
    }
}

bool TransportCompany::loadFromFile(const std::string& filename) {
    try {
        ifstream file(filename);
        if (!file.is_open()) {
            return false;
        }

        tariffs_.clear();

        size_t count;
        file >> count;
        file.ignore();

        for (size_t i = 0; i < count; ++i) {
            string name;
            getline(file, name);

            double basePrice;
            file >> basePrice;
            file.ignore();

            string strategyType;
            getline(file, strategyType);

            unique_ptr<IPriceCalculationStrategy> strategy;

            if (strategyType == "DISCOUNT") {
                double discountPercent;
                file >> discountPercent;
                file.ignore();
                strategy = make_unique<DiscountPriceStrategy>(discountPercent);
            }
            else {
                strategy = make_unique<StandardPriceStrategy>();
            }

            auto tariff = make_shared<Tariff>(name, basePrice, move(strategy));
            tariffs_.push_back(tariff);
        }

        file.close();
        return true;
    }
    catch (...) {
        return false;
    }
}

void TransportCompany::sortByName(bool ascending) {
    sort(tariffs_.begin(), tariffs_.end(),
        [ascending](const shared_ptr<Tariff>& a, const shared_ptr<Tariff>& b) {
            if (ascending) {
                return a->getName() < b->getName();
            }
            else {
                return a->getName() > b->getName();
            }
        });
}

void TransportCompany::sortByPrice(bool ascending) {
    sort(tariffs_.begin(), tariffs_.end(),
        [ascending](const shared_ptr<Tariff>& a, const shared_ptr<Tariff>& b) {
            if (ascending) {
                return a->getBasePrice() < b->getBasePrice();
            }
            else {
                return a->getBasePrice() > b->getBasePrice();
            }
        });
}

void TransportCompany::sortByFinalPrice(bool ascending) {
    sort(tariffs_.begin(), tariffs_.end(),
        [ascending](const shared_ptr<Tariff>& a, const shared_ptr<Tariff>& b) {
            if (ascending) {
                return a->getFinalPrice() < b->getFinalPrice();
            }
            else {
                return a->getFinalPrice() > b->getFinalPrice();
            }
        });
}

void TransportCompany::initializeDemoTariffs() {
    try {
        auto tariff1 = make_shared<Tariff>(
            "Economy", 5000, make_unique<StandardPriceStrategy>());
        tariffs_.push_back(tariff1);

        auto tariff2 = make_shared<Tariff>(
            "Standard", 8000, make_unique<DiscountPriceStrategy>(10));
        tariffs_.push_back(tariff2);

        auto tariff3 = make_shared<Tariff>(
            "Premium", 15000, make_unique<DiscountPriceStrategy>(15));
        tariffs_.push_back(tariff3);

        auto tariff4 = make_shared<Tariff>(
            "Express", 12000, make_unique<DiscountPriceStrategy>(5));
        tariffs_.push_back(tariff4);

        auto tariff5 = make_shared<Tariff>(
            "International", 25000, make_unique<DiscountPriceStrategy>(12));
        tariffs_.push_back(tariff5);
    }
    catch (const TariffException& e) {
        throw;
    }
}