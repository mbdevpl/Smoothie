#include "FluidFlowView.hpp"

FluidFlowView::FluidFlowView(QWidget *parent)
	: QGLWidget(parent), viewObject(nullptr), view(0), lastPos(QPoint(0,0)),
		lastFrame(nullptr), interactiveMode(true)
{
}

FluidFlowView::~FluidFlowView()
{
	makeCurrent();
	if(viewObject != nullptr)
	{
		delete viewObject;
		viewObject = nullptr;
		view = 0;
	}
}

QSize FluidFlowView::sizeHint() const
{
	return QSize(800, 600);
}

void FluidFlowView::initializeGL()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}

void FluidFlowView::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	if(view != 0)
		glCallList(view);
}

void FluidFlowView::resizeGL(int width, int height)
{
	GLdouble ratio = ((GLdouble)width) / height;
	static const GLdouble vertical = 5;
	GLdouble horizontal = vertical * ratio;

	glViewport(0, 0, (GLint)width, (GLint)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-horizontal, horizontal, -vertical, vertical, 10.0, 50.0);
	glMatrixMode(GL_MODELVIEW);
}

void FluidFlowView::mousePressEvent(QMouseEvent *event)
{
	if(!interactiveMode)
		return;

	lastPos = event->pos();
	if(viewObject->GetEditMode())
	{
#ifdef DEBUG
		qDebug() << lastPos.x() << " " << lastPos.y();
#endif
		int x = lastPos.x();
		int y = lastPos.y();
		GLdouble ox=0.0,oy=0.0,oz=0.0;

		GLint viewport[4];
		GLdouble modelview[16],projection[16];
		GLfloat wx=x,wy,wz;

		glGetIntegerv(GL_VIEWPORT,viewport);
		y=viewport[3]-y;
		wy=y;
		glGetDoublev(GL_MODELVIEW_MATRIX,modelview);
		glGetDoublev(GL_PROJECTION_MATRIX,projection);
		glReadPixels(x,y,1,1,GL_DEPTH_COMPONENT,GL_FLOAT,&wz);
		gluUnProject(wx,wy,wz,modelview,projection,viewport,&ox,&oy,&oz);

		//qDebug() << ox << " " << oy;

		//int indexx = viewObject->GetNodeXAtCoord(ox);
		//int indexy = viewObject->GetNodeYAtCoord(oy);
		int index = viewObject->GetIndexAt(ox, oy);

		//qDebug() << indexx << " " << indexy;

		if (event->buttons() & Qt::LeftButton)
		{
			// TODO: point movement
		}
		else if (event->buttons() & Qt::RightButton)
		{
			// point edition frame
			if(lastFrame != nullptr)
			{
				lastFrame->hide();
				delete lastFrame;
				lastFrame = nullptr;
			}

			if(index < 0)
				return;
			const Mesh& mesh = *(viewObject->GetMesh());
			const MeshPoint& pt = mesh.GetPoint((size_t)index);

			QRect rect; // = ui->viewWidget->geometry();
			rect.setLeft(15+lastPos.x());
			rect.setRight(15+lastPos.x()+162);
			rect.setTop(15+lastPos.y());
			rect.setBottom(15+lastPos.y()+53);

			lastFrame = new PointFrame((QWidget*)(this->parent()));
			lastFrame->SetDomainUsingMesh(mesh);
			lastFrame->setGeometry(rect);
			lastFrame->SetIndex(index);
			lastFrame->SetX(pt.x);
			lastFrame->SetY(pt.y);
			lastFrame->SetZ(pt.z);
			lastFrame->SetBoundary(pt.boundary);
			lastFrame->show();

			QObject::connect(lastFrame, SIGNAL(saved()), this, SLOT(updateMesh()));
		}
		else if (event->buttons() & Qt::MiddleButton)
		{
			// TODO: toggle 'point is boundary'
		}
	}
}

void FluidFlowView::updateMesh()
{
	if(lastFrame == nullptr)
		return;

	size_t index = lastFrame->GetIndex();

	if(index >= 0)
	{
		float x = lastFrame->GetX();
		float y = lastFrame->GetY();
		float z = lastFrame->GetZ();
		bool boundary = lastFrame->GetBoundary();

		Mesh& mesh = *const_cast<Mesh*>(viewObject->GetMesh());
		MeshPoint& pt = mesh.pt(index);
		pt.x = x;
		pt.y = y;
		pt.z = z;
		pt.boundary = boundary;
		mesh.recalculate();
		viewObject->UpdateData();
	}

	lastFrame->hide();
	delete lastFrame;
	lastFrame = nullptr;
}

void FluidFlowView::mouseMoveEvent(QMouseEvent *event)
{
	if(!interactiveMode)
		return;

	if(viewObject->GetEditMode())
		return;

	float dx = event->x() - lastPos.x();
	float dy = event->y() - lastPos.y();
	lastPos = event->pos();

	const int divisorX = this->width() / 60;
	const int divisorY = this->height() / 40;

	if (event->buttons() & Qt::LeftButton)
	{
		viewObject->ChangeViewPlane(0, dy/divisorY, dx/divisorX);
	}
}

void FluidFlowView::keyPressEvent(QKeyEvent *event)
{
	if(!interactiveMode)
		return;

	int keyDisplayed = event->key();
	Qt::Key keys = static_cast<Qt::Key>(keyDisplayed);

	if(keys == Qt::Key_Control// ||
		//keys == Qt::Key_Shift ||
		//keys == Qt::Key_Alt ||
		//keys == Qt::Key_Meta
		)
	{
		//qDebug() << "Single press of special key: Ctrl, Shift, Alt or Meta";
		//if (keys == Qt::Key_Meta)
		//	qDebug() << "meta: " << QKeySequence(Qt::META);
		//keyDisplayed = 0;
		viewObject->SetEditMode(true);
	}
	event->accept();
}

void FluidFlowView::keyReleaseEvent(QKeyEvent *event)
{
	if(!interactiveMode)
		return;

	int keyDisplayed = event->key();
	Qt::Key keys = static_cast<Qt::Key>(keyDisplayed);

	if(keys == Qt::Key_Control// ||
		//keys == Qt::Key_Shift ||
		//keys == Qt::Key_Alt ||
		//keys == Qt::Key_Meta
		)
	{
		//qDebug() << "Single release of special key: Ctrl, Shift, Alt or Meta";
		viewObject->SetEditMode(false);
	}
	event->accept();
}

void FluidFlowView::SetView(MeshView* view)
{
	this->viewObject = view;
	makeCurrent();
	viewObject->DrawBegin();
	viewObject->Draw();
	viewObject->DrawEnd();
	this->view = viewObject->GetView();
}

void FluidFlowView::RefreshView()
{
	if(viewObject == nullptr)
		return;
	makeCurrent();
	viewObject->DrawBegin();
	viewObject->Draw();
	viewObject->DrawEnd();
	view = viewObject->GetView();
	updateGL();
}
