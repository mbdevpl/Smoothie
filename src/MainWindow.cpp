#include "MainWindow.hpp"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent), mesh(nullptr), view(nullptr), calc(nullptr), calcThread(nullptr),
		timer(new QTimer(this)), ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	this->setWindowTitle("Smoothie - static fluid flow simulator");
	QObject::connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));
	QObject::connect(ui->actionStart, SIGNAL(triggered()), this, SLOT(startedCalculation()));

	mesh = &MeshExamples::get(0);
	view = new MeshView(mesh, Plane(0.0, -50.0, 0.0, AngleUnit::Degree));
	ui->viewWidget->SetView(view);

	QObject::connect(timer, SIGNAL(timeout()), this, SLOT(redrawGL()));
	timer->start(DrawingDelay);
}

MainWindow::~MainWindow()
{
	delete ui;
	if(calc != nullptr)
	{
		delete calc;
		calc = nullptr;
	}
}

void MainWindow::redrawGL()
{
	ui->viewWidget->RefreshView();
}

void MainWindow::startedCalculation()
{
	this->timer->stop();
	this->setEnabled(false);

	if(calc != nullptr)
	{
		delete calc;
		calc = nullptr;
	}
	if(calcThread != nullptr)
	{
		delete calcThread;
		calcThread = nullptr;
	}

	if(typeid(*mesh) == typeid(RectangularMesh))
		calc = new RectangularFluidFlowCalculator(*static_cast<RectangularMesh*>(mesh));
	else if(typeid(*mesh) == typeid(TriangularMesh))
		calc = new TriangukarFluidFlowCalculator(*static_cast<TriangularMesh*>(mesh));

	if(calc == nullptr)
		throw new std::runtime_error("sorry, the current kind of mesh is not handled by any existing calculator");

	calcThread = new CalculatorThread(*calc);
	QObject::connect(calc, SIGNAL(progress(size_t)), this, SLOT(setCalculationProgress(size_t)));
	QObject::connect(calc, SIGNAL(finished()), this, SLOT(finishedCalculation()));
	calcThread->start();
}

void MainWindow::setCalculationProgress(size_t percent)
{
	// not very useful now, as everything is calculated very quickly
	if(percent > 100)
		throw new std::runtime_error("progressing much?");
}

void MainWindow::finishedCalculation()
{
	for(size_t i = 0; i < mesh->GetPointsCount(); ++i)
		if(! mesh->pt(i).boundary)
			mesh->pt(i).z = calc->result(i);
	mesh->recalculate();

	view->UpdateData();

	this->setEnabled(true);
	this->timer->start(DrawingDelay);
}

void MainWindow::on_actionAbout_triggered()
{
	QMessageBox::about(this, "Smoothie",
			"A project for 'From Finite Element Method To Signal Analysis'"
			", a Computer Science course at Warsaw University of Technology."
			"\n\n"
			"This application is designed to solve static heat transfer (or static fuild flow) problem"
			" using Finite Element Method. Either triangular or rectangular elements can be used."
			" For details, please refer to the documentation."
			"\n\n"
			"(c) 2013-2014 Mateusz Bysiek  http://mbdev.pl/\n");
}

void MainWindow::on_actionNew_triggered()
{
	timer->stop();
	NewMeshDialog* dialog = new NewMeshDialog(this);
	if(dialog->exec() == QDialog::Accepted)
	{
		// backup old stuff
		Mesh* previousMesh = mesh;
		FluidFlowCalculator* previousCalc = calc;

		// replace old stuff with new
		mesh = &dialog->getMesh();
		view->SetMesh(mesh);
		view->SetViewPlane(Plane(0.0, -50.0, 0.0, AngleUnit::Degree));
		calc = nullptr;

		// delete old stuff
		if(previousCalc != nullptr)
		{
			delete previousCalc;
			previousCalc = nullptr;
		}
		if(previousMesh != nullptr)
		{
			delete previousMesh;
			previousMesh = nullptr;
		}
	}
	delete dialog;
	timer->start(DrawingDelay);
}

void MainWindow::on_actionLoad_triggered()
{
	timer->stop();
	QString filename = QFileDialog::getOpenFileName(this, "Smoothie - Load mesh from file", ".",
			"XML (*.xml);; CSV (*.csv);; Agimes file format (*.pem)");

	//if(dialog->exec() == QDialog::Accepted)
	if(filename.length() > 0)
	{
		//if(filename.length() == 0)
		//	throw new std::runtime_error("cannot load a file when its path is udefined");

		// backup old stuff
		Mesh* previousMesh = mesh;
		FluidFlowCalculator* previousCalc = calc;

		//MeshParameters params(filename);
		//switch(params.kind)
		//{
		//case MeshKind::Triangular:
		//	mesh = new TriangularMesh(params.width, params.height, params.domainStart, params.domainEnd);
		//	break;
		//case MeshKind::Rectangular:
		//	mesh = new RectangularMesh(params.width, params.height, params.domainStart, params.domainEnd);
		//	break;
		//}

		// replace old stuff with new
		IrregularMesh temporaryMesh(filename.toStdString());

		Mesh* newMesh = nullptr;
		if(temporaryMesh.GetType().compare("triangular") == 0)
			newMesh = new TriangularMesh(temporaryMesh);
		else if(temporaryMesh.GetType().compare("rectangular") == 0)
			newMesh = new RectangularMesh(temporaryMesh);

		if(newMesh == nullptr)
			throw new std::runtime_error("unable to create a valid mesh from the loaded file");

		mesh = newMesh;
		view->SetMesh(mesh);
		view->SetViewPlane(Plane(0.0, -50.0, 0.0, AngleUnit::Degree));
		calc = nullptr;

		// delete old stuff
		if(previousCalc != nullptr)
		{
			delete previousCalc;
			previousCalc = nullptr;
		}
		if(previousMesh != nullptr)
		{
			delete previousMesh;
			previousMesh = nullptr;
		}
	}
	timer->start(DrawingDelay);
}

void MainWindow::on_actionLoadExample_triggered()
{
	timer->stop();
	LoadExampleDialog* dialog = new LoadExampleDialog(this);
	if(dialog->exec() == QDialog::Accepted)
	{
		// backup old stuff
		Mesh* previousMesh = mesh;
		FluidFlowCalculator* previousCalc = calc;

		// replace old stuff with new
		mesh = &dialog->getMesh();
		view->SetMesh(mesh);
		view->SetViewPlane(Plane(0.0, -50.0, 0.0, AngleUnit::Degree));
		calc = nullptr;

		// delete old stuff
		if(previousCalc != nullptr)
		{
			delete previousCalc;
			previousCalc = nullptr;
		}
		if(previousMesh != nullptr)
		{
			delete previousMesh;
			previousMesh = nullptr;
		}
	}
	delete dialog;
	timer->start(DrawingDelay);
}

void MainWindow::on_actionSave_triggered()
{
	timer->stop();
	QString filename = QFileDialog::getSaveFileName(this, "Smoothie - Save mesh to file", ".",
			"XML (*.xml)");
	//if(dialog->exec() == QDialog::Accepted)
	if(filename.length() > 0)
	{
		//if(filename.length() == 0)
		//	throw new std::runtime_error("cannot save a file when the target path is udefined");

		mesh->saveToFile(filename.toStdString());
	}
	timer->start(DrawingDelay);
}

void MainWindow::on_actionView_triggered()
{
	// state here is after the actual checking/unchecking
	if(!ui->actionView->isChecked())
	{
		ui->actionView->setChecked(true);
		return;
	}
	ui->actionEdit->setChecked(false);

	MeshView* view = (MeshView*)ui->viewWidget->GetView();
	view->SetEditMode(false);
}

void MainWindow::on_actionEdit_triggered()
{
	if(!ui->actionEdit->isChecked())
	{
		ui->actionEdit->setChecked(true);
		return;
	}
	ui->actionView->setChecked(false);

	MeshView* view = (MeshView*)ui->viewWidget->GetView();
	view->SetEditMode(true);
}

void MainWindow::on_actionColourBoundary_triggered()
{
	if(!ui->actionColourBoundary->isChecked())
	{
		ui->actionColourBoundary->setChecked(true);
		return;
	}
	ui->actionColourValue->setChecked(false);

	MeshView* view = (MeshView*)ui->viewWidget->GetView();
	view->SetHeightMode(false);
}

void MainWindow::on_actionColourValue_triggered()
{
	if(!ui->actionColourValue->isChecked())
	{
		ui->actionColourValue->setChecked(true);
		return;
	}
	ui->actionColourBoundary->setChecked(false);

	MeshView* view = (MeshView*)ui->viewWidget->GetView();
	view->SetHeightMode(true);
}

void MainWindow::on_actionShowMesh_triggered()
{
	if(!ui->actionShowMesh->isChecked())
	{
		ui->actionShowMesh->setChecked(true);
		return;
	}
	ui->actionShowTextures->setChecked(false);

	MeshView* view = const_cast<MeshView*>(ui->viewWidget->GetView());
	view->SetTextureMode(false);
}

void MainWindow::on_actionShowTextures_triggered()
{
	if(!ui->actionShowTextures->isChecked())
	{
		ui->actionShowTextures->setChecked(true);
		return;
	}
	ui->actionShowMesh->setChecked(false);

	MeshView* view = const_cast<MeshView*>(ui->viewWidget->GetView());
	view->SetTextureMode(true);
}

void MainWindow::on_actionShowLabels_triggered()
{
	if(!ui->actionShowLabels->isChecked())
	{
		ui->actionShowLabels->setChecked(true);
		return;
	}
	ui->actionHideLabels->setChecked(false);

	MeshView* view = const_cast<MeshView*>(ui->viewWidget->GetView());
	view->SetLabeledMode(true);
}

void MainWindow::on_actionHideLabels_triggered()
{
	if(!ui->actionHideLabels->isChecked())
	{
		ui->actionHideLabels->setChecked(true);
		return;
	}
	ui->actionShowLabels->setChecked(false);

	MeshView* view = const_cast<MeshView*>(ui->viewWidget->GetView());
	view->SetLabeledMode(false);
}

void MainWindow::on_actionMatrices_triggered()
{
	if(calc == nullptr)
		return;

	timer->stop();

	MatricesDialog* dialog = new MatricesDialog(this);
	dialog->SetCalculator(*calc);
	dialog->exec();
	delete dialog;

	timer->start(DrawingDelay);
}
