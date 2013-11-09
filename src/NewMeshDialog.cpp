#include "NewMeshDialog.hpp"
#include "ui_NewMeshDialog.h"

NewMeshDialog::NewMeshDialog(QWidget *parent)
	: QDialog(parent), mesh(nullptr), view(nullptr), timer(new QTimer(this)), ui(new Ui::NewMeshDialog)
{
	ui->setupUi(this);
	QObject::connect(ui->buttonCancel, SIGNAL(clicked()), this, SLOT(reject()));

	QObject::connect(ui->radioButtonTriangular, SIGNAL(toggled(bool)), this, SLOT(rebuildData()));
	QObject::connect(ui->spinBoxHeight, SIGNAL(valueChanged(int)), this, SLOT(rebuildData()));
	QObject::connect(ui->spinBoxWidth, SIGNAL(valueChanged(int)), this, SLOT(rebuildData()));
	QObject::connect(ui->boundariesBox, SIGNAL(toggled(bool)), this, SLOT(rebuildData()));
	QObject::connect(ui->domainStartX, SIGNAL(valueChanged(double)), this, SLOT(rebuildData()));
	QObject::connect(ui->domainStartY, SIGNAL(valueChanged(double)), this, SLOT(rebuildData()));
	QObject::connect(ui->domainEndX, SIGNAL(valueChanged(double)), this, SLOT(rebuildData()));
	QObject::connect(ui->domainEndY, SIGNAL(valueChanged(double)), this, SLOT(rebuildData()));

	rebuildData();

	QObject::connect(timer, SIGNAL(timeout()), this, SLOT(refreshView()));
	timer->start(500);
}

NewMeshDialog::~NewMeshDialog()
{
	timer->stop();
	delete timer;

	delete ui;

	if(view != nullptr)
	{
		delete view;
		view = nullptr;
	}
	if(mesh != nullptr)
	{
		delete mesh;
		mesh = nullptr;
	}
}

void NewMeshDialog::rebuildMesh()
{
	// store old stuff
	Mesh* previous = mesh;

	// gather mesh parameters from UI
	size_t width = (size_t)ui->spinBoxWidth->value();
	size_t height = (size_t)ui->spinBoxHeight->value();
	bool boundaries = ui->boundariesBox->isChecked();
	Point3D domainStart(ui->domainStartX->value(), ui->domainStartY->value(), ui->domainStartZ->value());
	Point3D domainEnd(ui->domainEndX->value(), ui->domainEndY->value(), ui->domainEndZ->value());

	// create new mesh object
	if(ui->radioButtonTriangular->isChecked())
	{
		TriangularMesh* trigMesh = new TriangularMesh(width, height, domainStart, domainEnd, boundaries);
		mesh = static_cast<Mesh*>(trigMesh);
	}
	else if(ui->radioButtonRectangular->isChecked())
	{
		RectangularMesh* rectMesh = new RectangularMesh(width, height, domainStart, domainEnd, boundaries);
		mesh = static_cast<Mesh*>(rectMesh);
	}

	// delete old stuff
	if(previous != nullptr)
	{
		delete previous;
		previous = nullptr;
	}
}

void NewMeshDialog::rebuildView()
{
	// store old stuff
	MeshView* previous = view;

	// create new view object
	MeshView* view = new MeshView(mesh, Plane(0.0, -50.0, 0.0, AngleUnit::Degree));
	view->SetEditMode(true);
	ui->viewWidget->SetView(view);
	view->UpdateData();

	// delete old stuff
	if(previous != nullptr)
	{
		delete previous;
		previous = nullptr;
	}
}

Mesh& NewMeshDialog::getMesh()
{
	// change ownership of the mesh
	Mesh* m = mesh;
	mesh = nullptr;
	return *m;
}

void NewMeshDialog::rebuildData()
{
	rebuildMesh();
	rebuildView();
}

void NewMeshDialog::refreshView()
{
	ui->viewWidget->RefreshView();
}

void NewMeshDialog::on_buttonCreate_clicked()
{
	// TODO: set frame result
	//close();
	accept();
}
