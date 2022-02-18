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

    void AssimpParser::ParseScene(Scope<Scene> &scene, const std::string &path)
    {
        m_SceneRootDir = g_FileSystem->ParentPath(path);

        Assimp::Importer importer;

        m_AiScene = importer.ReadFile(path, importerFlags);

        if(!m_AiScene || m_AiScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !m_AiScene->mRootNode)
        {
            scene = nullptr; return;
        }

        ProcessMaterials(scene.get());
        ProcessTextures(scene.get());
        ProcessLights(scene.get());

        ProcessNode(m_AiScene->mRootNode, scene.get(), nullptr);

        // If the scene doesn't contain any light, add a default light
        if (scene.get()->m_Lights.empty())
        {
            DefaultLight(scene.get());
        }
    }

    void AssimpParser::ProcessMaterials(Scene *scene)
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

                sceneMat.m_Parameter.DiffuseColor = glm::vec4(diffuseFactor.r, diffuseFactor.g, diffuseFactor.b, 1.0f);
                sceneMat.m_Parameter.SpecularColor = glm::vec4(specularFactor.r, specularFactor.g, specularFactor.b, 1.0f);

                sceneMat.m_Parameter.Shininess = shininess;

                sceneMat.m_DiffuseTextureName = diffTexName.C_Str();
                sceneMat.m_SpecularTextureName = specularTexName.C_Str();

                sceneMat.m_NormalTextureName = normalTexName.C_Str();

            }
        }
    }

    void AssimpParser::ProcessTextures(Scene *scene)
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

    void AssimpParser::ProcessLights(Scene *scene)
    {
        for (size_t i = 0; i < m_AiScene->mNumLights; i++)
        {
            m_LightIndexDict[m_AiScene->mLights[i]->mName.C_Str()] = i;
            aiLight &ref = *m_AiScene->mLights[i];

            Light light;

            light.m_Node = nullptr;
            light.m_IsCastShadow = true;

            switch (ref.mType)
            {
                case aiLightSource_DIRECTIONAL: { light.m_Parameter.Type = LightType::Directional; break; }

                case aiLightSource_POINT: { light.m_Parameter.Type = LightType::Point; break; }

                case aiLightSource_SPOT: { light.m_Parameter.Type = LightType::Spot; break; }

                case aiLightSource_AMBIENT: { light.m_Parameter.Type = LightType::Ambient; break; }

                case aiLightSource_AREA: { light.m_Parameter.Type = LightType::Area; break; }

                default: { light.m_Parameter.Type = LightType::Undefined; break; }
            }

            light.m_Parameter.Position = {ref.mPosition.x, ref.mPosition.y, ref.mPosition.z};
            light.m_Parameter.Direction = {ref.mDirection.x, ref.mDirection.y, ref.mDirection.z};
            light.m_Parameter.Up = {ref.mUp.x, ref.mUp.y, ref.mUp.z};

            light.m_Parameter.AttenConstant = ref.mAttenuationConstant;
            light.m_Parameter.AttenLinear = ref.mAttenuationLinear;
            light.m_Parameter.AttenQuadratic = ref.mAttenuationQuadratic;

            light.m_Parameter.DiffuseColor = {ref.mColorDiffuse.r, ref.mColorDiffuse.g, ref.mColorDiffuse.b};
            light.m_Parameter.SpecularColor = {ref.mColorSpecular.r, ref.mColorSpecular.g, ref.mColorSpecular.b};
            light.m_Parameter.AmbientColor = {ref.mColorAmbient.r, ref.mColorAmbient.g, ref.mColorAmbient.b};

            light.m_Parameter.AngleInnerCone = ref.mAngleInnerCone;
            light.m_Parameter.AngleOuterCone = ref.mAngleOuterCone;

            light.m_Parameter.Size = {ref.mSize.x, ref.mSize.y};

            scene->m_Lights.push_back(light);
        }
    }

    void AssimpParser::ProcessNode(aiNode *pNode, Scene *scene, SceneNode *parent)
    {
        if (parent)
            parent->m_Children.emplace_back(SceneNode());
        else
            scene->m_SceneNodes.emplace_back(SceneNode());

        // Pointer to the newly added scene node
        SceneNode *node = parent ? &parent->m_Children.back() : &scene->m_SceneNodes.back();

        node->m_Name = pNode->mName.C_Str();
        node->m_Parent = parent;

        // Note: aiMatrix4x4 is row major. glm::mat4 is column major.
        aiMatrix4x4 mat = pNode->mTransformation;
        node->m_Transform.SetMatrix4x4(glm::transpose(glm::make_mat4x4(&mat[0][0])));

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

        if (m_LightIndexDict.find(node->m_Name) != m_LightIndexDict.end())
        {
            // Link the light and the node
            Light &light = scene->m_Lights[m_LightIndexDict[node->m_Name]];

            light.m_Node = node;
            node->m_Light = &light;
        }

        // recurse to process children
        for (size_t i = 0; i < pNode->mNumChildren; i++)
        {
            ProcessNode(pNode->mChildren[i], scene, node);
        }
    }

    void AssimpParser::DefaultLight(Scene *scene)
    {
        SceneNode &root = scene->m_SceneNodes.front();

        // Add a light
        Light light;

        light.m_Node = nullptr;
        light.m_IsCastShadow = true;

        light.m_Parameter.Type = Directional;

        light.m_Parameter.Position = {0, 0, 0};
        light.m_Parameter.Direction = {0, -1, 0};
        light.m_Parameter.Up = {0, 0, 0};

        light.m_Parameter.DiffuseColor = {1, 1, 1};
        light.m_Parameter.SpecularColor = {1, 1, 1};
        light.m_Parameter.AmbientColor = {1, 1, 1};

        scene->m_Lights.push_back(light);

        // Add a light node
        SceneNode &node = root.m_Children.emplace_back(SceneNode());

        node.m_Name = "_default_light";
        node.m_Parent = nullptr;
        node.m_Transform.SetMatrix4x4(glm::mat4(1.0f));

        // bookkeeping
        m_LightIndexDict[node.m_Name.c_str()] = 0;
    }

}