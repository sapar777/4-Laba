#pragma once

#using <System.dll>
#using <System.Windows.Forms.dll>
#using <System.Drawing.dll>
#using <System.Data.dll>

using namespace System;
using namespace System::IO;
using namespace System::Text;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;  // Это важно для DialogResult
using namespace System::Drawing;
using namespace System::Data;

// Добавьте эту строку для упрощения доступа к DialogResult
using namespace System::Windows::Forms;

#include "TariffWrapper.h"
#include "DetailsForm.h"

namespace DessanCargoWinForms {

    public ref class MainForm : public Form {
    public:
        MainForm(void) {
            InitializeComponent();
            LoadDemoTariffs();
        }

    protected:
        ~MainForm() {
            if (components) delete components;
        }

    private:
        System::ComponentModel::Container^ components;

        // Элементы формы
        DataGridView^ dataGridViewTariffs;
        Button^ btnAddTariff;
        Button^ btnEditTariff;
        Button^ btnDeleteTariff;
        Button^ btnLoadFromFile;
        Button^ btnSaveToFile;
        Button^ btnSortName;
        Button^ btnSortPrice;
        Button^ btnSortFinalPrice;
        Button^ btnFindCheapest;
        Button^ btnGenerateReport;
        Button^ btnCompanyInfo;
        Label^ lblTitle;
        StatusStrip^ statusStrip1;
        ToolStripStatusLabel^ lblStatus;
        MenuStrip^ menuStrip1;
        ToolStripMenuItem^ fileToolStripMenuItem;
        ToolStripMenuItem^ loadToolStripMenuItem;
        ToolStripMenuItem^ saveToolStripMenuItem;
        ToolStripMenuItem^ exitToolStripMenuItem;

        // Коллекция тарифов
        System::Collections::Generic::List<TariffWrapper^>^ tariffs;

        bool nameSortAscending = true;
        bool priceSortAscending = true;
        bool finalPriceSortAscending = true;

        // ВСПОМОГАТЕЛЬНЫЕ МЕТОДЫ
        int GetValidRowCount() {
            int count = 0;
            for each (DataGridViewRow ^ row in dataGridViewTariffs->Rows) {
                if (!row->IsNewRow && row->Cells["Name"]->Value != nullptr) {
                    String^ name = row->Cells["Name"]->Value->ToString();
                    if (!String::IsNullOrEmpty(name)) {
                        count++;
                    }
                }
            }
            return count;
        }

        Button^ CreateButton(String^ text, int x, int y, int width, int height) {
            Button^ btn = gcnew Button();
            btn->Text = text;
            btn->Location = Point(x, y);
            btn->Size = Drawing::Size(width, height);
            this->Controls->Add(btn);
            return btn;
        }

        void AddTariffToGrid(TariffWrapper^ tariff) {
            if (tariff == nullptr || String::IsNullOrEmpty(tariff->Name)) return;

            if (tariff->BasePrice > 1000000) {
                MessageBox::Show(String::Format(L"Тариф '{0}' имеет цену больше 1,000,000 руб. и не будет добавлен.",
                    tariff->Name), L"Предупреждение", MessageBoxButtons::OK, MessageBoxIcon::Warning);
                return;
            }

            String^ strategyText;
            double finalPrice;

            // ИСПРАВЛЕНО: Проверяем оба возможных значения
            if (tariff->StrategyType == "DISCOUNT" && tariff->DiscountPercent > 0) {
                strategyText = L"Со скидкой " + tariff->DiscountPercent.ToString("F1") + L"%";
                finalPrice = tariff->BasePrice * (1 - tariff->DiscountPercent / 100.0);
            }
            else {
                strategyText = L"Без скидки";
                finalPrice = tariff->BasePrice;
            }

            // ИСПРАВЛЕНО: Форматируем цену с правильным разделителем
            dataGridViewTariffs->Rows->Add(
                tariff->Name,
                tariff->BasePrice.ToString("F2", System::Globalization::CultureInfo::InvariantCulture),
                strategyText,
                finalPrice.ToString("F2", System::Globalization::CultureInfo::InvariantCulture)
            );
        }

        void UpdateStatus() {
            int count = GetValidRowCount();
            lblStatus->Text = L"Готово. Тарифов: " + count.ToString();
        }

        void LoadDemoTariffs() {
            dataGridViewTariffs->Rows->Clear();

            array<TariffWrapper^>^ demoTariffs = gcnew array<TariffWrapper^>(5);

            demoTariffs[0] = gcnew TariffWrapper();
            demoTariffs[0]->Name = "Economy";
            demoTariffs[0]->BasePrice = 5000;
            demoTariffs[0]->StrategyType = "STANDARD";
            demoTariffs[0]->DiscountPercent = 0;

            demoTariffs[1] = gcnew TariffWrapper();
            demoTariffs[1]->Name = "Standard";
            demoTariffs[1]->BasePrice = 8000;
            demoTariffs[1]->StrategyType = "DISCOUNT";  // Важно: именно "DISCOUNT"
            demoTariffs[1]->DiscountPercent = 10;

            demoTariffs[2] = gcnew TariffWrapper();
            demoTariffs[2]->Name = "Premium";
            demoTariffs[2]->BasePrice = 15000;
            demoTariffs[2]->StrategyType = "DISCOUNT";  // Важно: именно "DISCOUNT"
            demoTariffs[2]->DiscountPercent = 15;

            demoTariffs[3] = gcnew TariffWrapper();
            demoTariffs[3]->Name = "Express";
            demoTariffs[3]->BasePrice = 12000;
            demoTariffs[3]->StrategyType = "DISCOUNT";  // Важно: именно "DISCOUNT"
            demoTariffs[3]->DiscountPercent = 5;

            demoTariffs[4] = gcnew TariffWrapper();
            demoTariffs[4]->Name = "International";
            demoTariffs[4]->BasePrice = 25000;
            demoTariffs[4]->StrategyType = "DISCOUNT";  // Важно: именно "DISCOUNT"
            demoTariffs[4]->DiscountPercent = 12;

            for each (TariffWrapper ^ tariff in demoTariffs) {
                AddTariffToGrid(tariff);
            }

            UpdateStatus();
        }

        // ========== МЕТОДЫ СОХРАНЕНИЯ И ЗАГРУЗКИ ==========
        void SaveTariffsToFile(String^ fileName) {
            try {
                StreamWriter^ writer = gcnew StreamWriter(fileName, false, Encoding::UTF8);
                try {
                    writer->WriteLine(L"Тарифы транспортной компании DessanCargo");
                    writer->WriteLine(L"=========================================");
                    writer->WriteLine(String::Format(L"Дата сохранения: {0}", DateTime::Now.ToString(L"dd.MM.yyyy HH:mm")));
                    writer->WriteLine();

                    int count = 0;
                    for each (DataGridViewRow ^ row in dataGridViewTariffs->Rows) {
                        if (!row->IsNewRow && row->Cells["Name"]->Value != nullptr) {
                            String^ name = row->Cells["Name"]->Value->ToString();
                            String^ basePrice = row->Cells["BasePrice"]->Value->ToString();
                            String^ strategy = row->Cells["Strategy"]->Value->ToString();

                            String^ strategyType = L"STANDARD";
                            double discountPercent = 0.0;

                            if (strategy->Contains(L"скидк") || strategy->Contains(L"%")) {
                                strategyType = L"DISCOUNT";

                                int percentIndex = strategy->IndexOf(L'%');
                                if (percentIndex > 0) {
                                    int startIndex = 0;
                                    for (int i = percentIndex - 1; i >= 0; i--) {
                                        if (!Char::IsDigit(strategy[i]) && strategy[i] != L'.' && strategy[i] != L',') {
                                            startIndex = i + 1;
                                            break;
                                        }
                                    }

                                    if (startIndex < percentIndex) {
                                        String^ discountStr = strategy->Substring(startIndex, percentIndex - startIndex);
                                        discountStr = discountStr->Replace(L",", L".");
                                        Double::TryParse(discountStr, System::Globalization::NumberStyles::Float,
                                            System::Globalization::CultureInfo::InvariantCulture, discountPercent);
                                    }
                                }
                            }

                            // ИСПРАВЛЕНО: Убираем все пробелы из цены
                            String^ basePriceClean = basePrice->Replace(L" ", L"")->Replace(L",", L".");

                            // Дополнительно убираем неразрывный пробел, если он есть
                            basePriceClean = basePriceClean->Replace(L"\u00A0", L"");

                            writer->WriteLine(L"----- ТАРИФ -----");
                            writer->WriteLine(String::Format(L"НОМЕР={0}", ++count));
                            writer->WriteLine(String::Format(L"НАЗВАНИЕ={0}", name));
                            writer->WriteLine(String::Format(L"БАЗОВАЯ_ЦЕНА={0}", basePriceClean));
                            writer->WriteLine(String::Format(L"ТИП_СТРАТЕГИИ={0}", strategyType));
                            writer->WriteLine(String::Format(L"ПРОЦЕНТ_СКИДКИ={0}", discountPercent.ToString(L"F1", System::Globalization::CultureInfo::InvariantCulture)));
                            writer->WriteLine(L"----- КОНЕЦ ТАРИФА -----");
                            writer->WriteLine();
                        }
                    }

                    writer->WriteLine(L"=========================================");
                    writer->WriteLine(String::Format(L"ВСЕГО ТАРИФОВ={0}", count));
                }
                finally {
                    delete writer;
                }

                MessageBox::Show(String::Format(L"Тарифы успешно сохранены в файл:\n{0}", fileName),
                    L"Сохранение успешно", MessageBoxButtons::OK, MessageBoxIcon::Information);
            }
            catch (Exception^ ex) {
                MessageBox::Show(String::Format(L"Ошибка при сохранении файла:\n{0}", ex->Message),
                    L"Ошибка", MessageBoxButtons::OK, MessageBoxIcon::Error);
            }
        }

        void LoadTariffsFromFile(String^ fileName) {
            try {
                if (!File::Exists(fileName)) {
                    MessageBox::Show(L"Файл не найден", L"Ошибка",
                        MessageBoxButtons::OK, MessageBoxIcon::Error);
                    return;
                }

                int currentCount = GetValidRowCount();
                if (currentCount > 0) {
                    if (MessageBox::Show(L"При загрузке текущие данные будут заменены. Продолжить?",
                        L"Подтверждение", MessageBoxButtons::YesNo, MessageBoxIcon::Question) != System::Windows::Forms::DialogResult::Yes) {
                        return;
                    }
                }

                dataGridViewTariffs->Rows->Clear();

                StreamReader^ reader = gcnew StreamReader(fileName, Encoding::UTF8);
                try {
                    String^ line;
                    TariffWrapper^ currentTariff = nullptr;
                    bool inTariff = false;

                    while ((line = reader->ReadLine()) != nullptr) {
                        line = line->Trim();

                        if (String::IsNullOrEmpty(line)) {
                            continue;
                        }

                        if (line == L"----- ТАРИФ -----") {
                            if (currentTariff != nullptr && !String::IsNullOrEmpty(currentTariff->Name)) {
                                AddTariffToGrid(currentTariff);
                            }
                            currentTariff = gcnew TariffWrapper();
                            inTariff = true;
                            continue;
                        }

                        if (line == L"----- КОНЕЦ ТАРИФА -----") {
                            if (currentTariff != nullptr && !String::IsNullOrEmpty(currentTariff->Name)) {
                                AddTariffToGrid(currentTariff);
                            }
                            currentTariff = nullptr;
                            inTariff = false;
                            continue;
                        }

                        if (inTariff && currentTariff != nullptr && line->Contains(L"=")) {
                            int equalPos = line->IndexOf(L'=');
                            if (equalPos > 0) {
                                String^ key = line->Substring(0, equalPos)->Trim();
                                String^ value = line->Substring(equalPos + 1)->Trim();

                                if (key == L"НАЗВАНИЕ") {
                                    currentTariff->Name = value;
                                }
                                else if (key == L"БАЗОВАЯ_ЦЕНА") {
                                    // ИСПРАВЛЕНО: Убираем ВСЕ возможные проблемы с пробелами
                                    String^ priceStr = value->Replace(L" ", L"")  // обычный пробел
                                        ->Replace(L"\u00A0", L"")  // неразрывный пробел
                                        ->Replace(L",", L".");     // запятая на точку

                                    double price;
                                    if (Double::TryParse(priceStr, System::Globalization::NumberStyles::Float,
                                        System::Globalization::CultureInfo::InvariantCulture, price)) {
                                        currentTariff->BasePrice = price;
                                    }
                                    else {
                                        // Если не удалось распарсить, пробуем другой способ
                                        Double::TryParse(priceStr, price);
                                    }
                                }
                                else if (key == L"ТИП_СТРАТЕГИИ") {
                                    currentTariff->StrategyType = value;
                                }
                                else if (key == L"ПРОЦЕНТ_СКИДКИ") {
                                    String^ discountStr = value->Replace(L",", L".");
                                    double discount;
                                    if (Double::TryParse(discountStr, System::Globalization::NumberStyles::Float,
                                        System::Globalization::CultureInfo::InvariantCulture, discount)) {
                                        currentTariff->DiscountPercent = discount;
                                    }
                                }
                            }
                        }
                    }

                    if (currentTariff != nullptr && !String::IsNullOrEmpty(currentTariff->Name)) {
                        AddTariffToGrid(currentTariff);
                    }
                }
                finally {
                    delete reader;
                }

                UpdateStatus();
                MessageBox::Show(L"Тарифы успешно загружены из файла", L"Успех",
                    MessageBoxButtons::OK, MessageBoxIcon::Information);
            }
            catch (Exception^ ex) {
                MessageBox::Show(String::Format(L"Ошибка при загрузке файла:\n{0}", ex->Message),
                    L"Ошибка", MessageBoxButtons::OK, MessageBoxIcon::Error);
            }
        }

        // ========== МЕТОД InitializeComponent ==========
        void InitializeComponent(void) {
            this->components = gcnew System::ComponentModel::Container();
            this->Text = L"DessanCargo - Система управления тарифами";
            this->Size = Drawing::Size(900, 650);
            this->StartPosition = FormStartPosition::CenterScreen;

            tariffs = gcnew System::Collections::Generic::List<TariffWrapper^>();

            // Menu Strip
            menuStrip1 = gcnew MenuStrip();
            fileToolStripMenuItem = gcnew ToolStripMenuItem();
            loadToolStripMenuItem = gcnew ToolStripMenuItem();
            saveToolStripMenuItem = gcnew ToolStripMenuItem();
            exitToolStripMenuItem = gcnew ToolStripMenuItem();

            fileToolStripMenuItem->Text = L"Файл";
            loadToolStripMenuItem->Text = L"Загрузить из файла...";
            loadToolStripMenuItem->Click += gcnew EventHandler(this, &MainForm::btnLoadFromFile_Click);
            saveToolStripMenuItem->Text = L"Сохранить в файл...";
            saveToolStripMenuItem->Click += gcnew EventHandler(this, &MainForm::btnSaveToFile_Click);
            exitToolStripMenuItem->Text = L"Выход";
            exitToolStripMenuItem->Click += gcnew EventHandler(this, &MainForm::exitToolStripMenuItem_Click);

            fileToolStripMenuItem->DropDownItems->AddRange(gcnew array<ToolStripItem^> {
                loadToolStripMenuItem, saveToolStripMenuItem, exitToolStripMenuItem
            });
            menuStrip1->Items->Add(fileToolStripMenuItem);
            this->Controls->Add(menuStrip1);
            this->MainMenuStrip = menuStrip1;

            // Title Label 
            lblTitle = gcnew Label();
            lblTitle->Text = L"ТАРИФЫ ТРАНСПОРТНОЙ КОМПАНИИ DESSANCARGO";
            lblTitle->Font = gcnew Drawing::Font(L"Microsoft Sans Serif", 16, FontStyle::Bold);
            lblTitle->Location = Point(50, 40);
            lblTitle->Size = Drawing::Size(800, 30);
            lblTitle->TextAlign = ContentAlignment::MiddleCenter;
            this->Controls->Add(lblTitle);

            // DataGridView
            dataGridViewTariffs = gcnew DataGridView();
            dataGridViewTariffs->Location = Point(50, 90);
            dataGridViewTariffs->Size = Drawing::Size(800, 300);
            dataGridViewTariffs->ReadOnly = true;
            dataGridViewTariffs->AllowUserToAddRows = false;
            dataGridViewTariffs->AllowUserToDeleteRows = false;
            dataGridViewTariffs->SelectionMode = DataGridViewSelectionMode::FullRowSelect;
            dataGridViewTariffs->AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode::Fill;

            dataGridViewTariffs->Columns->Add("Name", "Название тарифа");
            dataGridViewTariffs->Columns->Add("BasePrice", "Базовая цена (руб.)");
            dataGridViewTariffs->Columns->Add("Strategy", "Стратегия расчета");
            dataGridViewTariffs->Columns->Add("FinalPrice", "Итоговая цена (руб.)");

            this->Controls->Add(dataGridViewTariffs);

            // Кнопки управления
            int buttonY = 410;
            int buttonWidth = 140;
            int buttonHeight = 35;
            int buttonSpacing = 10;

            // Первый ряд кнопок
            btnAddTariff = CreateButton(L"Добавить тариф", 50, buttonY, buttonWidth, buttonHeight);
            btnEditTariff = CreateButton(L"Редактировать", 50 + buttonWidth + buttonSpacing, buttonY, buttonWidth, buttonHeight);
            btnDeleteTariff = CreateButton(L"Удалить тариф", 50 + (buttonWidth + buttonSpacing) * 2, buttonY, buttonWidth, buttonHeight);
            btnLoadFromFile = CreateButton(L"Загрузить из файла", 50 + (buttonWidth + buttonSpacing) * 3, buttonY, buttonWidth, buttonHeight);

            // Второй ряд кнопок
            btnSortName = CreateButton(L"Сорт. по имени", 50, buttonY + 50, buttonWidth, buttonHeight);
            btnSortPrice = CreateButton(L"Сорт. по цене", 50 + buttonWidth + buttonSpacing, buttonY + 50, buttonWidth, buttonHeight);
            btnSortFinalPrice = CreateButton(L"Сорт. по итогу", 50 + (buttonWidth + buttonSpacing) * 2, buttonY + 50, buttonWidth, buttonHeight);
            btnSaveToFile = CreateButton(L"Сохранить в файл", 50 + (buttonWidth + buttonSpacing) * 3, buttonY + 50, buttonWidth, buttonHeight);

            // Привязываем обработчики событий
            btnAddTariff->Click += gcnew EventHandler(this, &MainForm::btnAddTariff_Click);
            btnEditTariff->Click += gcnew EventHandler(this, &MainForm::btnEditTariff_Click);
            btnDeleteTariff->Click += gcnew EventHandler(this, &MainForm::btnDeleteTariff_Click);
            btnSortName->Click += gcnew EventHandler(this, &MainForm::btnSortName_Click);
            btnSortPrice->Click += gcnew EventHandler(this, &MainForm::btnSortPrice_Click);
            btnSortFinalPrice->Click += gcnew EventHandler(this, &MainForm::btnSortFinalPrice_Click);
            btnLoadFromFile->Click += gcnew EventHandler(this, &MainForm::btnLoadFromFile_Click);
            btnSaveToFile->Click += gcnew EventHandler(this, &MainForm::btnSaveToFile_Click);

            statusStrip1 = gcnew StatusStrip();
            lblStatus = gcnew ToolStripStatusLabel();
            lblStatus->Text = L"Готово. Тарифов: 0";
            statusStrip1->Items->Add(lblStatus);
            this->Controls->Add(statusStrip1);
        }

        // ========== ОБРАБОТЧИКИ СОБЫТИЙ ==========
        void exitToolStripMenuItem_Click(Object^ sender, EventArgs^ e) {
            this->Close();
        }

        void btnAddTariff_Click(Object^ sender, EventArgs^ e) {
            DetailsForm^ detailsForm = gcnew DetailsForm();
                
            // ИСПРАВЛЕНО: используем полное имя или просто DialogResult
            if (detailsForm->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
                // или можно просто:
                // if (detailsForm->ShowDialog() == DialogResult::OK) {

                TariffWrapper^ newTariff = detailsForm->ResultTariff;

                if (newTariff != nullptr) {
                    if (newTariff->BasePrice > 1000000) {
                        MessageBox::Show(L"Цена не может превышать 1,000,000 руб.", L"Ошибка",
                            MessageBoxButtons::OK, MessageBoxIcon::Error);
                        return;
                    }

                    

                    AddTariffToGrid(newTariff);
                    UpdateStatus();
                    MessageBox::Show(L"Тариф успешно добавлен!", L"Успех",
                        MessageBoxButtons::OK, MessageBoxIcon::Information);
                }
            }
        }

        void btnEditTariff_Click(Object^ sender, EventArgs^ e) {
            try {
                if (dataGridViewTariffs->SelectedRows->Count == 0) {
                    MessageBox::Show(L"Выберите тариф для редактирования", L"Ошибка",
                        MessageBoxButtons::OK, MessageBoxIcon::Warning);
                    return;
                }

                DataGridViewRow^ selectedRow = dataGridViewTariffs->SelectedRows[0];

                if (selectedRow->IsNewRow || selectedRow->Cells["Name"]->Value == nullptr) {
                    MessageBox::Show(L"Выберите существующий тариф для редактирования", L"Ошибка",
                        MessageBoxButtons::OK, MessageBoxIcon::Warning);
                    return;
                }

                TariffWrapper^ existingTariff = gcnew TariffWrapper();
                existingTariff->Name = selectedRow->Cells["Name"]->Value->ToString();

                String^ priceStr = selectedRow->Cells["BasePrice"]->Value->ToString();
                priceStr = priceStr->Replace(" ", "")->Replace(",", ".");

                double price;
                if (Double::TryParse(priceStr, System::Globalization::NumberStyles::Float,
                    System::Globalization::CultureInfo::InvariantCulture, price)) {
                    existingTariff->BasePrice = price;
                }

                String^ strategyText = selectedRow->Cells["Strategy"]->Value->ToString();
                if (strategyText != nullptr && (strategyText->Contains(L"скидк") || strategyText->Contains(L"%"))) {
                    existingTariff->StrategyType = "DISCOUNT";  // ИСПРАВЛЕНО: всегда "DISCOUNT"

                    int percentIndex = strategyText->IndexOf(L"%");
                    if (percentIndex > 0) {
                        int startIndex = 0;
                        for (int i = percentIndex - 1; i >= 0; i--) {
                            if (!Char::IsDigit(strategyText[i]) && strategyText[i] != '.' && strategyText[i] != ',') {
                                startIndex = i + 1;
                                break;
                            }
                        }

                        if (startIndex < percentIndex) {
                            String^ discountStr = strategyText->Substring(startIndex, percentIndex - startIndex);
                            discountStr = discountStr->Replace(",", ".");

                            double discount;
                            if (Double::TryParse(discountStr, System::Globalization::NumberStyles::Float,
                                System::Globalization::CultureInfo::InvariantCulture, discount)) {
                                existingTariff->DiscountPercent = discount;
                            }
                        }
                    }
                }
                else {
                    existingTariff->StrategyType = "STANDARD";  // ИСПРАВЛЕНО: всегда "STANDARD"
                    existingTariff->DiscountPercent = 0;
                }

                DetailsForm^ detailsForm = gcnew DetailsForm(existingTariff);

                if (detailsForm->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
                    TariffWrapper^ updatedTariff = detailsForm->ResultTariff;

                    if (updatedTariff != nullptr) {
                        if (updatedTariff->BasePrice > 1000000) {
                            MessageBox::Show(L"Цена не может превышать 1,000,000 руб.", L"Ошибка",
                                MessageBoxButtons::OK, MessageBoxIcon::Error);
                            return;
                        }

                        String^ newStrategyText = L"Без скидки";
                        double newFinalPrice = updatedTariff->BasePrice;

                        // ИСПРАВЛЕНО: проверяем оба варианта
                        if (updatedTariff->StrategyType == "DISCOUNT" || updatedTariff->StrategyType == "Discount") {
                            newStrategyText = L"Со скидкой " + updatedTariff->DiscountPercent.ToString("F1") + L"%";
                            newFinalPrice = updatedTariff->BasePrice * (1 - updatedTariff->DiscountPercent / 100.0);
                        }

                        selectedRow->Cells["Name"]->Value = updatedTariff->Name;
                        selectedRow->Cells["BasePrice"]->Value = updatedTariff->BasePrice.ToString("N2");
                        selectedRow->Cells["Strategy"]->Value = newStrategyText;
                        selectedRow->Cells["FinalPrice"]->Value = newFinalPrice.ToString("N2");

                        UpdateStatus();
                        MessageBox::Show(L"Тариф успешно обновлен!", L"Успех",
                            MessageBoxButtons::OK, MessageBoxIcon::Information);
                    }
                }
            }
            catch (Exception^ ex) {
                MessageBox::Show(L"Ошибка при редактировании: " + ex->Message, L"Ошибка",
                    MessageBoxButtons::OK, MessageBoxIcon::Error);
            }
        }

        void btnDeleteTariff_Click(Object^ sender, EventArgs^ e) {
            if (dataGridViewTariffs->SelectedRows->Count == 0) {
                MessageBox::Show(L"Выберите тариф для удаления", L"Ошибка",
                    MessageBoxButtons::OK, MessageBoxIcon::Warning);
                return;
            }

            DataGridViewRow^ selectedRow = dataGridViewTariffs->SelectedRows[0];
            if (selectedRow->IsNewRow) return;

            String^ name = selectedRow->Cells["Name"]->Value->ToString();

            String^ message = L"Вы уверены, что хотите удалить тариф '" + name + L"'?";
            // ИСПРАВЛЕНО: используем полное имя для DialogResult::Yes
            if (MessageBox::Show(message, L"Подтверждение удаления",
                MessageBoxButtons::YesNo, MessageBoxIcon::Question) == System::Windows::Forms::DialogResult::Yes) {
                dataGridViewTariffs->Rows->Remove(selectedRow);
                UpdateStatus();
                MessageBox::Show(L"Тариф успешно удален", L"Успех",
                    MessageBoxButtons::OK, MessageBoxIcon::Information);
            }
        }

        void btnSortName_Click(Object^ sender, EventArgs^ e) {
            dataGridViewTariffs->Sort(dataGridViewTariffs->Columns["Name"],
                System::ComponentModel::ListSortDirection::Ascending);
            lblStatus->Text = L"Отсортировано по названию";
        }

        void btnSortPrice_Click(Object^ sender, EventArgs^ e) {
            dataGridViewTariffs->Sort(dataGridViewTariffs->Columns["BasePrice"],
                System::ComponentModel::ListSortDirection::Ascending);
            lblStatus->Text = L"Отсортировано по базовой цене";
        }

        void btnSortFinalPrice_Click(Object^ sender, EventArgs^ e) {
            dataGridViewTariffs->Sort(dataGridViewTariffs->Columns["FinalPrice"],
                System::ComponentModel::ListSortDirection::Ascending);
            lblStatus->Text = L"Отсортировано по итоговой цене";
        }

        void btnFindCheapest_Click(Object^ sender, EventArgs^ e) {
            int count = GetValidRowCount();
            if (count == 0) {
                MessageBox::Show(L"Нет тарифов для поиска", L"Ошибка",
                    MessageBoxButtons::OK, MessageBoxIcon::Warning);
                return;
            }

            double minPrice = Double::MaxValue;
            String^ cheapestName = L"";
            String^ cheapestPrice = L"";

            for each (DataGridViewRow ^ row in dataGridViewTariffs->Rows) {
                if (row->IsNewRow) continue;

                String^ priceStr = row->Cells["FinalPrice"]->Value->ToString();
                double price = Double::Parse(priceStr->Replace(",", "."));

                if (price < minPrice) {
                    minPrice = price;
                    cheapestName = row->Cells["Name"]->Value->ToString();
                    cheapestPrice = priceStr;
                }
            }

            String^ message = L"Самый дешевый тариф:\n\n" +
                L"Название: " + cheapestName + L"\n" +
                L"Итоговая цена: " + cheapestPrice + L" руб.";
            MessageBox::Show(message, L"Самый дешевый тариф",
                MessageBoxButtons::OK, MessageBoxIcon::Information);
        }

        void btnGenerateReport_Click(Object^ sender, EventArgs^ e) {
            int validCount = GetValidRowCount();
            if (validCount == 0) {
                MessageBox::Show(L"Нет данных для отчета", L"Ошибка",
                    MessageBoxButtons::OK, MessageBoxIcon::Warning);
                return;
            }

            int total = 0;
            double totalPrice = 0;
            double minPrice = Double::MaxValue;
            double maxPrice = Double::MinValue;

            for each (DataGridViewRow ^ row in dataGridViewTariffs->Rows) {
                if (row->IsNewRow) continue;

                String^ priceStr = row->Cells["FinalPrice"]->Value->ToString();
                double price = Double::Parse(priceStr->Replace(",", "."));
                totalPrice += price;
                total++;

                if (price < minPrice) minPrice = price;
                if (price > maxPrice) maxPrice = price;
            }

            double avgPrice = totalPrice / total;

            String^ report = L"=== ОТЧЕТ КОМПАНИИ DESSANCARGO ===\n\n" +
                L"Общее количество тарифов: " + total.ToString() + L"\n" +
                L"Самый дешевый тариф: " + minPrice.ToString("N2") + L" руб.\n" +
                L"Самый дорогой тариф: " + maxPrice.ToString("N2") + L" руб.\n" +
                L"Средняя цена: " + avgPrice.ToString("N2") + L" руб.\n" +
                L"====================================";

            MessageBox::Show(report, L"Отчет компании",
                MessageBoxButtons::OK, MessageBoxIcon::Information);
        }

        void btnCompanyInfo_Click(Object^ sender, EventArgs^ e) {
            int count = GetValidRowCount();

            String^ info = L"=== ИНФОРМАЦИЯ О КОМПАНИИ ===\n" +
                L"Название: DessanCargo\n" +
                L"Количество тарифов: " + count.ToString() + L"\n" +
                L"Специализация: Грузоперевозки\n" +
                L"==================================";
            MessageBox::Show(info, L"Информация о компании",
                MessageBoxButtons::OK, MessageBoxIcon::Information);
        }

        void btnLoadFromFile_Click(Object^ sender, EventArgs^ e) {
            OpenFileDialog^ openFileDialog = gcnew OpenFileDialog();
            openFileDialog->Filter = "Текстовые файлы (*.txt)|*.txt|Все файлы (*.*)|*.*";
            openFileDialog->Title = "Загрузить тарифы из файла";

            // ИСПРАВЛЕНО: используем полное имя
            if (openFileDialog->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
                LoadTariffsFromFile(openFileDialog->FileName);
            }
        }

        void btnSaveToFile_Click(Object^ sender, EventArgs^ e) {
            SaveFileDialog^ saveFileDialog = gcnew SaveFileDialog();
            saveFileDialog->Filter = "Текстовые файлы (*.txt)|*.txt|Все файлы (*.*)|*.*";
            saveFileDialog->Title = "Сохранить тарифы в файл";
            saveFileDialog->FileName = "тарифы_" + DateTime::Now.ToString("yyyyMMdd_HHmm") + ".txt";

            // ИСПРАВЛЕНО: используем полное имя
            if (saveFileDialog->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
                SaveTariffsToFile(saveFileDialog->FileName);
            }
        }

        void SwapRows(int index1, int index2) {
            DataGridViewRow^ row1 = dataGridViewTariffs->Rows[index1];
            DataGridViewRow^ row2 = dataGridViewTariffs->Rows[index2];

            Object^ tempName1 = row1->Cells["Name"]->Value;
            Object^ tempBasePrice1 = row1->Cells["BasePrice"]->Value;
            Object^ tempStrategy1 = row1->Cells["Strategy"]->Value;
            Object^ tempFinalPrice1 = row1->Cells["FinalPrice"]->Value;

            Object^ tempName2 = row2->Cells["Name"]->Value;
            Object^ tempBasePrice2 = row2->Cells["BasePrice"]->Value;
            Object^ tempStrategy2 = row2->Cells["Strategy"]->Value;
            Object^ tempFinalPrice2 = row2->Cells["FinalPrice"]->Value;

            row1->Cells["Name"]->Value = tempName2;
            row1->Cells["BasePrice"]->Value = tempBasePrice2;
            row1->Cells["Strategy"]->Value = tempStrategy2;
            row1->Cells["FinalPrice"]->Value = tempFinalPrice2;

            row2->Cells["Name"]->Value = tempName1;
            row2->Cells["BasePrice"]->Value = tempBasePrice1;
            row2->Cells["Strategy"]->Value = tempStrategy1;
            row2->Cells["FinalPrice"]->Value = tempFinalPrice1;
        }
    };
}