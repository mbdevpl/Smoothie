#pragma once

#include "TriangukarFluidFlowCalculator.hpp"
#include "RectangularFluidFlowCalculator.hpp"
#include "MeshExamples.hpp"
#include "IrregularMesh.hpp"

#include "FluidFlowView.hpp"
#include "NewMeshDialog.hpp"
#include "LoadExampleDialog.hpp"
#include "MatricesDialog.hpp"
#include "CalculatorThread.hpp"

namespace Ui {
class MainWindow;
}

/*!
 * \brief The Main window of the application.
 */
class MainWindow : public QMainWindow
{
	Q_OBJECT

private:
	Mesh* mesh;
	MeshView* view;
	FluidFlowCalculator* calc;
	CalculatorThread* calcThread;
	QTimer* timer;
	Ui::MainWindow *ui;

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

private slots:
	void redrawGL();
	void startedCalculation();
	void setCalculationProgress(size_t percent);
	void finishedCalculation();
	/*!
	 * \brief Trigerred when 'About' menu option is clicked.
	 */
	void on_actionAbout_triggered();
	/*!
	 * \brief Trigerred when 'New' menu option is clicked.
	 */
	void on_actionNew_triggered();
	/*!
	 * \brief Trigerred when 'Load' menu option is clicked.
	 */
	void on_actionLoad_triggered();
	/*!
	 * \brief Trigerred when 'Load example' menu option is clicked.
	 */
	void on_actionLoadExample_triggered();
	/*!
	 * \brief Trigerred when 'Save' menu option is clicked.
	 */
	void on_actionSave_triggered();

	void on_actionView_triggered();
	void on_actionEdit_triggered();
	void on_actionColourBoundary_triggered();
	void on_actionColourValue_triggered();
	void on_actionShowMesh_triggered();
	void on_actionShowTextures_triggered();

	void on_actionMatrices_triggered();

};
