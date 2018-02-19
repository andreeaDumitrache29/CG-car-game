#include "Laborator5.h"

#include <vector>
#include <string>
#include <iostream>

#include <Core/Engine.h>

using namespace std;

Laborator5::Laborator5()
{
}

Laborator5::~Laborator5()
{
}

float neg = 45;
float latime = -10.0;
float lungime = -10.0;
void Laborator5::Init()
{
	polygonMode = GL_FILL;
	renderCameraTarget = false;

	camera = new Laborator::Camera();
	camera->Set(glm::vec3(0, 2, 3.5f), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));

	{
		Mesh* mesh = new Mesh("box");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "box.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Mesh* mesh = new Mesh("sphere");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "sphere.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Mesh* mesh = new Mesh("cylinder");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "cylinder.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Shader *shader = new Shader("ShaderLab5");
		shader->AddShader("Source/Laboratoare/Laborator5/Shaders/VertexShader.glsl", GL_VERTEX_SHADER);
		shader->AddShader("Source/Laboratoare/Laborator5/Shaders/FragmentShader.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	tx = 0.0f;
	ty = 0.0f;
	tz = 0.0f;
	angularStep = 0.0f;
	carRotation = 0.0f;
	velocity = 0;

	width = 1.0f;
	length = 0.5f;
	depth = 0.5f;

	Mesh* par = CreateParallelepiped("par", glm::vec3(-0.5f, -0.25f, 0.25f),  depth,  length,  width, glm::vec3(1,0,0));
	meshes[par->GetMeshID()] = par;

	projectionMatrix = glm::perspective(RADIANS(60), window->props.aspectRatio, 0.01f, 200.0f);
}

void Laborator5::FrameStart()
{
	// clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	// sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);
}

void Laborator5::Update(float deltaTimeSeconds)
{
	glPolygonMode(GL_FRONT_AND_BACK, polygonMode);
	{
		glm::mat4 modelMatrix = glm::mat4(1);
		// Attention! The RenderMesh function overrides the usual RenderMesh that we used until now
		// It uses the viewMatrix from Laborator::Camera instance and the local projectionMatrix
		modelMatrix = glm::translate(modelMatrix, glm::vec3(tx, ty, tz));
		RenderMesh(meshes["par"], shaders["VertexNormal"], modelMatrix);
	}

	{
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(-width/4.0f, -length/2 , - depth/ 2.0f));
		modelMatrix = glm::translate(modelMatrix, glm::vec3(tx, ty, tz));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.25f));
		modelMatrix = glm::rotate(modelMatrix, RADIANS(angularStep), glm::vec3(0, 0, 1));
		RenderMesh(meshes["cylinder"], shaders["VertexNormal"], modelMatrix);
	}

	{
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(width / 4.0f, - length / 2 , -depth / 2.0f));
		modelMatrix = glm::translate(modelMatrix, glm::vec3(tx, ty, tz));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.25f));
		modelMatrix = glm::rotate(modelMatrix, RADIANS(angularStep), glm::vec3(0, 0, 1));
		RenderMesh(meshes["cylinder"], shaders["VertexNormal"], modelMatrix);
	}

	{
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(- width / 4.0f, -length / 2 , depth/2.0f));
		modelMatrix = glm::translate(modelMatrix, glm::vec3(tx, ty, tz));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.25f));
		modelMatrix = glm::rotate(modelMatrix, RADIANS(angularStep), glm::vec3(0, 0, 1));
		RenderMesh(meshes["cylinder"], shaders["VertexNormal"], modelMatrix);
	}

	{
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(width / 4.0f, -length / 2, depth/2.0f));
		modelMatrix = glm::translate(modelMatrix, glm::vec3(tx, ty, tz));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.25f));
		modelMatrix = glm::rotate(modelMatrix, RADIANS(angularStep), glm::vec3(0, 0, 1));
		RenderMesh(meshes["cylinder"], shaders["VertexNormal"], modelMatrix);
	}


	// Render the camera target. Useful for understanding where is the rotation point in Third-person camera movement
	if (renderCameraTarget)
	{
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, camera->GetTargetPosition());
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f));
		RenderMesh(meshes["sphere"], shaders["VertexNormal"], modelMatrix);
	}
}

void Laborator5::FrameEnd()
{
	DrawCoordinatSystem(camera->GetViewMatrix(), projectionMatrix);
}

void Laborator5::RenderMesh(Mesh * mesh, Shader * shader, const glm::mat4 & modelMatrix)
{
	if (!mesh || !shader || !shader->program)
		return;

	// render an object using the specified shader and the specified position
	shader->Use();
	glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
	glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	mesh->Render();
}

// Documentation for the input functions can be found in: "/Source/Core/Window/InputController.h" or
// https://github.com/UPB-Graphics/Framework-EGC/blob/master/Source/Core/Window/InputController.h

void Laborator5::OnInputUpdate(float deltaTime, int mods)
{
	// move the camera only if MOUSE_RIGHT button is pressed
	if (window->KeyHold(GLFW_KEY_SPACE))
	{
		switch (polygonMode)
		{
		case GL_POINT:
			polygonMode = GL_FILL;
			break;
		case GL_LINE:
			polygonMode = GL_POINT;
			break;
		default:
			polygonMode = GL_LINE;
			break;
		}
	}
	if (window->KeyHold(GLFW_KEY_RIGHT)) {
		//tx += deltaTime * velocity;
	}
	if (window->KeyHold(GLFW_KEY_LEFT)) {
		//tx -= deltaTime * velocity;
	}
	if (window->KeyHold(GLFW_KEY_UP)) {
		tx += deltaTime * velocity;
		velocity += 0.5f;
		angularStep += deltaTime * velocity;
	}
	if (window->KeyHold(GLFW_KEY_DOWN)) {
		velocity -= 0.5f;
	if (velocity < 0) {
			velocity = 0;
		}
		angularStep -= deltaTime * velocity;
		tx += deltaTime * velocity;
	}
	if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
	{
		float cameraSpeed = 2.0f;

		if (window->KeyHold(GLFW_KEY_W)) {
			// TODO : translate the camera forward
			camera->TranslateForward(cameraSpeed * deltaTime);
		}

		if (window->KeyHold(GLFW_KEY_A)) {
			// TODO : translate the camera to the left
			camera->TranslateRight(-cameraSpeed * deltaTime);
		}

		if (window->KeyHold(GLFW_KEY_S)) {
			// TODO : translate the camera backwards
			camera->TranslateForward(-cameraSpeed * deltaTime);
		}

		if (window->KeyHold(GLFW_KEY_D)) {
			// TODO : translate the camera to the right
			camera->TranslateRight(cameraSpeed * deltaTime);
		}

		if (window->KeyHold(GLFW_KEY_Q)) {
			// TODO : translate the camera down
			camera->TranslateUpword(-cameraSpeed * deltaTime);
		}

		if (window->KeyHold(GLFW_KEY_E)) {
			// TODO : translate the camera up
			camera->TranslateUpword(cameraSpeed * deltaTime);
		}
	}

}

void Laborator5::OnKeyPress(int key, int mods)
{
	// add key press event
	if (key == GLFW_KEY_T)
	{
		renderCameraTarget = !renderCameraTarget;
	}
}

void Laborator5::OnKeyRelease(int key, int mods)
{
	// add key release event
}

void Laborator5::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	// add mouse move event

	if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
	{
		float sensivityOX = 0.001f;
		float sensivityOY = 0.001f;

		if (window->GetSpecialKeyState() == 0) {
			renderCameraTarget = false;
			// TODO : rotate the camera in First-person mode around OX and OY using deltaX and deltaY
			// use the sensitivity variables for setting up the rotation speed
			camera->RotateFirstPerson_OX(deltaY * sensivityOX);
			camera->RotateFirstPerson_OY(deltaX * sensivityOY);
		}

		if (window->GetSpecialKeyState() && GLFW_MOD_CONTROL) {
			renderCameraTarget = true;
			// TODO : rotate the camera in Third-person mode around OX and OY using deltaX and deltaY
			// use the sensitivity variables for setting up the rotation speed
			camera->RotateThirdPerson_OX(deltaY * sensivityOX);
			camera->RotateThirdPerson_OY(deltaX * sensivityOY);
		}

	}
}

void Laborator5::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button press event
}

void Laborator5::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button release event
}

void Laborator5::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}

void Laborator5::OnWindowResize(int width, int height)
{
}

Mesh* Laborator5::CreateParallelepiped(std::string name, glm::vec3 leftBottomCorner, float depth, float length, float width, glm::vec3 color)
{
	glm::vec3 corner = leftBottomCorner;

	std::vector<VertexFormat> vertices =
	{
		VertexFormat(corner, color),
		VertexFormat(corner + glm::vec3(width, 0, 0), color),
		VertexFormat(corner + glm::vec3(0, length, 0), color),
		VertexFormat(corner + glm::vec3(width, length, 0), color),
		VertexFormat(corner + glm::vec3(0, 0, -depth), color),
		VertexFormat(corner + glm::vec3(width, 0, -depth), color),
		VertexFormat(corner + glm::vec3(0,length, -depth), color),
		VertexFormat(corner + glm::vec3(width, length, -depth), color),
	};

	Mesh* par = new Mesh(name);
	std::vector<unsigned short> indices =
	{
		0, 1, 2,		1, 3, 2,
		2, 3, 7,		2, 7, 6,
		1, 7, 3,		1, 5, 7,
		6, 7, 4,		7, 5, 4,
		0, 4, 1,		1, 4, 5,
		2, 6, 4,		0, 2, 4,
	};

	par->InitFromData(vertices, indices);
	return par;
}
