#include "SceneNode.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include "Entity.h"
namespace CoreTech {
void SceneNode::addChild(std::shared_ptr<SceneNode> child){
    if(child){
        child->parent = shared_from_this(); // this requires shared_ptr ownership at creation site
        children.push_back(child);
    }
}
glm::mat4 SceneNode::worldMatrix() const {
    glm::mat4 m = glm::mat4(1.0f);
    m = glm::translate(m, localPosition);
    glm::quat q = glm::quat(glm::radians(localRotation));
    m = m * glm::mat4_cast(q);
    m = glm::scale(m, localScale);
    if(auto p = parent.lock()){
        return p->worldMatrix() * m;
    }
    return m;
}
}