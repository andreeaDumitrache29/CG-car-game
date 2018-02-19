#include "Laborator6.h"

#include <vector>
#include <string>
#include <iostream>

#include <Core/Engine.h>

using namespace std;

vector <glm::vec4> coord;
vector <glm::vec4> coord_transform;
vector <glm::vec4> coord_par;
vector <glm::vec4> coord_par_transform;
bool backwards = false;
float minX, maxX, minZ, maxZ;
float minXP, maxXP, minZP, maxZP;
bool finish = false;
bool col[15];

Laborator6::Laborator6()
{
}

Laborator6::~Laborator6()
{
}

bool Laborator6::testCollision(glm::vec3 car1, glm::vec3 car2, glm::vec3 car3, glm::vec3 car4,glm::vec3 par1,glm::vec3 par2,
	glm::vec3 par3, glm::vec3 par4) {
	//punctul maxim si minim al bazei masinii
	float minX = min(min(min(car1.x, car2.x), car3.x), car4.x);
	float maxX = max(max(max(car1.x, car2.x), car3.x), car4.x);
	float minZ = min(min(min(car1.z, car2.z), car3.z), car4.z);
	float maxZ = max(max(max(car1.z, car2.z), car3.z), car4.z);

	//punctul minim si maxim al bordurii
	float minXP = min(min(min(par1.x, par2.x), par3.x), par4.x);
	float maxXP = max(max(max(par1.x, par2.x), par3.x), par4.x);
	float minZP = min(min(min(par1.z, par2.z), par3.z), par4.z);
	float maxZP = max(max(max(par1.z, par2.z), par3.z), par4.z);

	//verifica coliziunea
	if (Collision(minX, maxX, minZ, maxZ, minXP, maxXP, minZP, maxZP)) {
		return true;
	}
	return false;
}

void Laborator6::Init()
{
	polygonMode = GL_FILL;
	renderCameraTarget = false;

	// folosesc o instanta de camera pentru camer first person si o instanta pentru
	// camera third person si fac schimb intre ele in functie de apasarea tastei 'T'
	camera = new Laborator::Camera6();
	cameraTP = new Laborator::Camera6();
	cameraTP->Set(glm::vec3(0.0f, 1.0f, -2.0f), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	cameraFP = new Laborator::Camera6();
	cameraFP->Set(glm::vec3(0.0f, 1.0f, 0.5f), glm::vec3(0, 1, 1), glm::vec3(0, 1, 0));

	camera = cameraFP;

	//creare mesa sfera
	{
		Mesh* mesh = new Mesh("sphere");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "sphere.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	//creare mesa cilindru
	{
		Mesh* mesh = new Mesh("cylinder");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "cylinder.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	//instantiere shader
	{
		Shader *shader = new Shader("ShaderLab6");
		shader->AddShader("Source/Laboratoare/Laborator6/Shaders/VertexShader.glsl", GL_VERTEX_SHADER);
		shader->AddShader("Source/Laboratoare/Laborator6/Shaders/FragmentShader.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	//initializari variabile
	Score = 0;
	tx = 0.0f;
	ty = 0.0f;
	tz = 0.0f; 
	//rotatie roti in jurul centrului
	angularStep = 0.0f;
	//rotatie roti supa directia de deplasare
	carRotation = 0.0f;
	velocity = 0;
	width = 1.0f;
	length = 0.5f;
	depth = 0.5f;
	earthLenght = 1;
	earthWidth = 1;
	maxSpeed = 8;
	nrCol = 0;
	//initializari vector coliziuni
	for (int i = 0; i < 15; i++) {
		col[i] = false;
	}

	// creare paralelipiped pornind de la coltul stanga jos
	Mesh* par = CreateParallelepiped("par", glm::vec3(-0.5f, -0.25f, 0.25f), depth, length, width, glm::vec3(1, 0, 0));
	meshes[par->GetMeshID()] = par;

	// creare masina pornind de la coltul stanga jos
	Mesh* car = CreateParallelepiped("car", glm::vec3(-0.25f, -0.25f/2.0f, 0.25f/2.0f), depth/2.0f, length / 2.0f, width / 2.0f, glm::vec3(1, 0, 0));
	meshes[car->GetMeshID()] = car;

	//vectori cu coordonatele punctelor dreptunghiului de la baza masinii initiale si dupa
	// aplicarea transformarilor
	glm::vec3 corner = glm::vec3(-0.25f, -0.25f / 2.0f, 0.25f / 2.0f);
	coord_transform.push_back(glm::vec4(corner, 1));
	coord_transform.push_back(glm::vec4((corner + glm::vec3(width / 2.0f, 0.0f, 0.0f)), 1));
	coord_transform.push_back(glm::vec4((corner + glm::vec3(width / 2.0f, 0.0f, -depth / 2.0f)), 1));
	coord_transform.push_back(glm::vec4((corner + glm::vec3(0.0f, 0.0f, -depth / 2.0f)), 1));

	coord.push_back(glm::vec4(corner, 1));
	coord.push_back(glm::vec4((corner + glm::vec3(width / 2.0f, 0.0f, 0.0f)), 1));
	coord.push_back(glm::vec4((corner + glm::vec3(width / 2.0f, 0.0f, -depth / 2.0f)), 1));
	coord.push_back(glm::vec4((corner + glm::vec3(0.0f, 0.0f, -depth / 2.0f)), 1));
	
	//vectori cu coordonatele punctelor dreptunghiului de la baza unui paralelipiped 
	//initiale si dupa aplicarea transformarilor
	corner = glm::vec3(-0.5f, -0.25f, 0.25f);
	coord_par.push_back(glm::vec4(corner, 1));
	coord_par.push_back(glm::vec4((corner + glm::vec3(width, 0, 0.0f)), 1));
	coord_par.push_back(glm::vec4((corner + glm::vec3(width, 0, -depth)), 1));
	coord_par.push_back(glm::vec4((corner + glm::vec3(0.0f, 0, -depth)), 1));

	corner = glm::vec3(-0.5f, -0.25f, 0.25f);
	coord_par_transform.push_back(glm::vec4(corner, 1));
	coord_par_transform.push_back(glm::vec4((corner + glm::vec3(width, 0, 0.0f)), 1));
	coord_par_transform.push_back(glm::vec4((corner + glm::vec3(width, 0, -depth)), 1));
	coord_par_transform.push_back(glm::vec4((corner + glm::vec3(0.0f, 0, -depth)), 1));

	//creare dreptunghi
	Mesh* rectangle = CreateRectangle("rectangle", glm::vec3 (-earthLenght/2, 0, earthWidth / 2), earthLenght, earthWidth, glm::vec3(0.6f, 1, 0.8f));
	meshes[rectangle->GetMeshID()] = rectangle;

	//linia de finish
	Mesh* finish = CreateRectangle("finish", glm::vec3(-3,0,93), 6, 3, glm::vec3(1,1,1));
	meshes[finish->GetMeshID()] = finish;

	//drumul reprezentat de un dreptunghi de lungime 30 si latime 6
	Mesh* road30 = CreateRoad("road30", glm::vec3(-3, 0, 0), 6, 30, glm::vec3(0.6f, 1, 0.8f));
	meshes[road30->GetMeshID()] = road30;

	//matricea pentru proiectia perspectiva
	projectionMatrix = glm::perspective(RADIANS(60), window->props.aspectRatio, 0.01f, 200.0f);
}


Mesh* Laborator6::CreateMesh(const char *name, const std::vector<VertexFormat> &vertices, const std::vector<unsigned short> &indices)
{
	unsigned int VAO = 0;
	// TODO: Create the VAO and bind it
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// TODO: Create the VBO and bind it
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// TODO: Send vertices data into the VBO buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	// TODO: Crete the IBO and bind it
	unsigned int IBO;
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	// TODO: Send indices data into the IBO buffer
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);

	// ========================================================================
	// This section describes how the GPU Shader Vertex Shader program receives data

	// set vertex position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), 0);

	// set vertex normal attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(sizeof(glm::vec3)));

	// set texture coordinate attribute
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3)));

	// set vertex color attribute
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3) + sizeof(glm::vec2)));
	// ========================================================================

	//glEnableVertexAttribArray(4);
	//glVertexAttribPointer(4, 3, GL_DOUBLE, GL_FALSE, sizeof(GL_DOUBLE), (void*)(3 * sizeof(glm::vec3) + sizeof(glm::vec2)));

	// Unbind the VAO
	glBindVertexArray(0);

	// Check for OpenGL errors
	CheckOpenGLError();

	// Mesh information is saved into a Mesh object
	meshes[name] = new Mesh(name);
	meshes[name]->InitFromBuffer(VAO, static_cast<unsigned short>(indices.size()));
	meshes[name]->vertices = vertices;
	meshes[name]->indices = indices;
	return meshes[name];
}


void Laborator6::FrameStart()
{
	// clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	// sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);
}

void Laborator6::Update(float deltaTimeSeconds)
{
	glPolygonMode(GL_FRONT_AND_BACK, polygonMode);
	time = Engine::GetElapsedTime();
	//culori cer
	glm::vec3 morning = glm::vec3(0.5f, 0.8745f, 1);
	glm::vec3 afternoon = glm::vec3(0.2f, 0.8f, 1);
	glm::vec3 sunset = glm::vec3(0.98f, 0.349f, 0.192f);
	glm::vec3 night = glm::vec3(0.074f, 0.149f, 0.223f);

	//culori iarba
	glm::vec3 morningG = glm::vec3(0.6f, 1, 0.8f);
	glm::vec3 afternoonG = glm::vec3(0, 0.8f, 0.4f);
	glm::vec3 sunsetG = glm::vec3(0.17f, 0.525f, 0.176f);
	glm::vec3 nightG = glm::vec3(0, 0.2f, 0);

	//culori drum
	glm::vec3 morningR = glm::vec3(0.67f, 0.72f, 0.71f);
	glm::vec3 afternoonR = glm::vec3(0.64f, 0.66f, 0.65f);
	glm::vec3 sunsetR = glm::vec3(0.42f, 0.435f, 0.435f);
	glm::vec3 nightR = glm::vec3(0.21f, 0.2f, 0.2f);

	//verifica daca jucatorul a pierdut (a ajuns la a patra coliziune)
	if (!finish && nrCol > 3) {
		std::cout << "GAME OVER! Score: "<< Score << "\n";
		finish = true;
	}
	
	// creare masina folosind un paralelipiped si 4 cilindrii
	{
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(tx, ty, tz));
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0.15, 0));
		modelMatrix = glm::rotate(modelMatrix, carRotation, glm::vec3(0, 1, 0));
		modelMatrix = glm::rotate(modelMatrix, RADIANS(90), glm::vec3(0, 1, 0));

		//calculam separat de desenare coordonatele noi ale masinii pentru a putea detecta coliziunile
		//si din modul first person
		for (int i = 0; i < coord.size(); i++) {
			coord_transform.at(i) = modelMatrix *  coord.at(i);
		}

		//camera third person este activata, asa ca desenam masina
		if (renderCameraTarget)
		{
			{
				RenderMesh(meshes["car"], shaders["ShaderLab6"], modelMatrix, glm::vec3(0.89, 0.85, 0), time, glm::vec3(0.89, 0.85, 0));
			}

			//creare si desenare roti
			{
				glm::mat4 modelMatrix = glm::mat4(1);
				modelMatrix = glm::translate(modelMatrix, glm::vec3(tx, ty, tz));
				modelMatrix = glm::rotate(modelMatrix, carRotation, glm::vec3(0, 1, 0));
				modelMatrix = glm::translate(modelMatrix, glm::vec3(-width / 8.0f, -length / 4, depth / 4.0f));
				modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0.15, 0));
				modelMatrix = glm::scale(modelMatrix, glm::vec3(0.05f));
				modelMatrix = glm::rotate(modelMatrix, angularStep, glm::vec3(0, 0, 1));
				RenderMesh(meshes["cylinder"], shaders["ShaderLab6"], modelMatrix, glm::vec3(1, 0, 1), time, glm::vec3(1, 0, 1));
			}

			{
				glm::mat4 modelMatrix = glm::mat4(1);
				modelMatrix = glm::translate(modelMatrix, glm::vec3(tx, ty, tz));
				modelMatrix = glm::rotate(modelMatrix, carRotation, glm::vec3(0, 1, 0));
				modelMatrix = glm::translate(modelMatrix, glm::vec3(width / 8.0f, -length / 4, depth / 4.0f));
				modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0.15, 0));
				modelMatrix = glm::scale(modelMatrix, glm::vec3(0.05f));
				modelMatrix = glm::rotate(modelMatrix, angularStep, glm::vec3(0, 0, 1));
				RenderMesh(meshes["cylinder"], shaders["ShaderLab6"], modelMatrix, glm::vec3(1, 0, 1), time, glm::vec3(1, 0, 1));
			}

			{
				glm::mat4 modelMatrix = glm::mat4(1);
				modelMatrix = glm::translate(modelMatrix, glm::vec3(tx, ty, tz));
				modelMatrix = glm::rotate(modelMatrix, carRotation, glm::vec3(0, 1, 0));
				modelMatrix = glm::translate(modelMatrix, glm::vec3(-width / 8.0f, -length / 4, -depth / 4.0f));
				modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0.15, 0));
				modelMatrix = glm::scale(modelMatrix, glm::vec3(0.05f));
				modelMatrix = glm::rotate(modelMatrix, angularStep, glm::vec3(0, 0, 1));
				RenderMesh(meshes["cylinder"], shaders["ShaderLab6"], modelMatrix, glm::vec3(1, 0, 1), time, glm::vec3(1, 0, 1));
			}

			{
				glm::mat4 modelMatrix = glm::mat4(1);
				modelMatrix = glm::translate(modelMatrix, glm::vec3(tx, ty, tz));
				modelMatrix = glm::rotate(modelMatrix, carRotation, glm::vec3(0, 1, 0));
				modelMatrix = glm::translate(modelMatrix, glm::vec3(width / 8.0f, -length / 4, -depth / 4.0f));
				modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0.15, 0));
				modelMatrix = glm::scale(modelMatrix, glm::vec3(0.05f));
				modelMatrix = glm::rotate(modelMatrix, angularStep, glm::vec3(0, 0, 1));
				RenderMesh(meshes["cylinder"], shaders["ShaderLab6"], modelMatrix, glm::vec3(1, 0, 1), time, glm::vec3(1, 0, 1));
			}
		}
	}


	//cerul
	{
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(tx, ty, tz));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(400));
		if (sin(time / 5.0f) >= 0 && cos(time / 5.0f) >= 0) {
			RenderMesh(meshes["sphere"], shaders["ShaderLab6"], modelMatrix, morning, abs(sin(time/5.0f)), afternoon);
		}else if (sin(time / 5.0f) >= 0 && cos(time / 5.0f) < 0) {
			RenderMesh(meshes["sphere"], shaders["ShaderLab6"], modelMatrix, afternoon, abs(cos(time / 5.0f)), sunset);
		}if (sin(time / 5.0f) < 0 && cos(time / 5.0f) < 0) {
			RenderMesh(meshes["sphere"], shaders["ShaderLab6"], modelMatrix, sunset, abs(sin(time / 5.0f)), night);
		}if (sin(time / 5.0f) < 0 && cos(time / 5.0f) >= 0) {
			RenderMesh(meshes["sphere"], shaders["ShaderLab6"], modelMatrix, night, abs(cos(time / 5.0f)), morning);
		}
		
	}

	//pamant
	{
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(tx, -1, tz));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(300));
		if (sin(time / 5.0f) >= 0 && cos(time / 5.0f) >= 0) {
			RenderMesh(meshes["rectangle"], shaders["ShaderLab6"], modelMatrix, morningG, abs(sin(time / 5.0f)), afternoonG);
		}
		else if (sin(time / 5.0f) >= 0 && cos(time / 5.0f) < 0) {
			RenderMesh(meshes["rectangle"], shaders["ShaderLab6"], modelMatrix, afternoonG, abs(cos(time / 5.0f)), sunsetG);
		}if (sin(time / 5.0f) < 0 && cos(time / 5.0f) < 0) {
			RenderMesh(meshes["rectangle"], shaders["ShaderLab6"], modelMatrix, sunsetG, abs(sin(time / 5.0f)), nightG);
		}if (sin(time / 5.0f) < 0 && cos(time / 5.0f) >= 0) {
			RenderMesh(meshes["rectangle"], shaders["ShaderLab6"], modelMatrix, nightG, abs(cos(time / 5.0f)), morningG);
		}
	}

	//prima bucata drum
	{
		glm::mat4 modelMatrix = glm::mat4(1);
		if (sin(time / 5.0f) >= 0 && cos(time / 5.0f) >= 0) {
			RenderMesh(meshes["road30"], shaders["ShaderLab6"], modelMatrix, morningR, abs(sin(time / 5.0f)), afternoonR);
		}
		else if (sin(time / 5.0f) >= 0 && cos(time / 5.0f) < 0) {
			RenderMesh(meshes["road30"], shaders["ShaderLab6"], modelMatrix, afternoonR, abs(cos(time / 5.0f)), sunsetR);
		}if (sin(time / 5.0f) < 0 && cos(time / 5.0f) < 0) {
			RenderMesh(meshes["road30"], shaders["ShaderLab6"], modelMatrix, sunsetR, abs(sin(time / 5.0f)), nightR);
		}if (sin(time / 5.0f) < 0 && cos(time / 5.0f) >= 0) {
			RenderMesh(meshes["road30"], shaders["ShaderLab6"], modelMatrix, nightR, abs(cos(time / 5.0f)), morningR);
		}
	}

	glm::vec3 colorP = glm::vec3(1, 0, 0);
	glm::vec3 colorI = glm::vec3(0, 0, 0);
	//bordurile stanga
	for (float i = 0.0f; i < 36.0f; i+=1.0f) {
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(-2.75f, 0.25f, 1.0f + i));
		modelMatrix = glm::rotate(modelMatrix, RADIANS(90), glm::vec3(0, 1, 0));
		if ((int)i % 2 == 0)
			RenderMesh(meshes["par"], shaders["ShaderLab6"], modelMatrix, colorP, time, colorP);
		else
			RenderMesh(meshes["par"], shaders["ShaderLab6"], modelMatrix, colorI, time, colorI);
		//verificam coliziunea cu bordurile daca masina este in aceasta zona
		if (tz <= 40) {
			for (int j = 0; j < coord_par.size(); j++) {
				coord_par_transform.at(j) = modelMatrix * coord_par.at(j);
			}
			if (testCollision(coord_transform.at(0), coord_transform.at(1), coord_transform.at(2), coord_transform.at(3), coord_par_transform.at(0), 
				coord_par_transform.at(1), coord_par_transform.at(2), coord_par_transform.at(3))) {
				velocity = 0;
				if (!col[0]) {
					nrCol++;
					Score -= Score / 3.0f;
				}
				col[0] = true;
			}
		}
		if (col[0] && tx > -2.0f) {
			col[0] = false;
		}
	}

	//borduri dreapta
	for (float i = 0.0f; i < 30.0f; i++) {
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(2.75f, 0.25f, 1.0f + i));
		modelMatrix = glm::rotate(modelMatrix, RADIANS(90), glm::vec3(0, 1, 0));
		if ((int)i % 2 == 0)
			RenderMesh(meshes["par"], shaders["ShaderLab6"], modelMatrix, colorP, time, colorP);
		else
			RenderMesh(meshes["par"], shaders["ShaderLab6"], modelMatrix, colorI, time, colorI);
		//verificam coliziunea cu bordurile daca masina este in aceasta zona
		if (tz <= 30) {
			for (int i = 0; i < coord_par.size(); i++) {
				coord_par_transform.at(i) = modelMatrix * coord_par.at(i);
			}
			if (testCollision(coord_transform.at(0), coord_transform.at(1), coord_transform.at(2), coord_transform.at(3), coord_par_transform.at(0),
				coord_par_transform.at(1), coord_par_transform.at(2), coord_par_transform.at(3))) {
				velocity = 0;
				if (!col[1]) {
					nrCol++;
					Score -= Score / 3.0f;
				}
				col[1] = true;
			}
		}
		if (col[1] && tx < 2.0f) {
			col[1] = false;
		}
	}

	//a doua bucata de drum
	{
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(-3, 0, 33));
		modelMatrix = glm::rotate(modelMatrix, RADIANS(90), glm::vec3(0, 1, 0));
		if (sin(time / 5.0f) >= 0 && cos(time / 5.0f) >= 0) {
			RenderMesh(meshes["road30"], shaders["ShaderLab6"], modelMatrix, morningR, abs(sin(time / 5.0f)), afternoonR);
		}
		else if (sin(time / 5.0f) >= 0 && cos(time / 5.0f) < 0) {
			RenderMesh(meshes["road30"], shaders["ShaderLab6"], modelMatrix, afternoonR, abs(cos(time / 5.0f)), sunsetR);
		}if (sin(time / 5.0f) < 0 && cos(time / 5.0f) < 0) {
			RenderMesh(meshes["road30"], shaders["ShaderLab6"], modelMatrix, sunsetR, abs(sin(time / 5.0f)), nightR);
		}if (sin(time / 5.0f) < 0 && cos(time / 5.0f) >= 0) {
			RenderMesh(meshes["road30"], shaders["ShaderLab6"], modelMatrix, nightR, abs(cos(time / 5.0f)), morningR);
		}
	}

	//borduri dreapta
	for (int i = 0; i < 30; i++) {
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(3.5f + i, 0.25f, 30));
		if (i % 2 == 0)
			RenderMesh(meshes["par"], shaders["ShaderLab6"], modelMatrix, colorP, time, colorP);
		else
			RenderMesh(meshes["par"], shaders["ShaderLab6"], modelMatrix, colorI, time, colorI);
		//verificam coliziunea cu bordurile daca masina este in aceasta zona
		if (tz >= 30 && tz <= 36) {
			for (int i = 0; i < coord_par.size(); i++) {
				coord_par_transform.at(i) = modelMatrix * coord_par.at(i);
			}
			if (testCollision(coord_transform.at(0), coord_transform.at(1), coord_transform.at(2), coord_transform.at(3), coord_par_transform.at(0),
				coord_par_transform.at(1), coord_par_transform.at(2), coord_par_transform.at(3))) {
				velocity = 0;
				if (!col[2]) {
					nrCol++;
					Score -= Score / 3;
				}
				col[2] = true;
			}
		}
		if (col[2] && tz > 30.75f) {
			col[2] = false;
		}
	}
	//borduri stanga
	for (int i = 0; i < 30; i++) {
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(-2.5 + i, 0.25, 36));
		if (i % 2 == 0)
			RenderMesh(meshes["par"], shaders["ShaderLab6"], modelMatrix, colorP, time, colorP);
		else
			RenderMesh(meshes["par"], shaders["ShaderLab6"], modelMatrix, colorI, time, colorI);
		//verificam coliziunea cu bordurile daca masina este in aceasta zona
		if (tz >= 30 && tz <= 36) {
			for (int i = 0; i < coord_par.size(); i++) {
				coord_par_transform.at(i) = modelMatrix * coord_par.at(i);
			}
			if (testCollision(coord_transform.at(0), coord_transform.at(1), coord_transform.at(2), coord_transform.at(3), coord_par_transform.at(0),
				coord_par_transform.at(1), coord_par_transform.at(2), coord_par_transform.at(3))) {
				velocity = 0;
				if (!col[3]) {
					nrCol++;
					Score -= Score / 3;
				}
				col[3] = true;
			}
		}
		if (col[3] && tz < 35.25f) {
			col[3] = false;
		}
	}

	//a treia bucata de drum
	
	{
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(30, 0, 30));
		if (sin(time / 5.0f) >= 0 && cos(time / 5.0f) >= 0) {
			RenderMesh(meshes["road30"], shaders["ShaderLab6"], modelMatrix, morningR, abs(sin(time / 5.0f)), afternoonR);
		}
		else if (sin(time / 5.0f) >= 0 && cos(time / 5.0f) < 0) {
			RenderMesh(meshes["road30"], shaders["ShaderLab6"], modelMatrix, afternoonR, abs(cos(time / 5.0f)), sunsetR);
		}if (sin(time / 5.0f) < 0 && cos(time / 5.0f) < 0) {
			RenderMesh(meshes["road30"], shaders["ShaderLab6"], modelMatrix, sunsetR, abs(sin(time / 5.0f)), nightR);
		}if (sin(time / 5.0f) < 0 && cos(time / 5.0f) >= 0) {
			RenderMesh(meshes["road30"], shaders["ShaderLab6"], modelMatrix, nightR, abs(cos(time / 5.0f)), morningR);
		}
	}

	//borduri dreapta
	for (int i = 0; i < 36; i++) {
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(33, 0.25, 30.5 + i));
		modelMatrix = glm::rotate(modelMatrix, RADIANS(90), glm::vec3(0, 1, 0));
		if (i % 2 == 0)
			RenderMesh(meshes["par"], shaders["ShaderLab6"], modelMatrix, colorP, time, colorP);
		else
			RenderMesh(meshes["par"], shaders["ShaderLab6"], modelMatrix, colorI, time, colorI);
		//verificam coliziunea cu bordurile daca masina este in aceasta zona
		if (tz >= 36 && tz <= 66) {
			for (int i = 0; i < coord_par.size(); i++) {
				coord_par_transform.at(i) = modelMatrix * coord_par.at(i);
			}
			if (testCollision(coord_transform.at(0), coord_transform.at(1), coord_transform.at(2), coord_transform.at(3), coord_par_transform.at(0),
				coord_par_transform.at(1), coord_par_transform.at(2), coord_par_transform.at(3))) {
				velocity = 0;
				if (!col[4]) {
					nrCol++;
					Score -= Score / 3;
				}
				col[4] = true;
			}
		}
		if (col[4] && tx < 32.25f) {
			col[4] = false;
		}
	}

	//borduri stanga
	for (int i = 0; i < 24; i++) {
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(27, 0.25, 36.5 + i));
		modelMatrix = glm::rotate(modelMatrix, RADIANS(90), glm::vec3(0, 1, 0));
		if (i % 2 == 0)
			RenderMesh(meshes["par"], shaders["ShaderLab6"], modelMatrix, colorP, time, colorP);
		else
			RenderMesh(meshes["par"], shaders["ShaderLab6"], modelMatrix, colorI, time, colorI);
		//verificam coliziunea cu bordurile daca masina este in aceasta zona
		if (tz >= 36 && tz <= 66) {
			for (int i = 0; i < coord_par.size(); i++) {
				coord_par_transform.at(i) = modelMatrix * coord_par.at(i);
			}
			if (testCollision(coord_transform.at(0), coord_transform.at(1), coord_transform.at(2), coord_transform.at(3), coord_par_transform.at(0),
				coord_par_transform.at(1), coord_par_transform.at(2), coord_par_transform.at(3))) {
				velocity = 0;
				if (!col[5]) {
					nrCol++;
					Score -= Score / 3;
				}
				col[5] = true;
			}
		}
		if (col[5] && tx > 27.75f) {
			col[5] = false;
		}
	}

	//a patra bucata de drum
	{
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(33, 0, 63));
		modelMatrix = glm::rotate(modelMatrix, RADIANS(270), glm::vec3(0, 1, 0));
		if (sin(time / 5.0f) >= 0 && cos(time / 5.0f) >= 0) {
			RenderMesh(meshes["road30"], shaders["ShaderLab6"], modelMatrix, morningR, abs(sin(time / 5.0f)), afternoonR);
		}
		else if (sin(time / 5.0f) >= 0 && cos(time / 5.0f) < 0) {
			RenderMesh(meshes["road30"], shaders["ShaderLab6"], modelMatrix, afternoonR, abs(cos(time / 5.0f)), sunsetR);
		}if (sin(time / 5.0f) < 0 && cos(time / 5.0f) < 0) {
			RenderMesh(meshes["road30"], shaders["ShaderLab6"], modelMatrix, sunsetR, abs(sin(time / 5.0f)), nightR);
		}if (sin(time / 5.0f) < 0 && cos(time / 5.0f) >= 0) {
			RenderMesh(meshes["road30"], shaders["ShaderLab6"], modelMatrix, nightR, abs(cos(time / 5.0f)), morningR);
		}
	}

	//borduri dreapta
	for (int i = 0; i < 30; i++) {
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(32.5 - i, 0.25, 66));
		if (i % 2 == 0)
			RenderMesh(meshes["par"], shaders["ShaderLab6"], modelMatrix, colorP, time, colorP);
		else
			RenderMesh(meshes["par"], shaders["ShaderLab6"], modelMatrix, colorI, time, colorI);
		//verificam coliziunea cu bordurile daca masina este in aceasta zona
		if (tz >= 60 && tz <= 66) {
			for (int i = 0; i < coord_par.size(); i++) {
				coord_par_transform.at(i) = modelMatrix * coord_par.at(i);
			}
			if (testCollision(coord_transform.at(0), coord_transform.at(1), coord_transform.at(2), coord_transform.at(3), coord_par_transform.at(0),
				coord_par_transform.at(1), coord_par_transform.at(2), coord_par_transform.at(3))) {
				velocity = 0;
				if (!col[6]) {
					nrCol++;
					Score -= Score / 3;
				}
				col[6] = true;
			}
		}
		if (col[6] && tz < 65.25f) {
			col[6] = false;
		}
	}

	//borduri stanga
	for (int i = 0; i < 30; i++) {
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(26.5 - i, 0.25, 60));
		if (i % 2 == 0)
			RenderMesh(meshes["par"], shaders["ShaderLab6"], modelMatrix, colorP, time, colorP);
		else
			RenderMesh(meshes["par"], shaders["ShaderLab6"], modelMatrix, colorI, time, colorI);
		//verificam coliziunea cu bordurile daca masina este in aceasta zona
		if (tz >= 60 && tz <= 66) {
			for (int i = 0; i < coord_par.size(); i++) {
				coord_par_transform.at(i) = modelMatrix * coord_par.at(i);
			}
			if (testCollision(coord_transform.at(0), coord_transform.at(1), coord_transform.at(2), coord_transform.at(3), coord_par_transform.at(0),
				coord_par_transform.at(1), coord_par_transform.at(2), coord_par_transform.at(3))) {
				velocity = 0;
				if (!col[7]) {
					nrCol++;
					Score -= Score / 3;
				}
				col[7] = true;
			}
		}
		if (col[7] && tz > 60.75f) {
			col[7] = false;
		}
	}

	//ultima bucata de drum
	{
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0, 60));
		if (sin(time / 5.0f) >= 0 && cos(time / 5.0f) >= 0) {
			RenderMesh(meshes["road30"], shaders["ShaderLab6"], modelMatrix, morningR, abs(sin(time / 5.0f)), afternoonR);
		}
		else if (sin(time / 5.0f) >= 0 && cos(time / 5.0f) < 0) {
			RenderMesh(meshes["road30"], shaders["ShaderLab6"], modelMatrix, afternoonR, abs(cos(time / 5.0f)), sunsetR);
		}if (sin(time / 5.0f) < 0 && cos(time / 5.0f) < 0) {
			RenderMesh(meshes["road30"], shaders["ShaderLab6"], modelMatrix, sunsetR, abs(sin(time / 5.0f)), nightR);
		}if (sin(time / 5.0f) < 0 && cos(time / 5.0f) >= 0) {
			RenderMesh(meshes["road30"], shaders["ShaderLab6"], modelMatrix, nightR, abs(cos(time / 5.0f)), morningR);
		}
	}

	//borduri stanga
	for (int i = 0; i < 30; i++) {
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(-2.75, 0.25, 60.5 + i));
		modelMatrix = glm::rotate(modelMatrix, RADIANS(90), glm::vec3(0, 1, 0));
		if (i % 2 == 0)
			RenderMesh(meshes["par"], shaders["ShaderLab6"], modelMatrix, colorP, time, colorP);
		else
			RenderMesh(meshes["par"], shaders["ShaderLab6"], modelMatrix, colorI, time, colorI);
		//verificam coliziunea cu bordurile daca masina este in aceasta zona
		if (tz >= 60) {
			for (int i = 0; i < coord_par.size(); i++) {
				coord_par_transform.at(i) = modelMatrix * coord_par.at(i);
			}
			if (testCollision(coord_transform.at(0), coord_transform.at(1), coord_transform.at(2), coord_transform.at(3), coord_par_transform.at(0),
				coord_par_transform.at(1), coord_par_transform.at(2), coord_par_transform.at(3))) {
				velocity = 0;
				if (!col[8]) {
					nrCol++;
					Score -= Score / 3;
				}
				col[8] = true;
			}
		}
		if (col[8] && tx > -2.0f) {
			col[8] = false;
		}
	}

	//borduri dreapta
	for (int i = 0; i < 24; i++) {
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(2.75, 0.25, 66.5 + i));
		modelMatrix = glm::rotate(modelMatrix, RADIANS(90), glm::vec3(0, 1, 0));
		if (i % 2 == 0)
			RenderMesh(meshes["par"], shaders["ShaderLab6"], modelMatrix, colorP, time, colorP);
		else
			RenderMesh(meshes["par"], shaders["ShaderLab6"], modelMatrix, colorI, time, colorI);
		//verificam coliziunea cu bordurile daca masina este in aceasta zona
		if (tz >= 60) {
			for (int i = 0; i < coord_par.size(); i++) {
				coord_par_transform.at(i) = modelMatrix * coord_par.at(i);
			}
			if (testCollision(coord_transform.at(0), coord_transform.at(1), coord_transform.at(2), coord_transform.at(3), coord_par_transform.at(0),
				coord_par_transform.at(1), coord_par_transform.at(2), coord_par_transform.at(3))) {
				velocity = 0;
				if (!col[9]) {
					nrCol++;
					Score -= Score / 3;
				}
				col[9] = true;
			}
		}
		if (col[9] && tz < 2.0f) {
			col[9] = false;
		}
	}

	// desenare si verificare coliziune cu linia de finish
	glm::mat4 modelMatrix = glm::mat4(1);
	RenderMesh(meshes["finish"], shaders["ShaderLab6"], modelMatrix, glm::vec3(1, 1, 1), abs(cos(time / 10.0f)), glm::vec3(1, 1, 1));
	if (!finish) {
		//coordonate minime si maxime pentru linia de finish
		glm::vec3 bottomLeft = glm::vec3(-3, 0, 93);
		glm::vec3 topRight = bottomLeft + glm::vec3(6, 0, -3);
		//coordonatele curente ale masinii
		minX = min(min(min(coord_transform.at(0).x, coord_transform.at(1).x), coord_transform.at(2).x), coord_transform.at(3).x);
		maxX = max(max(max(coord_transform.at(0).x, coord_transform.at(1).x), coord_transform.at(2).x), coord_transform.at(3).x);;
		minZ = min(min(min(coord_transform.at(0).z, coord_transform.at(1).z), coord_transform.at(2).z), coord_transform.at(3).z);
		maxZ = max(max(max(coord_transform.at(0).z, coord_transform.at(1).z), coord_transform.at(2).z), coord_transform.at(3).z);

		if (Collision(minX, maxX, minZ, maxZ, bottomLeft.x, topRight.x, bottomLeft.z, topRight.z)) {
			velocity = 0;
			finish = true;
			std::cout << "YOU WON with a Score of: " << Score << "\n";
		}
	}
	 //primul obstacol
	 if (!col[10]) {
		 //desenare obstacol
		 modelMatrix = glm::mat4(1);
		 modelMatrix = glm::translate(modelMatrix, glm::vec3(-1, 0.25, 18));
		 modelMatrix = glm::scale(modelMatrix, glm::vec3(1.5f));
		 RenderMesh(meshes["par"], shaders["ShaderLab6"], modelMatrix, glm::vec3(0, 0, 1), time, glm::vec3(0, 0, 1));
		 //calculam coordonatele unde a fost plasat obstacolul
		 for (int i = 0; i < coord_par.size(); i++) {
			 coord_par_transform.at(i) = modelMatrix * coord_par.at(i);
		 }
		 //testam coliziunea
		 if (testCollision(coord_transform.at(0), coord_transform.at(1), coord_transform.at(2), coord_transform.at(3), coord_par_transform.at(0),
			 coord_par_transform.at(1), coord_par_transform.at(2), coord_par_transform.at(3))) {
			velocity = 0;
			nrCol++;
			Score -= Score / 2;
			 
			 col[10] = true;
		 }
	 }

	 //al doilea obstacol
	 if (!col[11]) {
		 //desenare obstacol
		 modelMatrix = glm::mat4(1);
		 modelMatrix = glm::translate(modelMatrix, glm::vec3(15, 0.25, 33));
		 modelMatrix = glm::scale(modelMatrix, glm::vec3(1.3f));
		 RenderMesh(meshes["par"], shaders["ShaderLab6"], modelMatrix, glm::vec3(0.256, 0.541, 0.874), time, glm::vec3(0.256, 0.541, 0.874));
		 //calculam coordonatele unde a fost plasat obstacolul
		 for (int i = 0; i < coord_par.size(); i++) {
			 coord_par_transform.at(i) = modelMatrix * coord_par.at(i);
		 }
		 //testare coliziune
		 if (testCollision(coord_transform.at(0), coord_transform.at(1), coord_transform.at(2), coord_transform.at(3), coord_par_transform.at(0),
			 coord_par_transform.at(1), coord_par_transform.at(2), coord_par_transform.at(3))) {
			 velocity = 0;
			 nrCol++;
			 Score -= Score / 2;
			 col[11] = true;
		 }
	}

	 //al treilea obstacol
	 if (!col[12]) {
		 //desenare obstacol
		 modelMatrix = glm::mat4(1);
		 modelMatrix = glm::translate(modelMatrix, glm::vec3(31, 0.25, 43));
		 modelMatrix = glm::scale(modelMatrix, glm::vec3(1.3f));
		 RenderMesh(meshes["par"], shaders["ShaderLab6"], modelMatrix, glm::vec3(0.256, 0.541, 0.174), time, glm::vec3(0.256, 0.541, 0.174));
		 //calculam coordonatele unde a fost plasat obstacolul
		 for (int i = 0; i < coord_par.size(); i++) {
			 coord_par_transform.at(i) = modelMatrix * coord_par.at(i);
		 }
		 //testare coliziune
		 if (testCollision(coord_transform.at(0), coord_transform.at(1), coord_transform.at(2), coord_transform.at(3), coord_par_transform.at(0),
			 coord_par_transform.at(1), coord_par_transform.at(2), coord_par_transform.at(3))) {
			 velocity = 0;
			 nrCol++;
			 Score -= Score / 2;
			 col[12] = true;
		 }
	}

	 //al patrulea obstacol
	 if (!col[13]) {
		 //desenare
		 modelMatrix = glm::mat4(1);
		 modelMatrix = glm::translate(modelMatrix, glm::vec3(17, 0.25, 63));
		 modelMatrix = glm::scale(modelMatrix, glm::vec3(1.6f));
		 RenderMesh(meshes["par"], shaders["ShaderLab6"], modelMatrix, glm::vec3(0.856, 0.241, 0.174), time, glm::vec3(0.856, 0.241, 0.174));
		 //calculam coordonatele unde a fost plasat obstacolul
		 for (int i = 0; i < coord_par.size(); i++) {
			 coord_par_transform.at(i) = modelMatrix * coord_par.at(i);
		 }
		 //testare coliziune
		 if (testCollision(coord_transform.at(0), coord_transform.at(1), coord_transform.at(2), coord_transform.at(3), coord_par_transform.at(0),
			 coord_par_transform.at(1), coord_par_transform.at(2), coord_par_transform.at(3))) {
			 velocity = 0;
			 nrCol++;
			 Score -= Score / 2;
			 col[13] = true;
		 }
	}


	 //al cincilea obstacol
	 if (!col[14]) {
		 //desenare obstacol
		 modelMatrix = glm::mat4(1);
		 modelMatrix = glm::translate(modelMatrix, glm::vec3(1, 0.25, 78));
		 modelMatrix = glm::scale(modelMatrix, glm::vec3(1.4f));
		 RenderMesh(meshes["par"], shaders["ShaderLab6"], modelMatrix, glm::vec3(0.523, 0.123, 0.123), time, glm::vec3(0.523, 0.123, 0.123));
		 //calculam coordonatele unde a fost plasat obstacolul
		 for (int i = 0; i < coord_par.size(); i++) {
			 coord_par_transform.at(i) = modelMatrix * coord_par.at(i);
		 }
		 //testare coliziune
		 if (testCollision(coord_transform.at(0), coord_transform.at(1), coord_transform.at(2), coord_transform.at(3), coord_par_transform.at(0),
			 coord_par_transform.at(1), coord_par_transform.at(2), coord_par_transform.at(3))) {
			 velocity = 0;
			 nrCol++;
			 Score -= Score / 2;
			 col[14] = true;
		 }
	}
}

void Laborator6::FrameEnd()
{

	//DrawCoordinatSystem(camera->GetViewMatrix(), projectionMatrix);
	
}

void Laborator6::RenderSimpleMesh(Mesh *mesh, Shader *shader, const glm::mat4 & modelMatrix)
{
	if (!mesh || !shader || !shader->GetProgramID())
		return;

	// render an object using the specified shader and the specified position
	glUseProgram(shader->program);
	float time = sin(Engine::GetElapsedTime());
	// TODO : get shader location for uniform mat4 "Model"
	int locationM = glGetUniformLocation(shader->program, "Model");
	// TODO : set shader uniform "Model" to modelMatrix
	glUniformMatrix4fv(locationM, 1, GL_FALSE, glm::value_ptr(modelMatrix));
	// TODO : get shader location for uniform mat4 "View"
	int locationV = glGetUniformLocation(shader->program, "View");
	// TODO : set shader uniform "View" to viewMatrix
	glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
	glUniformMatrix4fv(locationV, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	// TODO : get shader location for uniform mat4 "Projection"
	int locationP = glGetUniformLocation(shader->program, "Projection");
	// TODO : set shader uniform "Projection" to projectionMatrix
	glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
	glUniformMatrix4fv(locationP, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	int l = glGetUniformLocation(shader->program, "time");
	glUniform1f(l, time);
	// Draw the object
	glBindVertexArray(mesh->GetBuffers()->VAO);
	glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_SHORT, 0);
}

// Documentation for the input functions can be found in: "/Source/Core/Window/InputController.h" or
// https://github.com/UPB-Graphics/Framework-EGC/blob/master/Source/Core/Window/InputController.h

void Laborator6::OnInputUpdate(float deltaTime, int mods)
{
	// add key press event
	// move the camera only if MOUSE_RIGHT button is pressed
	float cameraSpeed = 2.0f;

	//desenare wireframe la apasarea tastei 'L'
	if (window->KeyHold(GLFW_KEY_L))
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
	//rotatie masina la dreapta (sens orar, deci negativ)
	if (window->KeyHold(GLFW_KEY_RIGHT)) {
		if (!finish) {
			backwards = false;
			//update unghiuri
			angularStep += deltaTime * velocity;
			carRotation -= deltaTime;
			//rotatie camere
			cameraTP->RotateThirdPerson_OY(-deltaTime);
			cameraFP->RotateFirstPerson_OY(-deltaTime);
			//deplasare masina
			tx += deltaTime * velocity * sin(carRotation);
			tz += deltaTime * velocity * cos(carRotation);
			//deplasare camere
			cameraTP->MoveForward(deltaTime * velocity);
			cameraFP->MoveForward(deltaTime * velocity);
			Score++;
		}
	}
	//rotatie masina la stanga (sens trigonometric, deci pozitiv)
	if (window->KeyHold(GLFW_KEY_LEFT)) {
		if (!finish) {
			backwards = false;
			//update unghiuri
			angularStep += deltaTime * velocity;
			carRotation += deltaTime;
			//rotatie camere
			cameraTP->RotateThirdPerson_OY(deltaTime);
			cameraFP->RotateFirstPerson_OY(deltaTime);
			//update coordonate masina
			tx += deltaTime * velocity * sin(carRotation);
			tz += deltaTime * velocity * cos(carRotation);
			//translatie camere
			cameraTP->MoveForward(deltaTime * velocity);
			cameraFP->MoveForward(deltaTime * velocity);
			Score++;
	}
		
	}
	//accelerare pana la viteza maxima si mers inainte
	if (window->KeyHold(GLFW_KEY_UP)) {
		if (!finish) {
			backwards = false;
			velocity += 0.10f;
			//verificare limita viteza
			if (velocity > maxSpeed) {
				velocity = maxSpeed;
			}
			//update coordonate masina
			tx += deltaTime * velocity * sin(carRotation);
			tz += deltaTime * velocity * cos(carRotation);
			//update unghi rotatie
			angularStep += deltaTime * velocity;
			//translatie camera
			cameraTP->MoveForward(deltaTime * velocity);
			cameraFP->MoveForward(deltaTime * velocity);
			Score++;
		}
		
	}
	//franare pana viteza ajunge la 0 si mers cu spatele
	if (window->KeyHold(GLFW_KEY_DOWN)) {
		if (!finish) {
			velocity -= 0.10f;
			//rotie se misca inapoi
			angularStep -= deltaTime * velocity;
			//viteza a ajuns negativa, deci mergem cu spatele
			if (velocity < 0) {
				backwards = true;
				velocity = 5.0f * deltaTime;
				//update coordonate
				tx -= deltaTime * velocity * sin(carRotation);
				tz -= deltaTime * velocity * cos(carRotation);
				//translatie camera
				cameraTP->MoveForward(-deltaTime * velocity);
				cameraFP->MoveForward(-deltaTime * velocity);
			}
			//viteza pozitiva, deci franam
			else if (velocity >= 0 && !backwards) {
				//update coordonate
				tx += deltaTime * velocity * sin(carRotation);
				tz += deltaTime * velocity * cos(carRotation);
				//translatie camera
				cameraTP->MoveForward(deltaTime * velocity);
				cameraFP->MoveForward(deltaTime * velocity);
			}
	}
	}
	
	if (window->KeyHold(GLFW_KEY_W)) {
		//translate the camera forward
		cameraTP->TranslateForward(cameraSpeed * deltaTime);
		cameraFP->TranslateForward(cameraSpeed * deltaTime);
	}

	if (window->KeyHold(GLFW_KEY_A)) {
		// translate the camera to the left
		cameraTP->TranslateRight(-cameraSpeed * deltaTime);
		cameraFP->TranslateRight(-cameraSpeed * deltaTime);
	}

	if (window->KeyHold(GLFW_KEY_S)) {
		//  translate the camera backwards
		cameraTP->TranslateForward(-cameraSpeed * deltaTime);
		cameraFP->TranslateForward(-cameraSpeed * deltaTime);
	}

	if (window->KeyHold(GLFW_KEY_D)) {
		//  translate the camera to the right
		cameraTP->TranslateRight(cameraSpeed * deltaTime);
		cameraFP->TranslateRight(cameraSpeed * deltaTime);
	}

	if (window->KeyHold(GLFW_KEY_Q)) {
		// translate the camera down
		cameraTP->TranslateUpword(-cameraSpeed * deltaTime);
		cameraFP->TranslateUpword(-cameraSpeed * deltaTime);
	}

	if (window->KeyHold(GLFW_KEY_E)) {
		//  translate the camera up
		cameraTP->TranslateUpword(cameraSpeed * deltaTime);
		cameraFP->TranslateUpword(cameraSpeed * deltaTime);
	}
}

void Laborator6::OnKeyPress(int key, int mods)
{
	//schimbare camere la apasarea tastei
	if (key == GLFW_KEY_T)
	{
		renderCameraTarget = !renderCameraTarget;
		if (renderCameraTarget) {
			camera = cameraTP;
		}
		else {
			camera = cameraFP;
		}
	}
}

void Laborator6::OnKeyRelease(int key, int mods)
{
	// add key release event
}

void Laborator6::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
	{
		float sensivityOX = 0.001f;
		float sensivityOY = 0.001f;

		if (window->GetSpecialKeyState() == 0) {
			renderCameraTarget = false;
			// rotate the camera in First-person mode around OX and OY using deltaX and deltaY
			// use the sensitivity variables for setting up the rotation speed
			cameraFP->RotateFirstPerson_OX(deltaY * sensivityOX);
			cameraFP->RotateFirstPerson_OY(deltaX * sensivityOY);
		}

		if (window->GetSpecialKeyState() && GLFW_MOD_CONTROL) {
			renderCameraTarget = true;
			// rotate the camera in Third-person mode around OX and OY using deltaX and deltaY
			// use the sensitivity variables for setting up the rotation speed
			camera->RotateThirdPerson_OX(deltaY * sensivityOX);
			camera->RotateThirdPerson_OY(deltaX * sensivityOY);
		}

	}
}

void Laborator6::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button press event
}

void Laborator6::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button release event
}

void Laborator6::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}

void Laborator6::OnWindowResize(int width, int height)
{
}

//creare paralelipiped pe baza coltului stanga jos
Mesh* Laborator6::CreateParallelepiped(std::string name, glm::vec3 leftBottomCorner, float depth, float length, float width, glm::vec3 color)
{
	glm::vec3 corner = leftBottomCorner;

	//coordonatele varfurilor in functie de lungime, latime si adancime
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
	//laturi
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

//randare mesa folosind vertex shader
void Laborator6::RenderMesh(Mesh * mesh, Shader * shader, const glm::mat4 & modelMatrix, glm::vec3 color, float time, glm::vec3 futureColor)
{
	if (!mesh || !shader || !shader->program)
		return;

	// render an object using the specified shader and the specified position
	shader->Use();
	
	//folosim matricea de vizualizare specifica camerei
	glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
	//folosim matricea de proiectie definita in init
	glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	//folosim matricea de modelare primita ca parametru
	glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	//uniforma pentru culoarea curenta a obiectului
	int loc_color = glGetUniformLocation(shader->program, "obj_color");
	glUniform3fv(loc_color, 1, glm::value_ptr(color));

	//uniforma pentru culoarea obiectului din urmatorul stagiu al zilei
	int loc_colorF = glGetUniformLocation(shader->program, "future_color");
	glUniform3fv(loc_colorF, 1, glm::value_ptr(futureColor));

	//uniforma pentru timp
	int loc_time = glGetUniformLocation(shader->program, "time");
	glUniform1f(loc_time,time);
	mesh->Render();
}

//creare dreptunghi pornind de la coltul stanga jos
Mesh* Laborator6::CreateRectangle(std::string name, glm::vec3 leftBottomCorner, float length, float width, glm::vec3 color)
{
	glm::vec3 corner = leftBottomCorner;

	//varfurile dreptunghiului folosind lungime si adancime
	std::vector<VertexFormat> vertices =
	{
		VertexFormat(corner, color),
		VertexFormat(corner + glm::vec3(length, 0, 0), color),
		VertexFormat(corner + glm::vec3(length, 0, -width), color),
		VertexFormat(corner + glm::vec3(0, 0, -width), color)
	};

	Mesh* rectangle = new Mesh(name);
	//laturile dreptunghiului
	std::vector<unsigned short> indices = { 0, 1, 2, 3, 0, 2 };

	rectangle->InitFromData(vertices, indices);
	return rectangle;
}

//creare drum
Mesh* Laborator6::CreateRoad(std::string name, glm::vec3 leftBottomCorner, float length, float width, glm::vec3 color)
{
	glm::vec3 corner = leftBottomCorner;

	//varfurile dreptunghiului
	std::vector<VertexFormat> vertices =
	{
		VertexFormat(corner, color),
		VertexFormat(corner + glm::vec3(length, 0, 0), color),
		VertexFormat(corner + glm::vec3(length, 0, width), color),
		VertexFormat(corner + glm::vec3(0, 0, width), color)
	};

	Mesh* rectangle = new Mesh(name);
	//laturi
	std::vector<unsigned short> indices = { 0, 1, 2, 3, 0, 2 };
	
	rectangle->InitFromData(vertices, indices);
	return rectangle;
}

//verificare coliziune intre 2 dreptunghiuri
bool Laborator6::Collision(double xmin, double xmax, double ymin, double ymax, double xl, double xr, double yl, double yr) {

	//verificam daca un dreptunghiu se afla la stangla celuilalt
	if (xl < xmin && xr < xmin)
		return false;
	//verificam daca un dreptunghiu se afla sub celuilalt
	if (yl < ymin && yr < ymin)
		return false;
	//verificam daca un dreptunghiu se afla la dreapta celuilalt
	if (xl > xmax && xr > xmax)
		return false;
	//verificam daca un dreptunghiu se afla deasupra celuilalt
	if (yl > ymax && yr > ymax)
		return false;

	return true;
}
