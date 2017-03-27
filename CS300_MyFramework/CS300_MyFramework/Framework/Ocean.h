#pragma once
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <complex>
#include "Mesh.h"

namespace Ocean
{
	struct Vertex
	{
		glm::vec3 m_NewPos;
		glm::vec3 m_Normal;
		glm::vec3 m_HTile0;
		glm::vec3 m_HTile0Conj;
		glm::vec3 m_OrigPos;
	};

	struct complex_offset_normal
	{
		std::complex<float> m_H; // height
		glm::vec2 m_Offset;
		glm::vec3 m_Normal;
	};


	//NOTE: these never get cleaned up in this implementation - to note
	struct OceanMesh
	{
		// think of a better way to organize this 
		std::vector<std::complex<float> > h_tilde;						
		std::vector<std::complex<float> > h_tilde_slopex;
		std::vector<std::complex<float> > h_tilde_slopez;
		std::vector<std::complex<float> > h_tilde_dx;
		std::vector<std::complex<float> > h_tilde_dz;
		unsigned m_IndicesCount = 0;
		unsigned m_VertexCount = 0;

		std::vector<Ocean::Vertex> m_Vertices;
		std::vector<UINT> m_Indices;
		
		ID3D11Buffer *m_VertBuffer;
		ID3D11Buffer *m_IndexBuffer;

		int N;
		int Nplus1;
		float A;
		glm::vec2 w;
		float length;
		float g;


		float dispersion(int n_prime, int m_prime);		// deep water
		float phillips(int n_prime, int m_prime);		// phillips spectrum
		std::complex<float> hTilde_0(int n_prime, int m_prime);
		std::complex<float> hTilde(float t, int n_prime, int m_prime);
		complex_offset_normal h_D_and_n(glm::vec2 x, float t);
		void evaluateWaves(float t);
		void evaluateWavesFFT(float t);
	};


	OceanMesh constructOcean(const int N, const float A, const glm::vec2 w, const float length, bool geometry);
	void release();


}
