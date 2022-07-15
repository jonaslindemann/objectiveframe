#include <ofui/ui_window.h>

#include <imgui.h>

using namespace ofui;

UiWindow::UiWindow(const std::string name)
	:m_name{name},
	 m_visible{true},
	 m_windowFlags{ ImGuiWindowFlags_AlwaysAutoResize },
	 m_updatePos{false},
	 m_corner{-1}
{
}

UiWindow::~UiWindow()
{
}

std::shared_ptr<UiWindow> UiWindow::create(const std::string name)
{
	return std::make_shared<UiWindow>(name);
}

void UiWindow::draw()
{
	if (m_visible)
	{
		if (m_updatePos)
		{
			const float PAD = 10.0f;
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
			ImVec2 work_size = viewport->WorkSize;
			ImVec2 window_pos, window_pos_pivot;
			window_pos.x = (m_corner & 1) ? (work_pos.x + work_size.x - PAD) : (work_pos.x + PAD);
			window_pos.y = (m_corner & 2) ? (work_pos.y + work_size.y - PAD) : (work_pos.y + PAD);
			window_pos_pivot.x = (m_corner & 1) ? 1.0f : 0.0f;
			window_pos_pivot.y = (m_corner & 2) ? 1.0f : 0.0f;
			ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
			m_updatePos = false;
		}
		doPreDraw();
		ImGui::Begin(m_name.c_str(), &m_visible, m_windowFlags); //, nullptr, ImGuiWindowFlags_AlwaysAutoResize);
		doDraw();
		ImGui::End();
		doPostDraw();
	}
}

void UiWindow::setWindowFlags(ImGuiWindowFlags flags)
{
	m_windowFlags = flags;
}

ImGuiWindowFlags UiWindow::windowFlags()
{
	return m_windowFlags;
}

void UiWindow::setVisible(bool flag)
{
	m_visible = flag;
}

bool UiWindow::visible()
{
	return m_visible;
}

void UiWindow::show()
{
	m_visible = true;
}

void UiWindow::hide()
{
	m_visible = false;
}

void UiWindow::setSize(int w, int h)
{
	ImGui::SetWindowSize(m_name.c_str(), ImVec2(float(w), float(h)));
}

void UiWindow::align(int corner)
{
	m_corner = corner;
	m_updatePos = true;
}

void UiWindow::doPreDraw()
{
}

void UiWindow::doDraw()
{
}

void UiWindow::doPostDraw()
{
}
