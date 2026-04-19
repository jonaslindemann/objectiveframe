#include <ofui/notification_overlay.h>

#include <imgui.h>
#include <imgui_internal.h>

#include <algorithm>

using namespace ofui;

std::shared_ptr<NotificationOverlay> NotificationOverlay::create()
{
    return std::make_shared<NotificationOverlay>();
}

void NotificationOverlay::add(const std::string &message, NotificationLevel level, float duration)
{
    m_notifications.push_back({message, level, 0.0f, duration});
}

ImVec4 NotificationOverlay::colorFor(NotificationLevel level) const
{
    switch (level)
    {
    case NotificationLevel::Warning: return ImVec4(0.95f, 0.75f, 0.10f, 1.0f);
    case NotificationLevel::Error:   return ImVec4(0.90f, 0.25f, 0.20f, 1.0f);
    default:                         return ImVec4(0.25f, 0.55f, 0.90f, 1.0f);
    }
}

void NotificationOverlay::draw(float deltaTime)
{
    // Advance timers and prune expired notifications
    for (auto &n : m_notifications)
        n.elapsed += deltaTime;

    m_notifications.erase(
        std::remove_if(m_notifications.begin(), m_notifications.end(),
                       [](const Notification &n) { return n.elapsed >= n.duration; }),
        m_notifications.end());

    if (m_notifications.empty())
        return;

    const float PAD = 16.0f;
    const ImGuiViewport *vp = ImGui::GetMainViewport();
    ImVec2 pos = ImVec2(vp->WorkPos.x + vp->WorkSize.x - PAD,
                        vp->WorkPos.y + vp->WorkSize.y - PAD);

    ImGui::SetNextWindowPos(pos, ImGuiCond_Always, ImVec2(1.0f, 1.0f));
    ImGui::SetNextWindowSizeConstraints(ImVec2(280, 0), ImVec2(400, vp->WorkSize.y * 0.6f));
    ImGui::SetNextWindowBgAlpha(0.0f);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking |
                             ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings |
                             ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav |
                             ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoInputs;

    ImGui::Begin("##notifications", nullptr, flags);

    for (int i = int(m_notifications.size()) - 1; i >= 0; --i)
    {
        const auto &n = m_notifications[i];

        float t = n.elapsed / n.duration;
        // Fade out during the last 25% of lifetime
        float alpha = (t > 0.75f) ? (1.0f - (t - 0.75f) / 0.25f) : 1.0f;
        alpha = std::max(0.0f, alpha);

        ImVec4 bg = colorFor(n.level);
        bg.w = alpha * 0.88f;

        ImDrawList *draw = ImGui::GetWindowDrawList();
        ImVec2 cursor = ImGui::GetCursorScreenPos();
        float width = ImGui::GetContentRegionAvail().x;

        // Measure text to size the background
        ImVec2 textSize = ImGui::CalcTextSize(n.message.c_str(), nullptr, false, width - 16.0f);
        float itemH = textSize.y + 20.0f; // padding top+bottom

        // Draw rounded rect background
        ImVec2 rMin = cursor;
        ImVec2 rMax = ImVec2(cursor.x + width, cursor.y + itemH);
        draw->AddRectFilled(rMin, rMax, ImGui::ColorConvertFloat4ToU32(bg), 6.0f);

        // Countdown bar at bottom of card
        float barAlpha = alpha * 0.5f;
        ImVec4 barColor = colorFor(n.level);
        barColor.w = barAlpha;
        float barWidth = width * (1.0f - t);
        draw->AddRectFilled(ImVec2(rMin.x, rMax.y - 3.0f),
                            ImVec2(rMin.x + barWidth, rMax.y),
                            ImGui::ColorConvertFloat4ToU32(barColor), 3.0f);

        // Text
        ImGui::SetCursorScreenPos(ImVec2(cursor.x + 8.0f, cursor.y + 10.0f));
        ImGui::PushTextWrapPos(cursor.x + width - 8.0f);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, alpha));
        ImGui::TextUnformatted(n.message.c_str());
        ImGui::PopStyleColor();
        ImGui::PopTextWrapPos();

        // Advance cursor past the card
        ImGui::SetCursorScreenPos(ImVec2(cursor.x, rMax.y + 6.0f));
        ImGui::Dummy(ImVec2(width, 0.0f));
    }

    ImGui::End();
}
