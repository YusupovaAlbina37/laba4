#pragma once
#include "technique.h"

struct DirectionalLight //������ ����������� ������������� �����
	//���� ��� ������ ������� ����� �����������, � ������������ �����������
{
	vec3 Color; // ����. ���� ���������� ����� ����� ����� ����� ������� � � ����� �������������
	//������: ���� ���� (1.0, 0.5, 0.0), �� ������� ����� ��������� ���������, ������� ����������, � ����� ���������� �������
	float AmbientIntensity; // ������� �������������
	//������� ������������� ���������� ��������� ������ ��� ����� ����. �� ������ ����� ��������� ����� � �������������� 1.0, 
	//����� ������ ����� ������ �������, ��� 0.1, ����� ������ ���������� ����� ����� ������ � ������� ������.
};
class LightingTechnique : public Technique //������������ ����� ��������� �����
{
public:
	LightingTechnique();
	virtual bool Init(); //�������� Technique::AddShader() � Techique::Finalize() ��� ��������� ��������� GLSL.

	void SetWVP(const mat4* WVP);
	void SetTextureUnit(unsigned int TextureUnit); 
	void SetDirectionalLight(const DirectionalLight& Light);

private:
	GLuint m_WVPLocation;
	GLuint m_samplerLocation;
	GLuint m_dirLightColorLocation;
	GLuint m_dirLightAmbientIntensityLocation;
};