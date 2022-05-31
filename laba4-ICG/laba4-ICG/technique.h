#pragma once
#include <iostream>
#include <GL/glew.h>
#include <list>
#include "camera4.h"

class Technique
{
public:
	Technique();
	~Technique();
	virtual bool Init(); //������������� ������ � ������� ���� �������
	void Enable(); //������� Enable() �� ����� ���� ��������� glUseProgram(), ��� ��� ��� ������ ���� ������� ������ ���, 
	//����� �� ������ ������� ����� ������� ������� ���������

protected:
	bool AddShader(GLenum ShaderType, const char* pShaderText);
	bool Finalize(); //���������� ��� �������� ��������
	GLint GetUniformLocation(const char* pUniformName); 

private:
	GLuint m_shaderProg;
	typedef std::list<GLuint> ShaderObjList;
	ShaderObjList m_shaderObjList;
};