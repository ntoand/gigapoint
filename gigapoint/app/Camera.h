#ifndef CAMERA_H
#define CAMERA_H

// ref: https://learnopengl.com/#!Getting-started/Camera

#include "GLInclude.h"

// Std. Includes
#include <vector>

// GL Includes
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

// Defines several possible options for camera movement.
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default camera values
const float YAW        = -90.0f;
const float PITCH      =  0.0f;
const float SPEED      =  3.0f;
const float SENSITIVTY =  0.25f;
const float ZOOM       =  45.0f;
const float ANGLE      = 45.0f;
const float RATIO      =  4.0f/3.0f;
const float NEAR       = 0.9f;
const float FAR        = 100000.0f;

//
class Camera
{
    
public:
    // Camera Attributes
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;
    // Eular Angles
    float yaw;
    float pitch;
    // Camera options
    float movementSpeed;
    float mouseSensitivity;
    float zoom;
    // Projection
    float angle, ratio;
    float near, far;
    
    // Constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVTY), zoom(ZOOM), angle(ANGLE), ratio(RATIO), near(NEAR), far(FAR) {
        this->position = position;
        this->worldUp = up;
        this->yaw = yaw;
        this->pitch = pitch;
        this->updateCameraVectors();
    }
    
    // Constructor with scalar values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVTY), zoom(ZOOM), angle(ANGLE), ratio(RATIO), near(NEAR), far(FAR) {
        this->position = glm::vec3(posX, posY, posZ);
        this->worldUp = glm::vec3(upX, upY, upZ);
        this->yaw = yaw;
        this->pitch = pitch;
        this->updateCameraVectors();
    }
    
    // Returns the view matrix calculated using Eular Angles and the LookAt Matrix
    glm::mat4 getViewMatrix() {
        return glm::lookAt(this->position, this->position + this->front, this->up);
    }
    
    glm::mat4 getProjectionMatrix() {
        return glm::perspective<float>( angle, ratio, near, far );
    }

    glm::mat4 getNormalMatrix() {
        return glm::inverseTranspose(getViewMatrix());
    }
    
    // Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void processKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = this->movementSpeed * deltaTime;
        if (direction == FORWARD)
            this->position += this->front * velocity;
        if (direction == BACKWARD)
            this->position -= this->front * velocity;
        if (direction == LEFT)
            this->position -= this->right * velocity;
        if (direction == RIGHT)
            this->position += this->right * velocity;
    }
    
    // Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= this->mouseSensitivity;
        yoffset *= this->mouseSensitivity;
        
        this->yaw   += xoffset;
        this->pitch += yoffset;
        
        // Make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (this->pitch > 89.0f)
                this->pitch = 89.0f;
            if (this->pitch < -89.0f)
                this->pitch = -89.0f;
        }
        
        // Update Front, Right and Up Vectors using the updated Eular angles
        this->updateCameraVectors();
    }
    
    // Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void processMouseScroll(float yoffset)
    {
        if (this->zoom >= 1.0f && this->zoom <= 45.0f)
            this->zoom -= yoffset;
        if (this->zoom <= 1.0f)
            this->zoom = 1.0f;
        if (this->zoom >= 45.0f)
            this->zoom = 45.0f;
    }
    
private:
    // Calculates the front vector from the Camera's (updated) Eular Angles
    void updateCameraVectors()
    {
        // Calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
        front.y = sin(glm::radians(this->pitch));
        front.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
        
        this->front = glm::normalize(front);
        // Also re-calculate the Right and Up vector
        this->right = glm::normalize(glm::cross(this->front, this->worldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        this->up    = glm::normalize(glm::cross(this->right, this->front));
    }
    
};

#endif // CAMERA_H
