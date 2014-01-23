#ifndef LOADEXAMPLEDIALOG_HPP
#define LOADEXAMPLEDIALOG_HPP

#include <QDialog>

#include "MeshView.hpp"
#include "MeshExamples.hpp"

namespace Ui {
class LoadExampleDialog;
}

class LoadExampleDialog : public QDialog
{
	Q_OBJECT

private:
	Mesh* mesh;
	MeshView* view;
	QTimer* timer;
	size_t selectedItem;
	Ui::LoadExampleDialog *ui;

public:
	explicit LoadExampleDialog(QWidget *parent = 0);
	~LoadExampleDialog();

private:
	void rebuildMesh();
	void rebuildView();

public:
	Mesh& getMesh();

private slots:
	void rebuildData();
	void refreshView();
	void select(int index);
	void on_buttonLoad_clicked();

};

#endif // LOADEXAMPLEDIALOG_HPP
