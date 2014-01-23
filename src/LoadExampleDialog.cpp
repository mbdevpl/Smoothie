#include "LoadExampleDialog.hpp"
#include "ui_LoadExampleDialog.h"

LoadExampleDialog::LoadExampleDialog(QWidget *parent)
	: QDialog(parent), mesh(nullptr), view(nullptr), timer(new QTimer(this)),
		selectedItem(0), ui(new Ui::LoadExampleDialog)
{
	ui->setupUi(this);
	QObject::connect(ui->buttonCancel, SIGNAL(clicked()), this, SLOT(reject()));

	ui->examplesListView->setSelectionMode(QAbstractItemView::SingleSelection);

	size_t examplesCount = MeshExamples::getCount();
	for(size_t i = 0; i < examplesCount; ++i)
	{
		const auto& mesh = MeshExamples::get(i);
		ui->examplesListView->addItem(mesh.GetName().c_str());
	}

	QObject::connect(ui->examplesListView, SIGNAL(currentRowChanged(int)), this, SLOT(select(int)));

	rebuildData();

	QObject::connect(timer, SIGNAL(timeout()), this, SLOT(refreshView()));
	timer->start(500);
}

LoadExampleDialog::~LoadExampleDialog()
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

void LoadExampleDialog::rebuildMesh()
{
	// store old stuff
	Mesh* previous = mesh;

	// create new mesh object
	//mesh = static_cast<Mesh*>(trigMesh);
	mesh = const_cast<Mesh*>(&MeshExamples::get(selectedItem));

	// delete old stuff
	if(previous != nullptr)
	{
		delete previous;
		previous = nullptr;
	}
}

void LoadExampleDialog::rebuildView()
{
	// store old stuff
	MeshView* previous = view;

	// create new view object
	MeshView* view = new MeshView(mesh, Plane(0.0, -30.0, 0.0, AngleUnit::Degree));
	view->SetEditMode(false);
	view->SetHeightMode(true);
	view->SetGuidedMode(false);
	view->SetLabeledMode(false);
	view->SetTextureMode(true);
	ui->viewWidget->SetView(view);
	ui->viewWidget->SetInteractiveMode(false);
	view->UpdateData();

	// delete old stuff
	if(previous != nullptr)
	{
		delete previous;
		previous = nullptr;
	}
}

Mesh& LoadExampleDialog::getMesh()
{
	// change ownership of the mesh
	Mesh* m = mesh;
	mesh = nullptr;
	return *m;
}

void LoadExampleDialog::rebuildData()
{
	rebuildMesh();
	rebuildView();
}

void LoadExampleDialog::refreshView()
{
	//ui->examplesListView->setItemSelected(ui->examplesListView->item());
	ui->viewWidget->RefreshView();
}

void LoadExampleDialog::select(int index)
{
	timer->stop();

	QThread::msleep(100);

	selectedItem = (size_t)index;
	rebuildMesh();
	rebuildView();

	timer->start(DrawingDelay);
}

void LoadExampleDialog::on_buttonLoad_clicked()
{
	// TODO: set frame result
	//close();
	accept();
}
