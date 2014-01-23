#include "MeshView.hpp"

#ifdef WINDOWS
#define FONT_PATH "C:/Windows/Fonts/arial.ttf"
#else
#define FONT_PATH NOT_IMPLEMENTED___LINUX_FONT_PATH_NOT_DEFINED
#endif

MeshView::MeshView(Mesh* mesh)
	: mesh(mesh), view(0), viewPlane(),
	editMode(false), flatMode(false), heightMode(false),
	guidedMode(true), labeledMode(true), textureMode(false),
	font(FONT_PATH),
	r(15.0), s(10.0)
{
	if(mesh == nullptr)
		throw new std::runtime_error("mesh view must contain a mesh");
	if(font.Error())
		throw new std::runtime_error("could not load font");
	initialize();
}

MeshView::MeshView(Mesh* mesh, Plane viewPlane)
	: mesh(mesh), view(0), viewPlane(viewPlane),
	editMode(false), flatMode(false), heightMode(false),
	guidedMode(true), labeledMode(true), textureMode(false),
	font(FONT_PATH),
	r(15.0), s(10.0)
{
	if(mesh == nullptr)
		throw new std::runtime_error("mesh view must contain a mesh");
	if(font.Error())
		throw new std::runtime_error("could not load font");
	initialize();
}

MeshView::~MeshView()
{
	mesh = nullptr;
	if(view != 0)
	{
		glDeleteLists(view, 1);
		view = 0;
	}
}

void MeshView::initialize()
{
	e = 0.3;

	ptsCount = mesh->GetPointsCount();
	xa = new GLfloat[ptsCount];
	ya = new GLfloat[ptsCount];
	za = new GLfloat[ptsCount];
	ba = new bool[ptsCount];

	elemsCount = mesh->GetElementsCount();
	ecxa = new GLfloat[elemsCount];
	ecya = new GLfloat[elemsCount];
	ecza = new GLfloat[elemsCount];

	float domainWidth = mesh->GetDomainWidth();
	float domainHeight = mesh->GetDomainHeight();
	float domainDepth = mesh->GetDomainDepth();

	float maxDomainSize = std::max(domainWidth, domainHeight);
	sizeMultiplier = (2.0 * s) / maxDomainSize;
	depthMultiplier = 5.0 / domainDepth;
	widthOffset = -1.0 * (mesh->GetDomainStart().x + domainWidth * 0.5) * sizeMultiplier;
	heightOffset = -1.0 * (mesh->GetDomainStart().y + domainHeight * 0.5) * sizeMultiplier;
	depthOffset = -1.0 * (mesh->GetDomainStart().z) * depthMultiplier;

	dDepth = domainDepth * depthMultiplier;

	dsx = widthOffset + mesh->GetDomainStart().x * sizeMultiplier;
	dsy = heightOffset + mesh->GetDomainStart().y * sizeMultiplier;
	dsz = depthOffset + mesh->GetDomainStart().z * depthMultiplier;
	domainStartLabel = (
				(std::stringstream*)&(
					std::stringstream() << "(" << std::setprecision(2)
					<< mesh->GetDomainStart().x
					<< ", " << mesh->GetDomainStart().y
					//<< ", " << mesh->GetDomainStart().z
					<< ")"
				)
			)->str();
	dslabel = const_cast<char*>(domainStartLabel.c_str());

	dex = widthOffset + mesh->GetDomainEnd().x * sizeMultiplier;
	dey = heightOffset + mesh->GetDomainEnd().y * sizeMultiplier;
	dez = depthOffset + mesh->GetDomainEnd().z * depthMultiplier;
	domainEndLabel = (
				(std::stringstream*)&(
					std::stringstream() << "(" << std::setprecision(2)
					<< mesh->GetDomainEnd().x
					<< ", " << mesh->GetDomainEnd().y
					//<< ", " << mesh->GetDomainEnd().z
					<< ")"
				)
			)->str();
	delabel = const_cast<char*>(domainEndLabel.c_str());

	conn = new bool*[ptsCount];
	for(size_t i = 0; i < ptsCount; ++i)
		conn[i] = new bool[ptsCount];

	recalculate();
}

void MeshView::release()
{
	delete[] xa;
	delete[] ya;
	delete[] za;
	delete[] ba;

	delete[] ecxa;
	delete[] ecya;
	delete[] ecza;

	for(size_t i = 0; i < ptsCount; ++i)
		delete[] conn[i];
	delete[] conn;
}

void MeshView::reinitialize()
{
	release();
	initialize();
}

void MeshView::recalculate()
{
	for(size_t i = 0; i < ptsCount; ++i)
	{
		const MeshPoint& pt = mesh->GetPoint(i);
		xa[i] = widthOffset + pt.x * sizeMultiplier;
		ya[i] = heightOffset + pt.y * sizeMultiplier;
		//if(editMode)
		//	za[i] = 0.0;
		//else
		za[i] = depthOffset + pt.z * depthMultiplier;
		ba[i] = pt.boundary;
	}

	for(size_t i = 0; i < elemsCount; ++i)
	{
		const Point3D& pt = mesh->GetElement(i).centre;
		ecxa[i] = widthOffset + pt.x * sizeMultiplier;
		ecya[i] = heightOffset + pt.y * sizeMultiplier;
		//if(editMode)
		//	ecza[i] = 0.0;
		//else
		ecza[i] = depthOffset + pt.z * depthMultiplier;
	}

	color_tresholds.clear();
	color_tresholds[0] = dsz;
	color_tresholds[1] = dsz + 0.25 * dDepth;
	color_tresholds[2] = dsz + 0.5 * dDepth;
	color_tresholds[3] = dsz + 0.75 * dDepth;
	color_tresholds[4] = dez;

	for(size_t it1 = 0; it1 < ptsCount; ++it1)
		for(size_t it2 = 0; it2 < ptsCount; ++it2)
			conn[it1][it2] = false;
	//for(size_t it1 = ptsCount - 2; ; --it1)
	//{
	//	for(size_t it2 = it1 + 1; it2 < ptsCount; ++it2)
	//		conn[it1][it2] = false;
	//	if(it1 == 0)
	//		break;
	//}

	for(size_t it1 = ptsCount - 2; ; --it1)
	{
		for(size_t it2 = it1 + 1; it2 < ptsCount; ++it2)
		{
			bool connected = false;
			for(size_t i = 0; i < elemsCount; ++i)
			{
				const MeshElement& elem = mesh->GetElement(i);
				size_t elemSize = elem.points.size();

				size_t elem_pt_first_index = elem.points.at(0)->index;
				size_t elem_pt_last_index = elem.points.at(elemSize - 1)->index;
				if(  (elem_pt_first_index == it1 && elem_pt_last_index == it2)
					|| (elem_pt_first_index == it2 && elem_pt_last_index == it1) )
					connected = true;
				else
				{
					size_t elem_pt_prev_index = elem_pt_first_index;
					for(size_t j = 1; j < elemSize; ++j)
					{
						size_t elem_pt_curr_index = elem.points.at(j)->index;
						if(  (elem_pt_prev_index == it1 && elem_pt_curr_index == it2)
							|| (elem_pt_prev_index == it2 && elem_pt_curr_index == it1) )
						{
							connected = true;
							break;
						}
						elem_pt_prev_index = elem_pt_curr_index;
					}
				}

				if(connected)
					break;
			}
			if(connected)
				conn[it1][it2] = true;
		}
		if(it1 == 0)
			break;
	}
}

void MeshView::SetMesh(Mesh* mesh)
{
	this->mesh = mesh;
	reinitialize();
}

void MeshView::ChangeViewPlane(AngleType xAngleChange, AngleType yAngleChange, AngleType zAngleChange)
{
	viewPlane.Change(xAngleChange, yAngleChange, zAngleChange);
}

int MeshView::GetIndexAt(GLfloat x, GLfloat y)
{
	Point3D beg = mesh->GetDomainStart();
	Point3D end = mesh->GetDomainEnd();

	beg.x *= sizeMultiplier;
	beg.y *= sizeMultiplier;
	end.x *= sizeMultiplier;
	end.y *= sizeMultiplier;

	beg.x += widthOffset;
	beg.y += heightOffset;
	end.x += widthOffset;
	end.y += heightOffset;

	if(x <= beg.x - e || x >= end.x + e || y <= beg.y - e || y >= end.y + e)
		return -1;

	for(size_t i = 0; i < ptsCount; ++i)
	{
		bool checkX = (x > xa[i] - e && x < xa[i] + e);
		bool checkY = (y > ya[i] - e && y < ya[i] + e);
		if(checkX && checkY)
			return (int)i;
	}

	return -1;
}

void MeshView::setColour(bool boundary)
{
	if(boundary)
		glColor3f(1.0f, 1.0f, 1.0f); // boundary point colour
	else
		glColor3f(0.45f, 0.45f, 0.45f); // free point colour
}

void MeshView::setColour(double value)
{
	double height = (value - dsz) / dDepth;

	if(value < color_tresholds[0])
		glColor3f(0.0f, 0.0f, 0.5f);
	else if(value <= color_tresholds[1]) // from 0.00 to 0.25
		glColor3f(0.1f, 0.0f + (height * 4), 1.0f);
	else if(value <= color_tresholds[2]) // from 0.25 to 0.50
		glColor3f(0.0f, 1.0f, 1.0f - ((height - 0.25) * 4));
	else if(value <= color_tresholds[3]) // from 0.50 to 0.75
		glColor3f(0.0f + ((height - 0.5) * 4), 1.0f, 0.0f);
	else if(value <= color_tresholds[4]) // from 0.75 to 1.00
		glColor3f(1.0f, 1.0f - ((height - 0.75) * 4), 0.0f);
	else
		glColor3f(1.0f, 0.0f, 0.0f);
}

void MeshView::DrawBegin()
{
	if(view != 0)
	{
		glDeleteLists(view, 1);
	}

	view = glGenLists(1);
	glNewList(view, GL_COMPILE);
}

void MeshView::Draw()
{
	Plane plane = GetViewPlane();
	//Mesh& mesh = *mesh;

	glTranslatef(0.0f, 0.0f, -30.0f);
	if(!editMode)
	{
		if(plane.xAngle != 0)
			glRotatef(plane.xAngle, 0.0f, -1.0f, 0.0f);
		if(plane.yAngle != 0)
			glRotatef(plane.yAngle, 1.0f, 0.0f, 0.0f);
		if(plane.zAngle != 0)
			glRotatef(plane.zAngle, 0.0f, 0.0f, 1.0f);
	}

	if(guidedMode)
	{
		// color
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
		setColourOrbit();

		// orbit
		glBegin(GL_POINTS);
		GLfloat x = 0.0, y = 0.0; //, z = 0.0;
		for(GLdouble i = 0.0; i < PiTimes2; i += ArcJump)
		{
			x = r * sin(i);
			y = r * cos(i);
			//z = 0.0;
			glVertex3f(x, y, 0.0f);
		}
		glEnd();
	}

	//size_t ptsCount = mesh->GetPointsCount();

	// color
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	//glColor3f(0.5f, 1.0f, 0.5f); // color is set for each vertex

	// mesh, points
	glBegin(GL_POINTS);
	for(size_t i = 0; i < ptsCount; ++i)
	{
		if(heightMode)
			setColour(za[i]);
		else
			setColour(ba[i]);
		if(flatMode)
			glVertex3f(xa[i], ya[i], 0.0f);
		else
			glVertex3f(xa[i], ya[i], za[i]);
	}
	glEnd();

	if(editMode)
	{
		for(size_t i = 0; i < ptsCount; ++i)
		{
			// sphere color
			setColour(ba[i]);
			// sphere
			if(flatMode)
				glTranslatef(xa[i], ya[i], 0.0f + 0.05f);
			else
				glTranslatef(xa[i], ya[i], za[i] + 0.05f);
			GLUquadric* quad = gluNewQuadric();
			gluQuadricDrawStyle(quad, GLU_FILL);
			gluSphere(quad, e * 1, 8, 2); // TODO: dynamic sphere size, not '* 1'
			gluDeleteQuadric(quad);
			if(flatMode)
				glTranslatef(-xa[i], -ya[i], 0.0f - 0.05f);
			else
				glTranslatef(-xa[i], -ya[i], -za[i] - 0.05f);
		}
	}

	if(labeledMode)
	{
		// mesh, elements' labels and domain delimiters

		// TODO: determine font size on viewport resize
		//GLint m_viewport[4];
		//glGetIntegerv(GL_VIEWPORT, m_viewport);
		//size_t screenHeight = (size_t)m_viewport[3];
		const unsigned int size = 16;
		const unsigned int res = 72;// + (screenHeight - 745) / 10;
		font.FaceSize(size, res);

		setColourLabel();
		for(size_t i = elemsCount - 1; ; --i)
		{
			std::string label = std::to_string(i + 1);

			if(flatMode)
				glRasterPos3f(ecxa[i], ecya[i], 0.0f);
			else
				glRasterPos3f(ecxa[i], ecya[i], ecza[i]);
			font.Render(label.c_str());

			if(i == 0)
				break;
		}

		if(editMode)
			glRasterPos3f(dsx, dsy - 1.5f, 0.0f);
		else
			glRasterPos3f(dsx, dsy, -0.5f);
		font.Render(dslabel);

		if(editMode)
			glRasterPos3f(dex, dey + 0.5f, 0.0f);
		else
			glRasterPos3f(dex, dey, 0.0f);
		font.Render(delabel);
	}

	// mesh, lines
	if(textureMode)
	{
		glBegin(GL_TRIANGLES);
		if(!mesh->GetType().compare("triangular"))
			for(size_t it = elemsCount - 1; ; --it)
			{
				const MeshElement& elem = mesh->GetElement(it);
				for(auto pit = elem.points.begin(); pit != elem.points.end(); ++pit)
				{
					size_t i = (**pit).index;
					if(heightMode)
						setColour(za[i]);
					else
						setColour(ba[i]);
					if(flatMode)
						glVertex3f(xa[i], ya[i], 0.0f);
					else
						glVertex3f(xa[i], ya[i], za[i]);
				}
				if(it == 0)
					break;
			}
		else if(!mesh->GetType().compare("rectangular"))
			for(size_t it = elemsCount - 1; ; --it)
			{
				const MeshElement& elem = mesh->GetElement(it);
				// first triangle
				for(auto pit = elem.points.begin(); pit != elem.points.end()-1; ++pit)
				{
					size_t i = (**pit).index;
					if(heightMode)
						setColour(za[i]);
					else
						setColour(ba[i]);
					if(flatMode)
						glVertex3f(xa[i], ya[i], 0.0f);
					else
						glVertex3f(xa[i], ya[i], za[i]);
				}
				// second triangle
				for(auto pit = elem.points.begin()+2; pit != elem.points.end(); ++pit)
				{
					size_t i = (**pit).index;
					if(heightMode)
						setColour(za[i]);
					else
						setColour(ba[i]);
					if(flatMode)
						glVertex3f(xa[i], ya[i], 0.0f);
					else
						glVertex3f(xa[i], ya[i], za[i]);
				}
				{
					auto pit = elem.points.begin();
					size_t i = (**pit).index;
					if(heightMode)
						setColour(za[i]);
					else
						setColour(ba[i]);
					if(flatMode)
						glVertex3f(xa[i], ya[i], 0.0f);
					else
						glVertex3f(xa[i], ya[i], za[i]);
				}
				if(it == 0)
					break;
			}
		else
			throw new std::runtime_error("texturing implemented only for triangular and rectangular meshes");
		glEnd();
	}
	else
	{
		glLineWidth(1.0);
		glBegin(GL_LINES);
		for(size_t it1 = ptsCount - 2; ; --it1)
		{
			for(size_t it2 = it1 + 1; it2 < ptsCount; ++it2)
			{
				if(!conn[it1][it2])
					continue;

				//bool connected = false;
				//for(size_t i = 0; i < elemsCount; ++i)
				//{
				//	const MeshElement& elem = mesh->GetElement(i);
				//	size_t elemSize = elem.points.size();

				//	if( (elem.points.at(0)->index == it1 && elem.points.at(elemSize - 1)->index == it2)
				//		 || (elem.points.at(0)->index == it2 && elem.points.at(elemSize - 1)->index == it1) )
				//		connected = true;
				//	else
				//		for(size_t j = 1; j < elemSize; ++j)
				//			if(  (elem.points.at(j-1)->index == it1 && elem.points.at(j)->index == it2)
				//				|| (elem.points.at(j-1)->index == it2 && elem.points.at(j)->index == it1) )
				//			{
				//				connected = true;
				//				break;
				//			}

				//	if(connected)
				//		break;
				//}
				//if(!connected)
				//	continue;

				if(heightMode)
					setColour(za[it1]);
				else
					setColour(ba[it1]);
				if(flatMode)
					glVertex3f(xa[it1], ya[it1], 0.0f);
				else
					glVertex3f(xa[it1], ya[it1], za[it1]);

				if(heightMode)
					setColour(za[it2]);
				else
					setColour(ba[it2]);
				if(flatMode)
					glVertex3f(xa[it2], ya[it2], 0.0f);
				else
				glVertex3f(xa[it2], ya[it2], za[it2]);
			}
			if(it1 == 0)
				break;
		}
		glEnd();
	}

	if(!editMode)
	{
		if(plane.zAngle != 0)
			glRotatef(-plane.zAngle, 0.0f, 0.0f, 1.0f);
		if(plane.yAngle != 0)
			glRotatef(-plane.yAngle, 1.0f, 0.0f, 0.0f);
		if(plane.xAngle != 0)
			glRotatef(-plane.xAngle, 0.0f, -1.0f, 0.0f);
	}
	glTranslatef(0.0f, 0.0f, 30.0f);
}

void MeshView::DrawEnd()
{
	glEndList();
}
