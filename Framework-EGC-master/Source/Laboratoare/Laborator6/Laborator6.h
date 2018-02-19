#pragma once
#include <Component/SimpleScene.h>
#include <Core/GPU/Mesh.h>
#include "LabCamera6.h"
#include <vector>
#include <iostream>
class Laborator6 : public SimpleScene
{
	public:
		Laborator6();
		~Laborator6();

		void Init() override;

		Mesh * CreateMesh(const char * name, const std::vector<VertexFormat> &vertices, const std::vector<unsigned short> &indices);

	private:
		void FrameStart() override;
		void Update(float deltaTimeSeconds) override;
		void FrameEnd() override;

		void RenderSimpleMesh(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix);

		void OnInputUpdate(float deltaTime, int mods) override;
		void OnKeyPress(int key, int mods) override;
		void OnKeyRelease(int key, int mods) override;
		void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
		void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
		void OnWindowResize(int width, int height) override;
		void RenderMesh(Mesh * mesh, Shader * shader, const glm::mat4 & modelMatrix, glm::vec3 color,  float time, glm::vec3 futureColor);
		Mesh* CreateParallelepiped(std::string name, glm::vec3 leftBottomCorner, float depth, float length, float width, glm::vec3 color);
		Mesh* CreateRectangle(std::string name, glm::vec3 leftBottomCorner, float length, float width, glm::vec3 color);
		Mesh* CreateRoad(std::string name, glm::vec3 leftBottomCorner, float length, float width, glm::vec3 color);
		bool Collision(double xmin, double xmax, double ymin, double ymax, double xl, double xr, double yl, double yr);
		bool testCollision(glm::vec3 car1, glm::vec3 car2, glm::vec3 car3, glm::vec3 car4, glm::vec3 par1, glm::vec3 par2,
			glm::vec3 par3, glm::vec3 par4);
protected:
	Laborator::Camera6 *camera;
	Laborator::Camera6 *cameraFP;
	Laborator::Camera6 *cameraTP;
	GLenum polygonMode;
	glm::mat4 projectionMatrix;
	bool renderCameraTarget;
	float width, length, depth;
	float earthLenght, earthWidth;
	float time;
	int nrCol;
	float Score;
	float tx, ty, tz;
	float angularStep;
	float velocity, maxSpeed;
	float carRotation;
};
