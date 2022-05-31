#include <stdio.h>
#include <iostream>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <Magick++.h>

#include "pipeline4.h"
#include "camera4.h"
#include "texture4.h"
#include "lighting_technique.h"
#include "glut_backend.h"
#include "util.h"

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768

using namespace glm;

//GLuint gWVPLocation;
//GLuint gSampler;

struct Vertex////
{
	vec3 m_pos;
	vec2 m_tex;
	vec3 m_normal;

	Vertex() {}

	Vertex(vec3 pos, vec2 tex)
	{
		m_pos = pos;
		m_tex = tex;
		m_normal = vec3(0.0f, 0.0f, 0.0f);
	}
};////

class Main : public ICallbacks
{
public:
	Main() 
	{
		m_pGameCamera = NULL;
		m_pTexture = NULL;
		m_pEffect = NULL;
		m_scale = 0.0f;
		m_directionalLight.Color = vec3(1.0f, 1.0f, 1.0f); //установка цвета света
		m_directionalLight.AmbientIntensity = 0.5f; //установка инсенсивности фонового освещения

		m_directionalLight.DiffuseIntensity = 0.75f;//установка интесивности рассеивания света
		m_directionalLight.Direction = vec3(1.0f, 0.0, 0.0);// установка направления света в мировом пространстве
	}
	~Main() 
	{
		delete m_pEffect;
		delete m_pGameCamera;
		delete m_pTexture;
	}
	bool Init() //берет на себя заботу о создании эффектов, загрузки текстуры и создание буферов вершин и индексов
	{
		vec3 Pos(0.0f, 0.0f, -3.0f);
		vec3 Target(0.0f, 0.0f, 1.0f);
		vec3 Up(0.0, 1.0f, 0.0f);
		
		m_pGameCamera = new Camera(WINDOW_WIDTH, WINDOW_HEIGHT, Pos, Target, Up);////

		unsigned int Indices[] = { 0, 3, 1,
								   1, 3, 2,
								   2, 3, 0,
								   1, 2, 0 };

		CreateIndexBuffer(Indices, sizeof(Indices));
		CreateVertexBuffer(Indices, ARRAY_SIZE_IN_ELEMENTS(Indices));

		m_pEffect = new LightingTechnique();

		if (!m_pEffect->Init())
		{
			printf("Error initializing the lighting technique\n");
			return false;
		}

		m_pEffect->Enable();

		m_pEffect->SetTextureUnit(0);

		m_pTexture = new Texture(GL_TEXTURE_2D, "C://test.png"); //создаем объект Текстуры и загружаем его

		if (!m_pTexture->Load()) //вызываем функцию для того чтобы проверить на ошибки
		{
			return false;
		}

		return true;
	} 
	void Run() //вызывает GLUTBackendRun() и передает себя в качестве параметра
	{
		GLUTBackendRun(this); 
	}
	virtual void RenderSceneCB() 
	{
		m_pGameCamera->OnRender();

		glClear(GL_COLOR_BUFFER_BIT);

		m_scale += 0.01f;

		Pipeline p;
		//p.Rotate(0.0f, Scale, 0.0f);
		p.WorldPos(0.0f, 0.0f, 3.0f);
		p.SetCamera(m_pGameCamera->GetPos(), m_pGameCamera->GetTarget(), m_pGameCamera->GetUp());
		p.SetPerspectiveProj(60.0f, WINDOW_WIDTH, WINDOW_HEIGHT, 1.0f, 100.0f);

		m_pEffect->SetWVP(p.GetWVPTrans());////

		const mat4* WorldTransformation = p.GetWorldTrans();////
		m_pEffect->SetWorldMatrix(WorldTransformation);////

		m_pEffect->SetDirectionalLight(m_directionalLight);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glEnableVertexAttribArray(2);////

		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)12);

		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)20);////

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
		m_pTexture->Bind(GL_TEXTURE0);
		glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		glDisableVertexAttribArray(2);////

		glutSwapBuffers();
	}
	virtual void IdleCB() 
	{
		RenderSceneCB();
	}
	virtual void SpecialKeyboardCB(int Key, int x, int y) 
	{
		m_pGameCamera->OnKeyboard(Key);
	}
	virtual void KeyboardCB(unsigned char Key, int x, int y) 
	{
		switch (Key) 
		{
		case 'q':
			exit(0);
		case 'a': //увеличение интенсивности фонового освещения
			m_directionalLight.AmbientIntensity += 0.05f;
			break;
		case 's': //уменьшение интеснивности фонового освещения
			m_directionalLight.AmbientIntensity -= 0.05f;
			break;

		case 'z': //увеличение интенсивности рассеивания света
			m_directionalLight.DiffuseIntensity += 0.05f;
			break;
		case 'x': //уменьшение интеснивности рассеивания света
			m_directionalLight.DiffuseIntensity -= 0.05f;
			break;
		}
	}
	virtual void PassiveMouseCB(int x, int y)
	{
		m_pGameCamera->OnMouse(x, y);
	}

private:

	//функция принимает массив индексов, получает вершины треугольников, полагаясь на них, и вычисляет нормали
	void CalcNormals(const unsigned int* pIndices, unsigned int IndexCount, Vertex* pVertices, unsigned int VertexCount) 
	{
		for (unsigned int i = 0; i < IndexCount; i += 3) //набираем нормали для каждой тройки вершин
		{
			unsigned int Index0 = pIndices[i];
			unsigned int Index1 = pIndices[i + 1];
			unsigned int Index2 = pIndices[i + 2];
			vec3 v1 = pVertices[Index1].m_pos - pVertices[Index0].m_pos;
			vec3 v2 = pVertices[Index2].m_pos - pVertices[Index0].m_pos;
			vec3 Normal = cross(v1, v2);
			Normal= normalize(Normal); //Перед добавлением нормаль в массив, ее нормируем. Причина в том, что 
			//результат векторного произведения не обязательно единичный вектор
			pVertices[Index0].m_normal += Normal;
			pVertices[Index1].m_normal += Normal;
			pVertices[Index2].m_normal += Normal;
		}

		for (unsigned int i = 0; i < VertexCount; i++) //cканируем массив вершин напрямую (больше нам не нужны индексы) и нормируем нормаль каждой вершины
		{
			pVertices[i].m_normal= normalize(pVertices[i].m_normal);
		}
	}////

	void CreateVertexBuffer(const unsigned int* pIndices, unsigned int IndexCount)
	{
		Vertex Vertices[4] = {
		Vertex(vec3(-1.0f, -1.0f, 0.5f), vec2(0.0f, 0.0f)),
		Vertex(vec3(0.0f, -1.0f, -1.0f), vec2(0.5f, 0.0f)),
		Vertex(vec3(1.0f, -1.0f, 0.5f),  vec2(1.0f, 0.0f)),
		Vertex(vec3(0.0f, 1.0f, 0.0f),   vec2(0.5f, 1.0f)) };

		unsigned int VertexCount = ARRAY_SIZE_IN_ELEMENTS(Vertices);////

		CalcNormals(pIndices, IndexCount, Vertices, VertexCount);////

		glGenBuffers(1, &m_VBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
	}

	void CreateIndexBuffer(const unsigned int* pIndices, unsigned int SizeInBytes)
	{
		glGenBuffers(1, &m_IBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, SizeInBytes, pIndices, GL_STATIC_DRAW);////
	}

	GLuint m_VBO;
	GLuint m_IBO;
	LightingTechnique* m_pEffect;
	Texture* m_pTexture;
	Camera* m_pGameCamera;
	float m_scale;
	DirectionalLight m_directionalLight;
};

int main(int argc, char **argv)
{
	GLUTBackendInit(argc, argv);

	if (!GLUTBackendCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, 32, false, "Tutorial 18")) 
	{
		return 1;
	}

	Main* pApp = new Main();

	if (!pApp->Init())
	{
		return 1;
	}

	pApp->Run();

	delete pApp;

	return 0;
}