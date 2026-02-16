#pragma once

#using <System.dll>
#using <System.Windows.Forms.dll>
#using <System.Drawing.dll>

// Включаем TariffWrapper.h
#include "TariffWrapper.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Drawing;

namespace DessanCargoWinForms {

    public ref class DetailsForm : public Form {
    public:
        DetailsForm() {
            tariff = nullptr;
            resultTariff = nullptr;
            isEditMode = false;
            InitializeComponent();
            InitializeForNewTariff();
        }

        DetailsForm(TariffWrapper^ existingTariff) {
            tariff = existingTariff;
            resultTariff = nullptr;
            isEditMode = true;
            InitializeComponent();
            InitializeForEditTariff();
        }

        ~DetailsForm() {
            if (components) {
                delete components;
            }
        }

        property TariffWrapper^ ResultTariff {
            TariffWrapper^ get() { return resultTariff; }
        }

    private:
        TariffWrapper^ tariff;
        TariffWrapper^ resultTariff;
        bool isEditMode;

        System::ComponentModel::Container^ components;
        TextBox^ txtName;
        TextBox^ txtPrice;
        ComboBox^ cmbStrategy;
        TextBox^ txtDiscount;
        Button^ btnSave;
        Button^ btnCancel;
        Label^ lblTitle;
        Label^ lblName;
        Label^ lblPrice;
        Label^ lblStrategy;
        Label^ lblDiscount;

        void InitializeComponent(void) {
            components = gcnew System::ComponentModel::Container();

            // Настройки формы
            this->Text = isEditMode ? L"Редактирование тарифа" : L"Добавление нового тарифа";
            this->Size = System::Drawing::Size(400, 300);
            this->StartPosition = FormStartPosition::CenterParent;
            this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
            this->MaximizeBox = false;
            this->MinimizeBox = false;

            // Заголовок
            lblTitle = gcnew Label();
            lblTitle->Font = gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, FontStyle::Bold);
            lblTitle->Location = System::Drawing::Point(20, 20);
            lblTitle->Size = System::Drawing::Size(350, 25);
            lblTitle->TextAlign = ContentAlignment::MiddleCenter;
            lblTitle->Text = isEditMode ? L"РЕДАКТИРОВАНИЕ ТАРИФА" : L"ДОБАВЛЕНИЕ ТАРИФА";
            this->Controls->Add(lblTitle);

            // Название
            lblName = gcnew Label();
            lblName->Text = L"Название тарифа:";
            lblName->Location = System::Drawing::Point(20, 60);
            lblName->Size = System::Drawing::Size(120, 20);
            this->Controls->Add(lblName);

            txtName = gcnew TextBox();
            txtName->Location = System::Drawing::Point(150, 60);
            txtName->Size = System::Drawing::Size(200, 20);
            this->Controls->Add(txtName);

            // Базовая цена
            lblPrice = gcnew Label();
            lblPrice->Text = L"Базовая цена:";
            lblPrice->Location = System::Drawing::Point(20, 90);
            lblPrice->Size = System::Drawing::Size(120, 20);
            this->Controls->Add(lblPrice);

            txtPrice = gcnew TextBox();
            txtPrice->Location = System::Drawing::Point(150, 90);
            txtPrice->Size = System::Drawing::Size(200, 20);
            this->Controls->Add(txtPrice);

            // Стратегия
            lblStrategy = gcnew Label();
            lblStrategy->Text = L"Стратегия:";
            lblStrategy->Location = System::Drawing::Point(20, 120);
            lblStrategy->Size = System::Drawing::Size(120, 20);
            this->Controls->Add(lblStrategy);

            cmbStrategy = gcnew ComboBox();
            cmbStrategy->Location = System::Drawing::Point(150, 120);
            cmbStrategy->Size = System::Drawing::Size(200, 20);
            cmbStrategy->DropDownStyle = ComboBoxStyle::DropDownList;
            cmbStrategy->Items->AddRange(gcnew array<Object^> { L"Стандартный (без скидки)", L"Со скидкой" });
            cmbStrategy->SelectedIndexChanged += gcnew EventHandler(this, &DetailsForm::cmbStrategy_SelectedIndexChanged);
            this->Controls->Add(cmbStrategy);

            // Скидка
            lblDiscount = gcnew Label();
            lblDiscount->Text = L"Процент скидки:";
            lblDiscount->Location = System::Drawing::Point(20, 150);
            lblDiscount->Size = System::Drawing::Size(120, 20);
            lblDiscount->Visible = false;
            this->Controls->Add(lblDiscount);

            txtDiscount = gcnew TextBox();
            txtDiscount->Location = System::Drawing::Point(150, 150);
            txtDiscount->Size = System::Drawing::Size(200, 20);
            txtDiscount->Visible = false;
            this->Controls->Add(txtDiscount);

            // Кнопки
            btnSave = gcnew Button();
            btnSave->Text = L"Сохранить";
            btnSave->Location = System::Drawing::Point(100, 200);
            btnSave->Size = System::Drawing::Size(90, 30);
            btnSave->Click += gcnew EventHandler(this, &DetailsForm::btnSave_Click);
            this->Controls->Add(btnSave);

            btnCancel = gcnew Button();
            btnCancel->Text = L"Отмена";
            btnCancel->Location = System::Drawing::Point(200, 200);
            btnCancel->Size = System::Drawing::Size(90, 30);
            btnCancel->Click += gcnew EventHandler(this, &DetailsForm::btnCancel_Click);
            this->Controls->Add(btnCancel);
        }

        void InitializeForNewTariff() {
            cmbStrategy->SelectedIndex = 0;
        }

        void InitializeForEditTariff() {
            if (tariff != nullptr) {
                txtName->Text = tariff->Name;
                txtPrice->Text = tariff->BasePrice.ToString();

                if (tariff->StrategyType == "DISCOUNT" || tariff->StrategyType == "Discount") {
                    cmbStrategy->SelectedIndex = 1;
                    txtDiscount->Text = tariff->DiscountPercent.ToString();
                    txtDiscount->Visible = true;
                    lblDiscount->Visible = true;
                }
                else {
                    cmbStrategy->SelectedIndex = 0;
                    txtDiscount->Visible = false;
                    lblDiscount->Visible = false;
                }
            }
        }

        void cmbStrategy_SelectedIndexChanged(Object^ sender, EventArgs^ e) {
            if (cmbStrategy->SelectedIndex == 1) {
                lblDiscount->Visible = true;
                txtDiscount->Visible = true;
            }
            else {
                lblDiscount->Visible = false;
                txtDiscount->Visible = false;
                txtDiscount->Text = "";
            }
        }

        void btnSave_Click(Object^ sender, EventArgs^ e) {
            // Проверка названия
            if (String::IsNullOrWhiteSpace(txtName->Text)) {
                MessageBox::Show(L"Введите название тарифа", L"Ошибка",
                    MessageBoxButtons::OK, MessageBoxIcon::Error);
                txtName->Focus();
                return;
            }

            // Проверка цены
            double price;
            if (!Double::TryParse(txtPrice->Text, price)) {
                MessageBox::Show(L"Введите корректную цену", L"Ошибка",
                    MessageBoxButtons::OK, MessageBoxIcon::Error);
                txtPrice->Focus();
                return;
            }

            if (price <= 0) {
                MessageBox::Show(L"Цена должна быть положительной", L"Ошибка",
                    MessageBoxButtons::OK, MessageBoxIcon::Error);
                txtPrice->Focus();
                return;
            }

            if (price > 1000000) {
                MessageBox::Show(L"Цена не может превышать 1,000,000 руб.", L"Ошибка",
                    MessageBoxButtons::OK, MessageBoxIcon::Error);
                txtPrice->Focus();
                return;
            }

            // Создаем результат
            resultTariff = gcnew TariffWrapper();
            resultTariff->Name = txtName->Text;
            resultTariff->BasePrice = price;

            // ИСПРАВЛЕНО: Правильно определяем стратегию
            if (cmbStrategy->SelectedIndex == 0) {
                resultTariff->StrategyType = "STANDARD";
                resultTariff->DiscountPercent = 0;
            }
            else {
                // Проверка скидки
                if (String::IsNullOrWhiteSpace(txtDiscount->Text)) {
                    MessageBox::Show(L"Введите процент скидки", L"Ошибка",
                        MessageBoxButtons::OK, MessageBoxIcon::Error);
                    txtDiscount->Focus();
                    return;
                }

                double discount;
                if (!Double::TryParse(txtDiscount->Text, discount)) {
                    MessageBox::Show(L"Введите корректный процент скидки", L"Ошибка",
                        MessageBoxButtons::OK, MessageBoxIcon::Error);
                    txtDiscount->Focus();
                    return;
                }

                if (discount < 0 || discount > 100) {
                    MessageBox::Show(L"Скидка должна быть от 0 до 100", L"Ошибка",
                        MessageBoxButtons::OK, MessageBoxIcon::Error);
                    txtDiscount->Focus();
                    return;
                }

                resultTariff->StrategyType = "DISCOUNT";  // Важно: именно "DISCOUNT", не "Discount"
                resultTariff->DiscountPercent = discount;
            }

            this->DialogResult = System::Windows::Forms::DialogResult::OK;
            this->Close();
        }

        void btnCancel_Click(Object^ sender, EventArgs^ e) {
            this->DialogResult = System::Windows::Forms::DialogResult::Cancel;
            this->Close();
        }
    };
}