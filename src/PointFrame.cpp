#include "PointFrame.hpp"
#include "ui_PointFrame.h"

PointFrame::PointFrame(QWidget *parent)
	: QFrame(parent), index(-1), ui(new Ui::PointFrame)
{
	ui->setupUi(this);
}

PointFrame::~PointFrame()
{
	delete ui;
}

void PointFrame::SetDomainUsingMesh(const Mesh& mesh)
{
	ui->xBox->setMinimum(mesh.GetDomainStart().x);
	ui->xBox->setMaximum(mesh.GetDomainEnd().x);
	ui->yBox->setMinimum(mesh.GetDomainStart().y);
	ui->yBox->setMaximum(mesh.GetDomainEnd().y);
	ui->valueBox->setMinimum(mesh.GetDomainStart().z);
	ui->valueBox->setMaximum(mesh.GetDomainEnd().z);
}

void PointFrame::SetX(float val)
{
	ui->xBox->setValue(val);
}

float PointFrame::GetX()
{
	return (float)ui->xBox->value();
}

void PointFrame::SetY(float val)
{
	ui->yBox->setValue(val);
}

float PointFrame::GetY()
{
	return (float)ui->yBox->value();
}

void PointFrame::SetZ(float val)
{
	ui->valueBox->setValue(val);
}

float PointFrame::GetZ()
{
	return (float)ui->valueBox->value();
}

void PointFrame::SetBoundary(bool set)
{
	ui->boundaryBox->setChecked(set);
}

bool PointFrame::GetBoundary()
{
	return ui->boundaryBox->isChecked();
}

void PointFrame::on_buttonUpdate_clicked()
{
	this->hide();
	emit saved();
}

void PointFrame::on_buttonCancel_clicked()
{
	this->hide();
	emit cancelled();
}
