#include <ofui/ui_profile.h>

#include <algorithm>
#include <cctype>

using namespace ofui;

UiProfile *UiProfile::m_this = nullptr;

UiProfile::UiProfile()
{
    this->applyPreset(m_mode);
}

UiProfile *UiProfile::instance()
{
    if (m_this == nullptr)
        m_this = new UiProfile();

    return m_this;
}

void UiProfile::applyPreset(UiMode mode)
{
    // Advanced is everything. Simple is everything minus the groups listed
    // below, which is the only place the difference between the two profiles is
    // written down.

    for (auto &feature : m_features)
        feature = true;

    if (mode == UiMode::Simple)
    {
        m_features[static_cast<int>(UiFeature::SelectionFilters)] = false;
        m_features[static_cast<int>(UiFeature::BeamTypes)] = false;
        m_features[static_cast<int>(UiFeature::LoadDialogs)] = false;
        m_features[static_cast<int>(UiFeature::BcDialogs)] = false;
        m_features[static_cast<int>(UiFeature::Materials)] = false;
        m_features[static_cast<int>(UiFeature::EigenmodeDetails)] = false;
    }
}

void UiProfile::setMode(UiMode mode)
{
    // Re-applying the same mode is not a no-op: it discards any per-feature
    // override made since, which is what picking a profile again should mean.

    m_mode = mode;
    this->applyPreset(mode);
    m_revision++;
}

UiMode UiProfile::mode() const
{
    return m_mode;
}

bool UiProfile::has(UiFeature feature) const
{
    if (feature == UiFeature::None)
        return true;

    const int idx = static_cast<int>(feature);

    if (idx < 0 || idx >= static_cast<int>(UiFeature::Count))
        return true;

    return m_features[idx];
}

void UiProfile::setFeature(UiFeature feature, bool flag)
{
    if (feature == UiFeature::None)
        return;

    const int idx = static_cast<int>(feature);

    if (idx < 0 || idx >= static_cast<int>(UiFeature::Count))
        return;

    if (m_features[idx] == flag)
        return;

    m_features[idx] = flag;
    m_revision++;
}

int UiProfile::revision() const
{
    return m_revision;
}

std::string UiProfile::modeName(UiMode mode)
{
    return (mode == UiMode::Simple) ? "simple" : "advanced";
}

UiMode UiProfile::modeFromName(const std::string &name, UiMode defaultMode)
{
    std::string lowered = name;

    std::transform(lowered.begin(), lowered.end(), lowered.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

    if (lowered == "simple")
        return UiMode::Simple;

    if (lowered == "advanced")
        return UiMode::Advanced;

    return defaultMode;
}
