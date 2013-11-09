#ifndef POINTFRAME_HPP
#define POINTFRAME_HPP

#include "Mesh.hpp"

namespace Ui {
class PointFrame;
}

/*!
 * \brief A frame used to quickly edit precise point coordinates and properties.
 */
class PointFrame : public QFrame
{
	Q_OBJECT

private:
	size_t index;
	Ui::PointFrame *ui;
	
public:
	explicit PointFrame(QWidget *parent = 0);
	~PointFrame();

public:
	void SetDomainUsingMesh(const Mesh& mesh);
	void SetX(float x);
	void SetY(float y);
	void SetZ(float val);
	void SetIndex(size_t index) { this->index = index; }
	float GetX();
	float GetY();
	float GetZ();
	float GetIndex() { return index; }
	void SetBoundary(bool set);
	bool GetBoundary();

signals:
	void saved();
	void cancelled();

private slots:
	void on_buttonUpdate_clicked();
	void on_buttonCancel_clicked();

};

#endif // POINTFRAME_HPP
