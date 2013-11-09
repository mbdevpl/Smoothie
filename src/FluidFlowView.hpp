#ifndef FLUIDFLOWVIEW_HPP
#define FLUIDFLOWVIEW_HPP

#include "MeshView.hpp"
#include "PointFrame.hpp"

/*!
 * \brief The Qt OpenGL control to render the mesh.
 */
class FluidFlowView : public QGLWidget
{
	Q_OBJECT

private:

	MeshView* viewObject;
	GLuint view;

	QPoint lastPos;
	PointFrame* lastFrame;

public:

	explicit FluidFlowView(QWidget *parent = 0);
	virtual ~FluidFlowView();
	QSize sizeHint() const;

protected:

	void initializeGL();
	void paintGL();
	void resizeGL(int width, int height);

	virtual void mousePressEvent(QMouseEvent *event);
	virtual void mouseMoveEvent(QMouseEvent *event);
	virtual void keyPressEvent(QKeyEvent *event);
	virtual void keyReleaseEvent(QKeyEvent *event);

public:

	void SetView(MeshView* view);
	inline const MeshView* GetView() const { return viewObject; }
	void RefreshView();

signals:
	
public slots:
	/*!
	 * \brief Meant to be used after an action related to point frame.
	 */
	void updateMesh();
	
};

#endif // FLUIDFLOWVIEW_HPP
