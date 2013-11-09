#pragma once

#include "FluidFlowCalculator.hpp"

namespace Ui {
class MatricesDialog;
}

/*!
 * \brief The dialog used to view the matrices that are used to calculate
 *  the final coordinates of each point.
 */
class MatricesDialog : public QDialog
{
	Q_OBJECT

private:
	FluidFlowCalculator* calc;
	Ui::MatricesDialog *ui;
	QFont* fontBold;
	std::vector<std::vector<QLabel*> > stiffnessMatrixLabels;
	std::vector<QLabel*> massMatrixLabels;
	std::vector<QLabel*> resultMatrixLabels;
	
public:
	explicit MatricesDialog(QWidget *parent = 0);
	~MatricesDialog();
	void SetCalculator(const FluidFlowCalculator& calc);

};
