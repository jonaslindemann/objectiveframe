#pragma once

#include <memory>
#include <string>
#include <vector>

#include <imgui.h>

namespace ofui {

enum class NotificationLevel { Info, Warning, Error };

class NotificationOverlay {
private:
    struct Notification {
        std::string message;
        NotificationLevel level;
        float elapsed;
        float duration;
    };

    std::vector<Notification> m_notifications;

    ImVec4 colorFor(NotificationLevel level) const;

public:
    NotificationOverlay() = default;

    static std::shared_ptr<NotificationOverlay> create();

    void add(const std::string &message, NotificationLevel level = NotificationLevel::Info,
             float duration = 4.0f);
    void draw(float deltaTime);
};

typedef std::shared_ptr<NotificationOverlay> NotificationOverlayPtr;

} // namespace ofui
