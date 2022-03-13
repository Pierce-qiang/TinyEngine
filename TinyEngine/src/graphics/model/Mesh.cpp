#include "Mesh.h"
#include <iostream>
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
		/*for (unsigned int i = 0;i<mBoneIDs.size();i++)
		{
			std::cout << "vertex " << i << " " << mBoneIDs[i][0] << mBoneIDs[i][1] << mBoneIDs[i][2] << mBoneIDs[i][3]<<"\n";
		}*/

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
		//pass data should be careful
		if (interleaved)
		{
			std::vector<Vertex> vertices;
			for (unsigned int i = 0;i<mPositions.size();++i)
			{
				Vertex newVertex;
				if (mPositions.size()>0)
					newVertex.Position = mPositions[i];
				if(mNormals.size()>0)
					newVertex.Normal = mNormals[i];
				if(mUVs.size()>0)
					newVertex.TexCoords = mUVs[i];
				if (mTangents.size()>0)
					newVertex.Tangent = mTangents[i];
				if(mBitangents.size()>0)
					newVertex.Bitangent = mBitangents[i];
				if (mBoneIDs.size() > 0) {
					newVertex.m_BoneIDs = mBoneIDs[i];
					newVertex.m_Weights = mWeights[i];
				}
				vertices.push_back(newVertex);
			}

			// create buffers/arrays
			glGenVertexArrays(1, &mVAO);
			glGenBuffers(1, &mVBO);
			if (mIndices.size()>0)
			{
				glGenBuffers(1, &mEBO);
			}
			

			glBindVertexArray(mVAO);
			// load data into vertex buffers
			glBindBuffer(GL_ARRAY_BUFFER, mVBO);
			// A great thing about structs is that their memory layout is sequential for all its items.
			// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
			// again translates to 3/2 floats which translates to a byte array.
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

			if (mIndices.size() > 0) glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
			if (mIndices.size() > 0) glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(unsigned int), &mIndices[0], GL_STATIC_DRAW);

			// set the vertex attribute pointers
			// vertex Positions
			if (mPositions.size())
			{
				glEnableVertexAttribArray(0);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
			}
			
			if (mNormals.size()>0)
			{
				// vertex normals
				glEnableVertexAttribArray(1);
				glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
			}
			
			if (mUVs.size()>0)
			{
				// vertex texture coords
				glEnableVertexAttribArray(2);
				glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
			}
			if (mTangents.size()>0)
			{// vertex tangent
				glEnableVertexAttribArray(3);
				glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
			}
			
			if (mBitangents.size()>0)
			{
				glEnableVertexAttribArray(4);
				glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
			}
			if (mBoneIDs.size()>0)
			{
				// ids
				glEnableVertexAttribArray(5);
				glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));

				// weights
				glEnableVertexAttribArray(6);
				glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));
			}
			
			glBindVertexArray(0);
		}

		else
		{
			// setup vao, vbo and ebo
			glGenVertexArrays(1, &mVAO);
			glGenBuffers(1, &mVBO);
			glGenBuffers(1, &mEBO);

			glBindVertexArray(mVAO);
			glBindBuffer(GL_ARRAY_BUFFER, mVBO);
			unsigned int totalOffset = mPositions.size() * 3 + mNormals.size() *3 + mUVs.size() * 2 + mTangents.size() * 3 + mBitangents.size() * 3 + mBoneIDs.size() * MAX_BONE_INFLUENCE + mWeights.size()* MAX_BONE_INFLUENCE;
			totalOffset *= sizeof(float); // size: int = float 

			glBufferData(GL_ARRAY_BUFFER, totalOffset, NULL, GL_STATIC_DRAW);
			unsigned int offset = 0;
			if (mPositions.size()>0)
			{
				unsigned int vectorsize = mPositions.size() * 3 * sizeof(float);
				glBufferSubData(GL_ARRAY_BUFFER, offset, vectorsize, &mPositions[0]);
				offset += vectorsize;
			}
			if (mNormals.size()>0)
			{
				unsigned int vectorsize = mNormals.size() * 3 * sizeof(float);
				glBufferSubData(GL_ARRAY_BUFFER, offset, vectorsize, &mNormals[0]);
				offset += vectorsize;
			}
			if (mUVs.size() > 0)
			{
				unsigned int vectorsize = mUVs.size() * 2 * sizeof(float);
				glBufferSubData(GL_ARRAY_BUFFER, offset, vectorsize, &mUVs[0]);
				offset += vectorsize;
			}
			if (mTangents.size() > 0)
			{
				unsigned int vectorsize = mTangents.size() * 3 * sizeof(float);
				glBufferSubData(GL_ARRAY_BUFFER, offset, vectorsize, &mTangents[0]);
				offset += vectorsize;
			}
			if (mBitangents.size() > 0)
			{
				unsigned int vectorsize = mBitangents.size() * 3 * sizeof(float);
				glBufferSubData(GL_ARRAY_BUFFER, offset, vectorsize, &mBitangents[0]);
				offset += vectorsize;
			}
			if (mBoneIDs.size() > 0)
			{
				unsigned int vectorsize = mBoneIDs.size() * MAX_BONE_INFLUENCE * sizeof(int);
				glBufferSubData(GL_ARRAY_BUFFER, offset, vectorsize, &mBoneIDs[0]);
				offset += vectorsize;
			}
			if (mWeights.size() > 0)
			{
				unsigned int vectorsize = mWeights.size() * MAX_BONE_INFLUENCE * sizeof(float);
				glBufferSubData(GL_ARRAY_BUFFER, offset, vectorsize, &mWeights[0]);
				offset += vectorsize;
			}

			// bind ebo if necessary
			if (mIndices.size() > 0)
			{
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(unsigned int), &mIndices[0], GL_STATIC_DRAW);
			}
			offset = 0;

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
			if (mBoneIDs.size() > 0)
			{
				glEnableVertexAttribArray(5);
				glVertexAttribIPointer(5, 4, GL_INT, 0, (void*)offset);
				offset += mBoneIDs.size() * 4 * sizeof(int);
			}
			if (mWeights.size() > 0)
			{
				glEnableVertexAttribArray(6);
				glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 0, (void*)offset);
				offset += mWeights.size() * 4 * sizeof(float);
			}
			glBindVertexArray(0);
		}
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
	void Mesh::SetBoneInfluence(std::vector<glm::ivec4>& BoneID, std::vector<glm::vec4>& BoneWeight) {
		mBoneIDs = BoneID;
		mWeights = BoneWeight;
	}
}