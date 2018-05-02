#pragma once

#include <GLFW/glfw3.h>
#include <string>

#include "Event.h"

namespace lpe
{
  namespace window
  {
    class WindowResizeEventArgs : public event::EventArgs
    {
    private:
      uint32_t oldWidth;
      uint32_t oldHeight;
      uint32_t newWidth;
      uint32_t newHeight;
    public:

      WindowResizeEventArgs(uint32_t oldWidth,
                            uint32_t oldHeight,
                            uint32_t newWidth,
                            uint32_t newHeight)
        : oldWidth(oldWidth),
          oldHeight(oldHeight),
          newWidth(newWidth),
          newHeight(newHeight)
      {
      }


      uint32_t GetOldWidth() const
      {
        return oldWidth;
      }

      uint32_t GetOldHeight() const
      {
        return oldHeight;
      }

      uint32_t GetNewWidth() const
      {
        return newWidth;
      }

      uint32_t GetNewHeight() const
      {
        return newHeight;
      }
    };

    enum class Key
    {
      Unknown = GLFW_KEY_UNKNOWN,
      Space = GLFW_KEY_SPACE,
      Apostrophe = GLFW_KEY_APOSTROPHE,
      Comma = GLFW_KEY_COMMA,
      Minus = GLFW_KEY_MINUS,
      Period = GLFW_KEY_PERIOD,
      Slash = GLFW_KEY_SLASH,
      Number_0 = GLFW_KEY_0,
      Number_1 = GLFW_KEY_1,
      Number_2 = GLFW_KEY_2,
      Number_3 = GLFW_KEY_3,
      Number_4 = GLFW_KEY_4,
      Number_5 = GLFW_KEY_5,
      Number_6 = GLFW_KEY_6,
      Number_7 = GLFW_KEY_7,
      Number_8 = GLFW_KEY_8,
      Number_9 = GLFW_KEY_9,
      Semicolon = GLFW_KEY_SEMICOLON,
      Equal = GLFW_KEY_EQUAL,
      A = GLFW_KEY_A,
      B = GLFW_KEY_B,
      C = GLFW_KEY_C,
      D = GLFW_KEY_D,
      E = GLFW_KEY_E,
      F = GLFW_KEY_F,
      G = GLFW_KEY_G,
      H = GLFW_KEY_H,
      I = GLFW_KEY_I,
      J = GLFW_KEY_J,
      K = GLFW_KEY_K,
      L = GLFW_KEY_L,
      M = GLFW_KEY_M,
      N = GLFW_KEY_N,
      O = GLFW_KEY_O,
      P = GLFW_KEY_P,
      Q = GLFW_KEY_Q,
      R = GLFW_KEY_R,
      S = GLFW_KEY_S,
      T = GLFW_KEY_T,
      U = GLFW_KEY_U,
      V = GLFW_KEY_V,
      W = GLFW_KEY_W,
      X = GLFW_KEY_X,
      Y = GLFW_KEY_Y,
      Z = GLFW_KEY_Z,
      LeftBracket = GLFW_KEY_LEFT_BRACKET,
      Backslash = GLFW_KEY_BACKSLASH,
      RightBracket = GLFW_KEY_RIGHT_BRACKET,
      GraveAccent = GLFW_KEY_GRAVE_ACCENT,
      World_1 = GLFW_KEY_WORLD_1,
      World_2 = GLFW_KEY_WORLD_2,
      Escape = GLFW_KEY_ESCAPE,
      Enter = GLFW_KEY_ENTER,
      Tab = GLFW_KEY_TAB,
      Backspace = GLFW_KEY_BACKSPACE,
      Insert = GLFW_KEY_INSERT,
      Delete = GLFW_KEY_DELETE,
      Right = GLFW_KEY_RIGHT,
      Left = GLFW_KEY_LEFT,
      Down = GLFW_KEY_DOWN,
      Up = GLFW_KEY_UP,
      PageUp = GLFW_KEY_PAGE_UP,
      PageDown = GLFW_KEY_PAGE_DOWN,
      Home = GLFW_KEY_HOME,
      End = GLFW_KEY_END,
      CapsLock = GLFW_KEY_CAPS_LOCK,
      ScrollLock = GLFW_KEY_SCROLL_LOCK,
      NumLock = GLFW_KEY_NUM_LOCK,
      PrintScreen = GLFW_KEY_PRINT_SCREEN,
      Pause = GLFW_KEY_PAUSE,
      F1 = GLFW_KEY_F1,
      F2 = GLFW_KEY_F2,
      F3 = GLFW_KEY_F3,
      F4 = GLFW_KEY_F4,
      F5 = GLFW_KEY_F5,
      F6 = GLFW_KEY_F6,
      F7 = GLFW_KEY_F7,
      F8 = GLFW_KEY_F8,
      F9 = GLFW_KEY_F9,
      F10 = GLFW_KEY_F10,
      F11 = GLFW_KEY_F11,
      F12 = GLFW_KEY_F12,
      F13 = GLFW_KEY_F13,
      F14 = GLFW_KEY_F14,
      F15 = GLFW_KEY_F15,
      F16 = GLFW_KEY_F16,
      F17 = GLFW_KEY_F17,
      F18 = GLFW_KEY_F18,
      F19 = GLFW_KEY_F19,
      F20 = GLFW_KEY_F20,
      F21 = GLFW_KEY_F21,
      F22 = GLFW_KEY_F22,
      F23 = GLFW_KEY_F23,
      F24 = GLFW_KEY_F24,
      F25 = GLFW_KEY_F25,
      KeyPad_0 = GLFW_KEY_KP_0,
      KeyPad_1 = GLFW_KEY_KP_1,
      KeyPad_2 = GLFW_KEY_KP_2,
      KeyPad_3 = GLFW_KEY_KP_3,
      KeyPad_4 = GLFW_KEY_KP_4,
      KeyPad_5 = GLFW_KEY_KP_5,
      KeyPad_6 = GLFW_KEY_KP_6,
      KeyPad_7 = GLFW_KEY_KP_7,
      KeyPad_8 = GLFW_KEY_KP_8,
      KeyPad_9 = GLFW_KEY_KP_9,
      KeyPadDecimal = GLFW_KEY_KP_DECIMAL,
      KeyPadDivide = GLFW_KEY_KP_DIVIDE,
      KeyPadMultiply = GLFW_KEY_KP_MULTIPLY,
      KeyPadSubtract = GLFW_KEY_KP_SUBTRACT,
      KeyPadAdd = GLFW_KEY_KP_ADD,
      KeyPadEnter = GLFW_KEY_KP_ENTER,
      KeyPadEqual = GLFW_KEY_KP_EQUAL,
      LeftShift = GLFW_KEY_LEFT_SHIFT,
      LeftControl = GLFW_KEY_LEFT_CONTROL,
      LeftAlt = GLFW_KEY_LEFT_ALT,
      LeftSuper = GLFW_KEY_LEFT_SUPER,
      RightShift = GLFW_KEY_RIGHT_SHIFT,
      RightControl = GLFW_KEY_RIGHT_CONTROL,
      RightAlt = GLFW_KEY_RIGHT_ALT,
      RightSuper = GLFW_KEY_RIGHT_SUPER,
      Menu = GLFW_KEY_MENU,
      Last = GLFW_KEY_LAST
    };

    enum class State
    {
      Pressed = GLFW_PRESS,
      Released = GLFW_RELEASE,
      Held = GLFW_REPEAT
    };

    enum class KeyModifiers
    {
      Shift = GLFW_MOD_SHIFT,
      Control = GLFW_MOD_CONTROL,
      Alt = GLFW_MOD_ALT,
      Super = GLFW_MOD_SUPER
    };

    enum class MouseButton
    {
      Button_1 = GLFW_MOUSE_BUTTON_1,
      Button_2 = GLFW_MOUSE_BUTTON_2,
      Button_3 = GLFW_MOUSE_BUTTON_3,
      Button_4 = GLFW_MOUSE_BUTTON_4,
      Button_5 = GLFW_MOUSE_BUTTON_5,
      Button_6 = GLFW_MOUSE_BUTTON_6,
      Button_7 = GLFW_MOUSE_BUTTON_7,
      Button_8 = GLFW_MOUSE_BUTTON_8,
      Last = GLFW_MOUSE_BUTTON_LAST,
      Left = GLFW_MOUSE_BUTTON_LEFT,
      Right = GLFW_MOUSE_BUTTON_RIGHT,
      Middle = GLFW_MOUSE_BUTTON_MIDDLE
    };

    class WindowKeyPressedEventArgs : public event::EventArgs
    {
    private:
      Key key;
      State state;
      int32_t scancode;
      KeyModifiers modifiers;
    public:

      WindowKeyPressedEventArgs(Key key,
                                State state,
                                int32_t scancode,
                                KeyModifiers modifiers)
        : key(key),
          state(state),
          scancode(scancode),
          modifiers(modifiers)
      {
      }


      Key GetKey() const
      {
        return key;
      }

      State GetState() const
      {
        return state;
      }

      int32_t GetScancode() const
      {
        return scancode;
      }

      KeyModifiers GetModifiers() const
      {
        return modifiers;
      }
    };

    class WindowMouseMovedEventArgs : public event::EventArgs
    {
    private:
      double oldX;
      double oldY;
      double newX;
      double newY;
    public:

      WindowMouseMovedEventArgs(double oldX,
                                double oldY,
                                double newX,
                                double newY)
        : oldX(oldX),
          oldY(oldY),
          newX(newX),
          newY(newY)
      {
      }


      double GetOldX() const
      {
        return oldX;
      }

      double GetOldY() const
      {
        return oldY;
      }

      double GetNewX() const
      {
        return newX;
      }

      double GetNewY() const
      {
        return newY;
      }
    };

    class WindowMouseClickedEventArgs : public event::EventArgs
    {
    private:
      MouseButton button;
      State state;
      KeyModifiers modifiers;
    public:

      WindowMouseClickedEventArgs(MouseButton button,
                                  State state,
                                  KeyModifiers modifiers)
        : button(button),
          state(state),
          modifiers(modifiers)
      {
      }


      MouseButton GetButton() const
      {
        return button;
      }

      State GetState() const
      {
        return state;
      }

      KeyModifiers GetModifiers() const
      {
        return modifiers;
      }
    };

    class Window
    {
    private:
      GLFWwindow* window = nullptr;
      uint32_t width;
      uint32_t height;
      double x;
      double y;
      std::string title;

      static void GLFW_KeyInputCallback(GLFWwindow* window,
                                        int key,
                                        int scancode,
                                        int action,
                                        int mods);
      static void GLFW_MouseInputCallback(GLFWwindow* window,
                                          int button,
                                          int action,
                                          int mods);
      static void GLFW_MouseMoveCallback(GLFWwindow* window,
                                         double xpos,
                                         double ypos);
      static void GLFW_WindowResizeCallback(GLFWwindow* window,
                                            int width,
                                            int height);

    public:
      Window() = delete;
      Window(const Window& other) = delete;
      Window(Window&& other) = delete;
      Window& operator=(const Window& other) = delete;
      Window& operator=(Window&& other) = delete;
      Window(const std::string&&,
             uint32_t width,
             uint32_t height,
             bool resizeable);
      ~Window() = default;

      void Render();
      bool IsOpen() const;

      event::Event<Window, WindowResizeEventArgs> Resized;
      event::Event<Window, WindowKeyPressedEventArgs> KeyPressed;
      event::Event<Window, WindowMouseMovedEventArgs> MouseMoved;
      event::Event<Window, WindowMouseClickedEventArgs> MousePressed;
    };
  }
}
