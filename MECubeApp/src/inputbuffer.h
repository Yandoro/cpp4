#pragma once
#include <XInput.h>
#include <unordered_map>
#include <string>
#include <minwinbase.h>

class InputManager {
public:
    // Konfiguriere eine Aktion mit einem XInput-Button
    void configureAction(const std::string& action, WORD button) {
        actions[action] = button;
    }

    // Prüfe, ob die Aktion ausgeführt wird
    bool isActionPressed(const std::string& action) {
        if (actions.find(action) != actions.end()) {
            XINPUT_STATE state;
            ZeroMemory(&state, sizeof(XINPUT_STATE));
            XInputGetState(0, &state);

            return (state.Gamepad.wButtons & actions[action]) != 0;
        }
        return false;
    }

private:
    std::unordered_map<std::string, WORD> actions;
};
