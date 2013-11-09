#include "MatricesDialog.hpp"
#include "ui_MatricesDialog.h"

MatricesDialog::MatricesDialog(QWidget *parent)
	: QDialog(parent), calc(nullptr), ui(new Ui::MatricesDialog), fontBold(nullptr),
	stiffnessMatrixLabels(), massMatrixLabels(), resultMatrixLabels()
{
	ui->setupUi(this);

	fontBold = new QFont(this->font());
	fontBold->setBold(true);
}

MatricesDialog::~MatricesDialog()
{
	delete ui;
	delete fontBold;
}

void MatricesDialog::SetCalculator(const FluidFlowCalculator& calc)
{
	if(&calc == this->calc)
		return;

	const Mesh& mesh = calc.GetMesh();
	size_t size = mesh.GetPointsCount();

	if(this->calc != nullptr)
	{
		// remove all previous content from the dialog
		for(auto itx = stiffnessMatrixLabels.begin(); itx != stiffnessMatrixLabels.end(); ++itx)
			for(auto ity = itx->begin(); ity != itx->end(); ++ity)
			{
				QLabel* l = *ity;
				ui->viewStiffnessMatrix->removeWidget(l);
				delete l;
			}

		for(auto it = massMatrixLabels.begin(); it != massMatrixLabels.end(); ++it)
		{
			QLabel* l = *it;
			ui->viewMassMatrix->removeWidget(l);
			delete l;
		}

		for(auto it = resultMatrixLabels.begin(); it != resultMatrixLabels.end(); ++it)
		{
			QLabel* l = *it;
			ui->viewResultMatrix->removeWidget(l);
			delete l;
		}
	}

	this->calc = const_cast<FluidFlowCalculator*>(&calc);

	// create new vectors for labels
	stiffnessMatrixLabels = std::vector<std::vector<QLabel*> >(size);
	for(auto itx = stiffnessMatrixLabels.begin(); itx != stiffnessMatrixLabels.end(); ++itx)
		*itx = std::vector<QLabel*>(size);
	massMatrixLabels = std::vector<QLabel*>(size);
	resultMatrixLabels = std::vector<QLabel*>(size);

	// assign correct textual value to each label
	for(size_t y = 0; y < size; ++y)
	{
		for(size_t x = 0; x < size; ++x)
		{
			auto stiffness = calc.stiffness(x, y);
			QLabel* stiffnessLabel = nullptr;
			if(stiffness != 0)
				stiffnessLabel = new QLabel(QString( std::to_string(stiffness, 3).c_str() ));
			else
				stiffnessLabel = new QLabel(QString(" "));
			stiffnessLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
			stiffnessMatrixLabels[x][y] = stiffnessLabel;
			ui->viewStiffnessMatrix->addWidget(stiffnessLabel, y, x);
		}
		QLabel* massLabel = new QLabel(QString( std::to_string(calc.mass(y), 3).c_str() ));
		massLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
		massMatrixLabels[y] = massLabel;
		ui->viewMassMatrix->addWidget(massLabel);

		QLabel* resultLabel = new QLabel(QString( std::to_string(calc.result(y), 3).c_str() ));
		resultLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
		if(! mesh.GetPoint(y).boundary)
			resultLabel->setFont(*fontBold);
		resultMatrixLabels[y] = resultLabel;
		ui->viewResultMatrix->addWidget(resultLabel);
	}
}
