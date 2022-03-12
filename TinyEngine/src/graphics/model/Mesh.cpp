#include "Mesh.h"

namespace TEngine {
	Mesh::Mesh() : mVAO(0), mVBO(0), mEBO(0) {}

	Mesh::Mesh(std::vector<glm::vec3>& positions, std::vector<glm::vec2>& uvs, std::vector<glm::vec3>& normals, std::vector<unsigned int>& indices)
		: mPositions(positions), mUVs(uvs), mNormals(normals), mIndices(indices), mVAO(0), mVBO(0), mEBO(0) {}

	Mesh::Mesh(std::vector<glm::vec3>& positions, std::vector<glm::vec2>& uvs, std::vector<glm::vec3>& normals, std::vector<glm::vec3>& tangents, std::vector<glm::vec3>& bitangents, std::vector<unsigned int>& indices)
		: mPositions(positions), mUVs(uvs), mNormals(normals), mTangents(tangents), mBitangents(bitangents), mIndices(indices), mVAO(0), mVBO(0), mEBO(0) {}


	Mesh::~Mesh()
	{
	}

	void Mesh::Draw() const
	{
		// take care: need to setup materials in model's draw first

		glBindVertexArray(mVAO);
		if (mIndices.size() > 0)
			glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, 0);
		else
			glDrawArrays(GL_TRIANGLES, 0, mPositions.size());
		glBindVertexArray(0);
	}

	// interleaved == true  vertex layout: position1, normal1, uv1, position2, normal2, uv2,...
	// interleaved == false vertex layout: position1, position2,...,normal1, normal2,...,uv1, uv2,...
	void Mesh::SetupMesh(bool interleaved)
	{
		// preprocess the mesh data in the specified layout
		std::vector<float> data;
		if (interleaved)
		{
			for (unsigned int i = 0; i < mPositions.size(); i++)
			{
				data.push_back(mPositions[i].x);
				data.push_back(mPositions[i].y);
				data.push_back(mPositions[i].z);

				if (mNormals.size() > 0)
				{
					data.push_back(mNormals[i].x);
					data.push_back(mNormals[i].y);
					data.push_back(mNormals[i].z);
				}
				if (mUVs.size() > 0)
				{
					data.push_back(mUVs[i].x);
					data.push_back(mUVs[i].y);
				}
				if (mTangents.size() > 0)
				{
					data.push_back(mTangents[i].x);
					data.push_back(mTangents[i].y);
					data.push_back(mTangents[i].z);
				}
				if (mBitangents.size() > 0)
				{
					data.push_back(mBitangents[i].x);
					data.push_back(mBitangents[i].y);
					data.push_back(mBitangents[i].z);
				}

			}
		}
		else
		{
			for (unsigned int i = 0; i < mPositions.size(); i++)
			{
				data.push_back(mPositions[i].x);
				data.push_back(mPositions[i].y);
				data.push_back(mPositions[i].z);
			}
			for (unsigned int i = 0; i < mNormals.size(); i++)
			{
				data.push_back(mNormals[i].x);
				data.push_back(mNormals[i].y);
				data.push_back(mNormals[i].z);
			}
			for (unsigned int i = 0; i < mUVs.size(); i++)
			{
				data.push_back(mUVs[i].x);
				data.push_back(mUVs[i].y);
			}
			for (unsigned int i = 0; i < mTangents.size(); i++)
			{
				data.push_back(mTangents[i].x);
				data.push_back(mTangents[i].y);
				data.push_back(mTangents[i].z);
			}
			for (unsigned int i = 0; i < mBitangents.size(); i++)
			{
				data.push_back(mBitangents[i].x);
				data.push_back(mBitangents[i].y);
				data.push_back(mBitangents[i].z);
			}
		}

		// compute the component count
		unsigned int bufferComponentCount = 0;
		if (mPositions.size() > 0)	bufferComponentCount += 3;
		if (mNormals.size() > 0)	bufferComponentCount += 3;
		if (mUVs.size() > 0)		bufferComponentCount += 2;
		if (mTangents.size() > 0)	bufferComponentCount += 3;
		if (mBitangents.size() > 0)	bufferComponentCount += 3;

		// setup vao, vbo and ebo
		glGenVertexArrays(1, &mVAO);
		glGenBuffers(1, &mVBO);
		glGenBuffers(1, &mEBO);

		glBindVertexArray(mVAO);
		glBindBuffer(GL_ARRAY_BUFFER, mVBO);
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);

		// bind ebo if necessary
		if (mIndices.size() > 0)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(unsigned int), &mIndices[0], GL_STATIC_DRAW);
		}

		if (interleaved)
		{
			// 先走offset距离，再走一步步的stride
			unsigned int stride = bufferComponentCount * sizeof(float);
			unsigned int offset = 0;


			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)offset);
			offset += 3 * sizeof(float);
			if (mNormals.size() > 0)
			{
				glEnableVertexAttribArray(1);
				glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)offset);
				offset += 3 * sizeof(float);
			}
			if (mUVs.size() > 0)
			{
				glEnableVertexAttribArray(2);
				glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)offset);
				offset += 2 * sizeof(float);
			}
			if (mTangents.size() > 0)
			{
				glEnableVertexAttribArray(3);
				glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (void*)offset);
				offset += 3 * sizeof(float);
			}
			if (mBitangents.size() > 0)
			{
				glEnableVertexAttribArray(4);
				glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, stride, (void*)offset);
				offset += 3 * sizeof(float);
			}

		}
		else
		{
			unsigned int offset = 0;

			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)offset);
			offset += mPositions.size() * 3 * sizeof(float);
			if (mNormals.size() > 0)
			{
				glEnableVertexAttribArray(1);
				glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)offset);
				offset += mNormals.size() * 3 * sizeof(float);
			}
			if (mUVs.size() > 0)
			{
				glEnableVertexAttribArray(2);
				glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)offset);
				offset += mUVs.size() * 2 * sizeof(float);
			}
			if (mTangents.size() > 0)
			{
				glEnableVertexAttribArray(3);
				glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void*)offset);
				offset += mTangents.size() * 3 * sizeof(float);
			}
			if (mBitangents.size() > 0)
			{
				glEnableVertexAttribArray(4);
				glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, (void*)offset);
				offset += mBitangents.size() * 3 * sizeof(float);
			}
		}

		glBindVertexArray(0);
	}
	void Mesh::OnGui() {
		
		//later maybe show pos, normal uv?

		//show material
		if (ImGui::TreeNode("Material"))
		{
			mMaterial.OnGui();
			ImGui::TreePop();
		}
		
	}
}