#pragma once

#include <string>
#include <memory>

#include "Singleton.hpp"


/**
 * GLFW window wrapper.
 */


class GLFWwindow;


class Window : public Singleton<Window> {
    friend class Input;

public:

    static std::shared_ptr<Window> Create(unsigned int width, unsigned int height, std::string title,
                                          bool fullscreen = false);

    static std::shared_ptr<Window> CreateHidden();
    static void Destroy();

    void UpdateViewport();
    void SetViewport(int width, int height);
    void CullFront();
    void CullBack();
    void Clear();
    void SwapBuffers();
    void PollEvents();
    void Close();

    void DrawElements(unsigned int count);

    void GetSize(int &width, int &height) const;
    bool IsClosed() const;

    ~Window();


private:

    static void PreInit();
    static void PostInit(GLFWwindow* window);

    Window() {};


private:

    GLFWwindow *mGlfwWindow = 0;

};


