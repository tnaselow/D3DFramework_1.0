#include "Ocean.h"
#include <DX/D3D11.h>
#include <DX/D3DX11.h>
#include <DX/D3DX10.h>
#include "Renderer_D3D.h"
#include "Utils.h"
#include <minwinbase.h>
#include <glm/gtc/matrix_transform.inl>
#include <float.h>
//unsigned int fp_control_state = _controlfp_s(_EM_INEXACT, _MCW_EM);

#define PI 3.14159265359f

namespace
{
	void createVertandIndBuffers(Ocean::OceanMesh &mesh)
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.ByteWidth = mesh.m_VertexCount * sizeof(Ocean::Vertex);
		desc.Usage = D3D11_USAGE_DYNAMIC;

		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(data));
		data.pSysMem = mesh.m_Vertices.data();
		HR(Renderer_D3D::getDevice()->CreateBuffer(&desc, &data, &mesh.m_VertBuffer));

		ZeroMemory(&desc, sizeof(desc));
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		desc.ByteWidth = mesh.m_IndicesCount * sizeof(UINT);
		desc.Usage = D3D11_USAGE_DYNAMIC;

		ZeroMemory(&data, sizeof(data));
		data.pSysMem = mesh.m_Indices.data();
		HR(Renderer_D3D::getDevice()->CreateBuffer(&desc, &data, &mesh.m_IndexBuffer));
	}

	float uniformRandomVariable() {
		return (float)rand() / RAND_MAX;
	}

	std::complex<float> gaussianRandomVariable() {
		float x1, x2, w;
		do {
			x1 = 2.f * uniformRandomVariable() - 1.f;
			x2 = 2.f * uniformRandomVariable() - 1.f;
			w = x1 * x1 + x2 * x2;
		} while (w >= 1.f);
		w = sqrt((-2.f * log(w)) / w);
		return std::complex<float>(x1 * w, x2 * w);
	}
}

namespace Ocean
{
	OceanMesh constructOcean(const int N, const float A, const glm::vec2 w, const float length, bool geometry)
	{
		OceanMesh mesh;

		mesh.N = N;
		mesh.Nplus1 = N + 1;
		mesh.A = A;
		mesh.w = w;
		mesh.g = 2.8f;
		mesh.length = length;
	
		mesh.h_tilde.resize(N * N);
		mesh.h_tilde_slopex.resize(N * N);
		mesh.h_tilde_slopez.resize(N * N);
		mesh.h_tilde_dx.resize(N * N);
		mesh.h_tilde_dz.resize(N * N);
		mesh.m_Vertices.resize((N + 1) * (N + 1));
		mesh.m_Indices.resize((N + 1) * (N + 1) * 10);

		int index;
		std::complex<float> htilde0, htilde0Conj;
		for(int z = 0; z < N + 1; ++z)
		{
			for(int x = 0; x < N + 1; ++x)
			{
				index = z * (N + 1) + x;

				htilde0 = mesh.hTilde_0(x, z);
				htilde0Conj = conj(mesh.hTilde_0(-x, -z));
				
				mesh.m_Vertices[index].m_HTile0.x = htilde0.real();
				mesh.m_Vertices[index].m_HTile0.y = htilde0.imag();

				mesh.m_Vertices[index].m_HTile0Conj.x = htilde0Conj.real(); 
				mesh.m_Vertices[index].m_HTile0Conj.y = htilde0Conj.imag();


				mesh.m_Vertices[index].m_OrigPos.x = mesh.m_Vertices[index].m_NewPos.x = (x - N / 2.0f) * length / N;
				mesh.m_Vertices[index].m_OrigPos.y = mesh.m_Vertices[index].m_NewPos.y = -0.1f;
				mesh.m_Vertices[index].m_OrigPos.z = mesh.m_Vertices[index].m_NewPos.z = ((z - N / 2.0f) * length / N) - 0.5f;
				
				mesh.m_Vertices[index].m_Normal.x = 0.0f;
				mesh.m_Vertices[index].m_Normal.y = 1.0f;
				mesh.m_Vertices[index].m_Normal.z = 0.0f;
			}
		}

		int Nplus1 = N + 1;
		mesh.m_IndicesCount = 0;
		mesh.m_VertexCount = index + 1;
		for (int z = 0; z < N; z++) {
			for (int x = 0; x < N; x++) {
				index = z * (N + 1) + x;

				if (geometry) {
					mesh.m_Indices[mesh.m_IndicesCount++] = index;				// lines
					mesh.m_Indices[mesh.m_IndicesCount++] = index + 1;
					mesh.m_Indices[mesh.m_IndicesCount++] = index;
					mesh.m_Indices[mesh.m_IndicesCount++] = index + Nplus1;
					mesh.m_Indices[mesh.m_IndicesCount++] = index;
					mesh.m_Indices[mesh.m_IndicesCount++] = index + Nplus1 + 1;
					if (x == N - 1) {
						mesh.m_Indices[mesh.m_IndicesCount++] = index + 1;
						mesh.m_Indices[mesh.m_IndicesCount++] = index + Nplus1 + 1;
					}
					if (z == N - 1) {
						mesh.m_Indices[mesh.m_IndicesCount++] = index + Nplus1;
						mesh.m_Indices[mesh.m_IndicesCount++] = index + Nplus1 + 1;
					}
				}
				else {
					mesh.m_Indices[mesh.m_IndicesCount++] = index;				// two triangles
					mesh.m_Indices[mesh.m_IndicesCount++] = index + Nplus1;
					mesh.m_Indices[mesh.m_IndicesCount++] = index + Nplus1 + 1;
					mesh.m_Indices[mesh.m_IndicesCount++] = index;
					mesh.m_Indices[mesh.m_IndicesCount++] = index + Nplus1 + 1;
					mesh.m_Indices[mesh.m_IndicesCount++] = index + 1;
				}
			}
		}

		createVertandIndBuffers(mesh);
		return mesh;
	}

	float OceanMesh::dispersion( int n_prime, int m_prime) {
		float w_0 = 2.0f * PI / 200.0f;
		float kx = PI * (2 * n_prime - N) / length;
		float kz = PI * (2 * m_prime - N) / length;
		return floor(sqrt(g * sqrt(kx * kx + kz * kz)) / w_0) * w_0;
	}

	float OceanMesh::phillips(int n_prime, int m_prime) {
		glm::vec2 k(PI * (2 * n_prime - N) / length,
			PI * (2 * m_prime - N) / length);
		float k_length = k.length();
		if (k_length < 0.000001) return 0.0;

		float k_length2 = k_length  * k_length;
		float k_length4 = k_length2 * k_length2;

		
		float k_dot_w = glm::dot(glm::normalize(k), glm::normalize(w));
		float k_dot_w2 = k_dot_w * k_dot_w;

		float w_length = w.length();
		float L = w_length * w_length / g;
		float L2 = L * L;

		float damping = 0.001;
		float l2 = L2 * damping * damping;

		return A * exp(-1.0f / (k_length2 * L2)) / k_length4 * k_dot_w2 * exp(-k_length2 * l2);
	}



	std::complex<float> OceanMesh::hTilde_0(int n_prime, int m_prime) {
		std::complex<float > r = gaussianRandomVariable();
		return r * sqrt(phillips(n_prime, m_prime) / 2.0f);
	}

	std::complex<float> OceanMesh::hTilde(float t, int n_prime, int m_prime) {
		int index = m_prime * Nplus1 + n_prime;

		std::complex<float>  htilde0(m_Vertices[index].m_HTile0.x, m_Vertices[index].m_HTile0.y);
		std::complex<float>  htilde0mkconj(m_Vertices[index].m_HTile0Conj.x, m_Vertices[index].m_HTile0Conj.y);

		float omegat = dispersion(n_prime, m_prime) * t;

		float cos_ = cos(omegat);
		float sin_ = sin(omegat);

		std::complex<float> c0(cos_, sin_);
		std::complex<float> c1(cos_, -sin_);

		std::complex<float> res = htilde0 * c0 + htilde0mkconj * c1;

		return htilde0 * c0 + htilde0mkconj*c1;
	}

	complex_offset_normal OceanMesh::h_D_and_n(glm::vec2 x, float t) {
		std::complex<float> h(0.0f, 0.0f);
		glm::vec2 D(0.0f, 0.0f);
		glm::vec3 n(0.0f, 0.0f, 0.0f);

		std::complex<float> c, res, htilde_c;
		glm::vec2 k;
		float kx, kz, k_length, k_dot_x;

		for (int m_prime = 0; m_prime < N; m_prime++) {
			kz = 2.0f * PI * (m_prime - N / 2.0f) / length;
			for (int n_prime = 0; n_prime < N; n_prime++) {
				kx = 2.0f * PI * (n_prime - N / 2.0f) / length;
				k = glm::vec2(kx, kz);

				k_length = k.length();
				k_dot_x = dot(k, x);

				c = std::complex<float>(cos(k_dot_x), sin(k_dot_x));
				htilde_c = hTilde(t, n_prime, m_prime) * c;

				h = h + htilde_c;

				n = n + glm::vec3(-kx * htilde_c.imag(), 0.0f, -kz * htilde_c.imag());

				if (k_length < 0.000001) continue;
				D = D + glm::vec2(kx / k_length * htilde_c.imag(), kz / k_length * htilde_c.imag());
			}
		}

		n = normalize(glm::vec3(0.0f, 1.0f, 0.0f) - n);

		complex_offset_normal cvn;
		cvn.m_H = h;
		cvn.m_Offset = D;
		cvn.m_Normal = n;
		return cvn;
	}


	void OceanMesh::evaluateWaves(float t) {
		float lambda = -1.0;
		int index;
		glm::vec2 x;
		//glm::vec2 d;
		complex_offset_normal h_d_and_n;
		for (int m_prime = 0; m_prime < N; m_prime++) {
			for (int n_prime = 0; n_prime < N; n_prime++) {
				index = m_prime * Nplus1 + n_prime;

				x = glm::vec2(m_Vertices[index].m_NewPos.x, m_Vertices[index].m_NewPos.z);

				h_d_and_n = h_D_and_n(x, t);

				m_Vertices[index].m_NewPos.y = h_d_and_n.m_H.real();

				m_Vertices[index].m_NewPos.x = m_Vertices[index].m_OrigPos.x + lambda*h_d_and_n.m_Offset.x;
				m_Vertices[index].m_NewPos.z = m_Vertices[index].m_OrigPos.z + lambda*h_d_and_n.m_Offset.y;

				m_Vertices[index].m_Normal.x = h_d_and_n.m_Normal.x;
				m_Vertices[index].m_Normal.y = h_d_and_n.m_Normal.y;
				m_Vertices[index].m_Normal.z = h_d_and_n.m_Normal.z;

				if (n_prime == 0 && m_prime == 0) {
					m_Vertices[index + N + Nplus1 * N].m_NewPos.y = h_d_and_n.m_H.real();

					m_Vertices[index + N + Nplus1 * N].m_NewPos.x = m_Vertices[index + N + Nplus1 * N].m_OrigPos.x + lambda*h_d_and_n.m_Offset.x;
					m_Vertices[index + N + Nplus1 * N].m_NewPos.z = m_Vertices[index + N + Nplus1 * N].m_OrigPos.z + lambda*h_d_and_n.m_Offset.y;

					m_Vertices[index + N + Nplus1 * N].m_Normal.x = h_d_and_n.m_Normal.x;
					m_Vertices[index + N + Nplus1 * N].m_Normal.y = h_d_and_n.m_Normal.y;
					m_Vertices[index + N + Nplus1 * N].m_Normal.z = h_d_and_n.m_Normal.z;
				}
				if (n_prime == 0) {
					m_Vertices[index + N].m_NewPos.y = h_d_and_n.m_H.real();

					m_Vertices[index + N].m_NewPos.x = m_Vertices[index + N].m_OrigPos.x + lambda*h_d_and_n.m_Offset.x;
					m_Vertices[index + N].m_NewPos.y = m_Vertices[index + N].m_OrigPos.z + lambda*h_d_and_n.m_Offset.y;

					m_Vertices[index + N].m_Normal.x = h_d_and_n.m_Normal.x;
					m_Vertices[index + N].m_Normal.y = h_d_and_n.m_Normal.y;
					m_Vertices[index + N].m_Normal.z = h_d_and_n.m_Normal.z;
				}
				if (m_prime == 0) {
					m_Vertices[index + Nplus1 * N].m_NewPos.y = h_d_and_n.m_H.real();

					m_Vertices[index + Nplus1 * N].m_NewPos.x = m_Vertices[index + Nplus1 * N].m_OrigPos.x + lambda*h_d_and_n.m_Offset.x;
					m_Vertices[index + Nplus1 * N].m_NewPos.z = m_Vertices[index + Nplus1 * N].m_OrigPos.z + lambda*h_d_and_n.m_Offset.y;

					m_Vertices[index + Nplus1 * N].m_Normal.x = h_d_and_n.m_Normal.x;
					m_Vertices[index + Nplus1 * N].m_Normal.y = h_d_and_n.m_Normal.y;
					m_Vertices[index + Nplus1 * N].m_Normal.z = h_d_and_n.m_Normal.z;
				}
			}
		}
	}
}