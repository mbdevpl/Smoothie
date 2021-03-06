#pragma once

#include "Mesh.hpp"
#include "Plane.hpp"

/*!
 * \brief The OpenGL wrapper that allows to see the Mesh class in several modes.
 */
class MeshView
{
private:
	/*!
	 * \brief The mesh that is viewed.
	 */
	Mesh* mesh;

	/*!
	 * \brief OpenGL list that stores the current data to be rendered.
	 */
	GLuint view;

	/*!
	 * \brief Collection of angles at which the mesh is seen.
	 */
	Plane viewPlane;

	/*!
	 * \brief If true, the mesh is editable.
	 *  If false, it is not.
	 */
	bool editMode;

	/*!
	 * \brief If true, the z coordinates of vertices are ignored during rendering.
	 *  If false, they are not ignored.
	 */
	bool flatMode;

	/*!
	 * \brief If true, the z coordinate of every vertex gives its colour;
	 *  if false, the boundary status of vertex gives its colour.
	 */
	bool heightMode;

	/*!
	 * \brief If true, the guiding circle is drawn around the mesh.
	 *  If false, it is not.
	 */
	bool guidedMode;

	/*!
	 * \brief If true, text labels on and around the mesh are displayed.
	 */
	bool labeledMode;

	/*!
	 * \brief If true, surfaces of elements are filled; if false, only the mesh is displayed.
	 */
	bool textureMode;

	/*!
	 * \brief Font used to render labels.
	 */
	FTGLPixmapFont font;

	// radius of the orbit, should be at least s*sqrt(2)
	const GLdouble r;
	// total area available for the mesh is 's' in each direction of x and y coords
	const GLdouble s;
	// size of the edit-mode sphere
	GLdouble e;
	// arrays storing raw vertices' locations
	size_t ptsCount;
	GLfloat* xa;
	GLfloat* ya;
	GLfloat* za;
	bool* ba;
	// arrays storing raw elements' centers
	size_t elemsCount;
	GLfloat* ecxa;
	GLfloat* ecya;
	GLfloat* ecza;
	// domain overall dimensions
	GLfloat dDepth;
	// domain start/end locations
	GLfloat dsx;
	GLfloat dsy;
	GLfloat dsz;
	std::string domainStartLabel;
	char* dslabel;
	GLfloat dex;
	GLfloat dey;
	GLfloat dez;
	std::string domainEndLabel;
	char* delabel;
	// various multipliers for converting not-raw into raw data
	float sizeMultiplier;
	float depthMultiplier;
	float widthOffset;
	float heightOffset;
	float depthOffset;

	/*!
	 * \brief Keeps the tresholds at which the height-based color changes.
	 */
	std::map<size_t,double> color_tresholds;

	/*!
	 * \brief Precalculated adjecency matrix for all points in the mesh. If given value conn[i][j]
	 *  is true, then the edge should be drawn from point at index i to point at index j.
	 */
	bool** conn;

public:
	MeshView(Mesh* mesh);
	MeshView(Mesh* mesh, Plane viewPlane);
	virtual ~MeshView();

public slots:
	void UpdateData() { recalculate(); }
	void ReloadData() { reinitialize(); }

private:
	void initialize();
	void release();
	void reinitialize();
	void recalculate();

public:
	inline const Mesh* GetMesh() const { return mesh; }
	void SetMesh(Mesh* mesh);
	inline void SetViewPlane(Plane viewPlane) { this->viewPlane = viewPlane; }
	void ChangeViewPlane(AngleType xAngleChange, AngleType yAngleChange, AngleType zAngleChange);
	inline Plane GetViewPlane() const { return viewPlane; }
	inline GLuint GetView() const { return view; }
	inline void SetEditMode(bool mode) { editMode = mode; flatMode = mode; recalculate(); }
	inline void SetFlatMode(bool mode) { flatMode = mode; recalculate(); }
	inline void SetHeightMode(bool mode) { heightMode = mode; }
	inline void SetGuidedMode(bool mode) { guidedMode = mode; }
	inline void SetLabeledMode(bool mode) { labeledMode = mode; }
	inline void SetTextureMode(bool mode) { textureMode = mode; }
	inline bool GetEditMode() { return editMode; }
	inline bool GetFlatMode() { return flatMode; }
	inline bool GetHeightMode() { return heightMode; }
	int GetIndexAt(GLfloat x, GLfloat y);

private:
	inline void setColourOrbit() { glColor3f(1.0f, 1.0f, 1.0f); }
	inline void setColourLabel() { glColor3f(0.7f, 0.7f, 0.9f); }
	void setColour(bool boundary);
	void setColour(double value);

public:
	void DrawBegin();
	virtual void Draw();
	void DrawEnd();

};
