#pragma once
#include "technique.h"

struct DirectionalLight //начало определения направленного света
	//Пока что только фоновая часть реализована, а направленная отсутствует
{
	vec3 Color; // цвет. Цвет определяет какой канал цвета может отражен и в какой интенсивности
	//Пример: если цвет (1.0, 0.5, 0.0), то красный канал отразится полностью, зеленый наполовину, а синий потеряется целиком
	float AmbientIntensity; // фоновая интенсивность
	//Фоновая интенсивность определяет насколько темный или яркий свет. Вы можете иметь полностью белый с интенсивностью 1.0, 
	//тогда объект будет хорошо освещен, или 0.1, тогда объект разглядеть можно будет только с большим трудом.
};
class LightingTechnique : public Technique //представляет собой инвентарь света
{
public:
	LightingTechnique();
	virtual bool Init(); //вызывает Technique::AddShader() и Techique::Finalize() для генерации программы GLSL.

	void SetWVP(const mat4* WVP);
	void SetTextureUnit(unsigned int TextureUnit); 
	void SetDirectionalLight(const DirectionalLight& Light);

private:
	GLuint m_WVPLocation;
	GLuint m_samplerLocation;
	GLuint m_dirLightColorLocation;
	GLuint m_dirLightAmbientIntensityLocation;
};