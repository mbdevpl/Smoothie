#pragma once

#include "TriangularMesh.hpp"
#include "RectangularMesh.hpp"
#include "MeshView.hpp"

namespace Ui {
class NewMeshDialog;
}

/*!
 * \brief The dialog used to quickly create a mesh.
 */
class NewMeshDialog : public QDialog
{
	Q_OBJECT

private:
	Mesh* mesh;
	MeshView* view;
	QTimer* timer;
	Ui::NewMeshDialog *ui;

public:
	explicit NewMeshDialog(QWidget *parent = 0);
	~NewMeshDialog();

private:
	void rebuildMesh();
	void rebuildView();

public:
	Mesh& getMesh();

private slots:
	void rebuildData();
	void refreshView();
	void on_buttonCreate_clicked();

};
