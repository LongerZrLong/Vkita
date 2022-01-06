#include "AssimpParser.h"

#include "Core/Image.h"
#include "Core/FileSystem.h"

#include "Rendering/Texture2D.h"

namespace VKT {

    static const auto importerFlags =
        aiProcess_FlipUVs |
        aiProcess_CalcTangentSpace |
        aiProcess_GenSmoothNormals |
        aiProcess_JoinIdenticalVertices |
        aiProcess_OptimizeMeshes |           // reduce the number of meshes
        aiProcess_ImproveCacheLocality |     // re-order triangles for better vertex cache locality.
        aiProcess_RemoveRedundantMaterials | // remove redundant/unreferenced materials.
        aiProcess_LimitBoneWeights |
        aiProcess_SplitLargeMeshes |
        aiProcess_Triangulate |
        aiProcess_GenUVCoords |
        aiProcess_SortByPType |              // splits meshes with more than one primitive type in homogeneous sub-meshes.
        aiProcess_FindDegenerates |          // convert degenerate primitives to proper lines or points.
        aiProcess_FindInvalidData |
        aiProcess_FindInstances |
        aiProcess_ValidateDataStructure |
        aiProcess_Debone;

    Ref<Scene> AssimpParser::ParseScene(const std::string &path)
    {
        m_SceneRootDir = g_FileSystem->ParentPath(path);

        Assimp::Importer importer;

        m_AiScene = importer.ReadFile(path, importerFlags);

        if(!m_AiScene || m_AiScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !m_AiScene->mRootNode)
            return nullptr;

        auto scene = CreateRef<Scene>();

        ProcessMaterials(scene);
        ProcessTextures(scene);
        ProcessNode(m_AiScene->mRootNode, scene, nullptr);

        return scene;
    }

    void AssimpParser::ProcessMaterials(Ref<Scene> &scene)
    {
        scene->m_Materials.resize(m_AiScene->mNumMaterials);
        for (size_t i = 0; i < m_AiScene->mNumMaterials; i++)
        {
            aiMaterial *material = m_AiScene->mMaterials[i];
            if (material)
            {
                // populate the material slot
                aiString name = material->GetName();

                aiColor3D diffuseFactor;
                material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseFactor);

                aiColor3D specularFactor;
                material->Get(AI_MATKEY_COLOR_SPECULAR, specularFactor);

                float shininess;
                material->Get(AI_MATKEY_SHININESS, shininess);

                // For simplicity, just load the first texture of each kind
                aiString diffTexName;
                if (material->GetTextureCount(aiTextureType_DIFFUSE) >= 1)
                {
                    material->GetTexture(aiTextureType_DIFFUSE, 0, &diffTexName);
                }

                aiString specularTexName;
                if (material->GetTextureCount(aiTextureType_SPECULAR) >= 1)
                {
                    material->GetTexture(aiTextureType_SPECULAR, 0, &specularTexName);
                }

                aiString normalTexName;
                if (material->GetTextureCount(aiTextureType_NORMALS) >= 1)
                {
                    material->GetTexture(aiTextureType_NORMALS, 0, &normalTexName);
                }

                // Populate Material struct
                Material &sceneMat = scene->m_Materials[i];
                sceneMat.m_Name = name.C_Str();

                sceneMat.m_DiffuseColor = glm::vec4(diffuseFactor.r, diffuseFactor.g, diffuseFactor.b, 1.0f);
                sceneMat.m_SpecularColor = glm::vec4(specularFactor.r, specularFactor.g, specularFactor.b, 1.0f);

                sceneMat.m_Shininess = shininess;

                sceneMat.m_DiffuseTextureName = diffTexName.C_Str();
                sceneMat.m_SpecularTextureName = specularTexName.C_Str();

                sceneMat.m_NormalTextureName = normalTexName.C_Str();

            }
        }
    }

    void AssimpParser::ProcessTextures(Ref<Scene> &scene)
    {
        auto &textures = scene->m_Textures;
        for (const Material &mat : scene->m_Materials)
        {
            auto found = textures.find(mat.m_DiffuseTextureName);
            if (found == textures.end() && !mat.m_DiffuseTextureName.empty())
            {
                textures[mat.m_DiffuseTextureName] = CreateScope<Image>(g_FileSystem->Append(m_SceneRootDir, mat.m_DiffuseTextureName));
            }

            found = textures.find(mat.m_SpecularTextureName);
            if (found == textures.end() && !mat.m_SpecularTextureName.empty())
            {
                textures[mat.m_SpecularTextureName] = CreateScope<Image>(g_FileSystem->Append(m_SceneRootDir, mat.m_SpecularTextureName));
            }

            found = textures.find(mat.m_NormalTextureName);
            if (found == textures.end() && !mat.m_NormalTextureName.empty())
            {
                textures[mat.m_NormalTextureName] = CreateScope<Image>(g_FileSystem->Append(m_SceneRootDir, mat.m_NormalTextureName));
            }
        }

        // Add a default white texture
        textures["_default"] = CreateScope<Image>(1, 1);
        textures["_default"]->m_Data = reinterpret_cast<uint8_t*>(new uint32_t(0xffffffff));
    }

    void AssimpParser::ProcessNode(aiNode *pNode, Ref<Scene> &scene, SceneNode *parent)
    {
        if (parent)
            parent->m_Children.emplace_back(SceneNode());
        else
            scene->m_SceneNodes.emplace_back(SceneNode());

        SceneNode *node = parent ? &parent->m_Children[parent->m_Children.size() - 1] : &scene->m_SceneNodes[scene->m_SceneNodes.size() - 1];

        node->m_Name = pNode->mName.C_Str();
        node->m_Parent = parent;

        // Note: aiMatrix4x4 is row major. glm::mat4 is column major.
        aiMatrix4x4 mat = pNode->mTransformation;
        node->m_Transform.SetMatrix4x4(glm::transpose(glm::make_mat4x4(&mat[0][0])));

        for (size_t i = 0; i < pNode->mNumChildren; i++)
        {
            ProcessNode(pNode->mChildren[i], scene, node);
        }

        for (size_t j = 0; j < pNode->mNumMeshes; j++)
        {
            auto firstIndex = static_cast<uint32_t>(scene->m_Indices.size());
            auto vertexStart = static_cast<uint32_t>(scene->m_Vertices.size());
            uint32_t indexCount = 0;

            uint32_t meshIdx = pNode->mMeshes[j];

            aiMesh *pMesh = m_AiScene->mMeshes[meshIdx];

            // retrieve vertices
            for (uint32_t i = 0; i < pMesh->mNumVertices; ++i)
            {
                aiVector3D position		= pMesh->mVertices[i];
                aiVector3D normal		= pMesh->mNormals ? pMesh->mNormals[i] : aiVector3D(0.0f, 0.0f, 0.0f);
                aiVector3D tangent		= pMesh->mTangents ? pMesh->mTangents[i] : aiVector3D(0.0f, 0.0f, 0.0f);
                aiVector3D bitangent	= pMesh->mBitangents ? pMesh->mBitangents[i] : aiVector3D(0.0f, 0.0f, 0.0f);
                aiVector3D texCoords	= pMesh->mTextureCoords[0] ? pMesh->mTextureCoords[0][i] : aiVector3D(0.0f, 0.0f, 0.0f);

                scene->m_Vertices.emplace_back
                    (
                        Rendering::Vertex
                            (
                                {
                                    {position.x,  position.y,  position.z},
                                    {normal.x,    normal.y,    normal.z},
                                    {tangent.x,   tangent.y,   tangent.z},
                                    {bitangent.x, bitangent.y, bitangent.z},
                                    {texCoords.x, texCoords.y}
                                }
                            )
                    );
            }

            // retrieve indices
            for (uint32_t faceID = 0; faceID < pMesh->mNumFaces; faceID++)
            {
                auto &face = pMesh->mFaces[faceID];

                for (size_t indexID = 0; indexID < face.mNumIndices; indexID++)
                    scene->m_Indices.push_back(face.mIndices[indexID] + vertexStart);

                indexCount += face.mNumIndices;
            }

            Primitive primitive{};
            primitive.FirstIndex = firstIndex;
            primitive.IndexCount = indexCount;
            primitive.MaterialIndex = pMesh->mMaterialIndex;

            node->m_Mesh.m_Primitives.emplace_back(primitive);
        }

    }

}