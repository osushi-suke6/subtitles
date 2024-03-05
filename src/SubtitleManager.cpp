#include "SubtitleManager.h"

namespace Subtitles
{
	void SubtitleManager::AddSubtitle(RE::SubtitleInfo* info)
	{
		if (!info) {
			return;
		}

		activeSubtitles.push_back(info);
	}

	void SubtitleManager::ShowSubtitles()
	{
		auto iniSettings = RE::INISettingCollection::GetSingleton();
		float maxSubtitleDistance = iniSettings->GetSetting("fMaxSubtitleDistance:Interface")->GetFloat();
		bool showSpeakerName = iniSettings->GetSetting("bShowSubtitleSpeakerName:Interface")->GetBool();
		uint32_t speakerNameColor = iniSettings->GetSetting("iSubtitleSpeakerNameColor:Interface")->GetUInt();

		constexpr const char* speakerNameFmtString = "<font color='#{:06X}'>{}</font>: {}";

		std::stringstream bigSubtitle{};

		bool first = true;
		for (auto info : activeSubtitles) {
			// `targetDistance` is the squared distance
			if (info && info->targetDistance < maxSubtitleDistance * maxSubtitleDistance) {
				if (!first) {
					bigSubtitle << "<br>";
				}
				if (showSpeakerName) {
					// TODO: get DisplayName instead
					auto speakerName = info->speaker.get()->GetName();
					bigSubtitle << std::format(speakerNameFmtString, speakerNameColor, speakerName, info->subtitle.c_str());
				} else {
					bigSubtitle << info->subtitle.c_str();
				}

				first = false;
			}
		}

		activeSubtitles.clear();
		logger::info("big subtitle: {}", bigSubtitle.str());

		auto hudMenu = RE::UI::GetSingleton()->GetMenu<RE::HUDMenu>(RE::HUDMenu::MENU_NAME);
		auto root = hudMenu->GetRuntimeData().root;

		if (bigSubtitle.str().length() > 0) {
			RE::GFxValue asStr(bigSubtitle.str().c_str());
			root.Invoke("ShowSubtitle", nullptr, &asStr, 1);
		} else {
			root.Invoke("HideSubtitle", nullptr, nullptr, 0);
		}
	}
}