#include "ModelComponent.hpp"

#include "ArrayObject.hpp"
#include "BufferObject.hpp"
#include "ShaderProgram.hpp"
#include "TransformComponent.hpp"
#include "Entity.hpp"
#include "Window.hpp"
#include "Resources.hpp"
#include "ModelData.hpp"
#include "Texture.hpp"


void ModelComponent::OnAttach(std::weak_ptr<Entity> entity) {
    Component::OnAttach(entity);
    mEntity = entity;

    mRenderListenerId = Events::GetSingleton()->AddListener("Render", [this](void *) { OnRender(); });
    mShadowListenerId = Events::GetSingleton()->AddListener("RenderShadows", [this](void *) { OnShadow(); });
}

void ModelComponent::OnDetach() {
    Component::OnDetach();
    Events::GetSingleton()->RemoveListener("Render", mRenderListenerId);
    Events::GetSingleton()->RemoveListener("RenderShadows", mShadowListenerId);
}

std::string ModelComponent::GetType() const {
    return "ModelComponent";
}

void ModelComponent::OnRender() {
    auto transform = (TransformComponent *) mEntity.lock()->GetComponent("TransformComponent").get();

    ((Texture *) mTexture.get())->Bind(0);
    ShaderProgram::Get("Main")->Texture(ShaderProgram::TextureUniformName, 0);
    ((Texture *) mNormalMap.get())->Bind(1);
    ShaderProgram::Get("Main")->Texture(ShaderProgram::NormalMapUniformName, 1);
    ((Texture *) mReflectionMap.get())->Bind(2);
    ShaderProgram::Get("Main")->Texture(ShaderProgram::ReflectionMapUniformName, 2);

    ShaderProgram::Get("Main")->Uniform(ShaderProgram::EmissionUniformName, mEmission);

    mArrayObject->Bind();
    ShaderProgram::Get("Main")->Uniform(ShaderProgram::ModelUniformName, transform->GetMatrix());
    Window::GetSingleton()->DrawElements(mIndexNumber);
}

void ModelComponent::OnShadow() {
    auto transform = (TransformComponent *) mEntity.lock()->GetComponent("TransformComponent").get();

    mShadowArrayObject->Bind();
    ShaderProgram::Get("Light")->Uniform(ShaderProgram::ModelUniformName, transform->GetMatrix());
    Window::GetSingleton()->DrawElements(mIndexNumber);
}

ModelComponent::ModelComponent(std::string modelName, std::string textureName, std::string normalMapName,
                               std::string reflectionMapName, float emission) {
    mEmission = emission;

    mArrayObject = std::make_shared<ArrayObject>();
    mArrayObject->Bind();

    auto resource = Resources::GetSingleton()->GetResource(modelName);
    auto model = (ModelData *) resource.get();

    mVertices = std::make_shared<BufferObject>(BufferObject::Type::Vertex);
    mVertices->CopyData(sizeof(float) * model->GetVertices().size(), (void *) model->GetVertices().data());

    if (model->GetColors().size() > 0) {
        mColors = std::make_shared<BufferObject>(BufferObject::Type::Vertex);
        mColors->CopyData(sizeof(float) * model->GetColors().size(), (void *) model->GetColors().data());
    }

    mNormals = std::make_shared<BufferObject>(BufferObject::Type::Vertex);
    mNormals->CopyData(sizeof(float) * model->GetNormals().size(), (void *) model->GetNormals().data());

    if (model->GetTexCoords().size() > 0) {
        mTexCoords = std::make_shared<BufferObject>(BufferObject::Type::Vertex);
        mTexCoords->CopyData(sizeof(float) * model->GetTexCoords().size(), (void *) model->GetTexCoords().data());
    }

    mTangents = std::make_shared<BufferObject>(BufferObject::Type::Vertex);
    mTangents->CopyData(sizeof(float) * model->GetTangents().size(), (void *) model->GetTangents().data());

    mIndices = std::make_shared<BufferObject>(BufferObject::Type::Index);
    mIndices->CopyData(sizeof(unsigned int) * model->GetIndices().size(), (void *) model->GetIndices().data());
    mIndexNumber = (unsigned int) model->GetIndices().size();

    mVertices->Bind();
    ShaderProgram::Get("Main")->Attribute(ShaderProgram::PositionAttributeName, 3);

    if (mColors) {
        mColors->Bind();
        ShaderProgram::Get("Main")->Attribute(ShaderProgram::ColorAttributeName, 3);
    }

    mNormals->Bind();
    ShaderProgram::Get("Main")->Attribute(ShaderProgram::NormalAttributeName, 3);

    if (mTexCoords) {
        mTexCoords->Bind();
        ShaderProgram::Get("Main")->Attribute(ShaderProgram::TexCoordAttributeName, 2);
    }

    mTangents->Bind();
    ShaderProgram::Get("Main")->Attribute(ShaderProgram::TangentAttributeName, 3);

    mTexture = Resources::GetSingleton()->GetResource(textureName);
    mNormalMap = Resources::GetSingleton()->GetResource(normalMapName);
    mReflectionMap = Resources::GetSingleton()->GetResource(reflectionMapName);

    mShadowArrayObject = std::make_shared<ArrayObject>();
    mShadowArrayObject->Bind();

    mVertices->Bind();
    ShaderProgram::Get("Light")->Attribute(ShaderProgram::PositionAttributeName, 3);
    mIndices->Bind();
}
