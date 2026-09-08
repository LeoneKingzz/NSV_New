#include "hook.h"

constexpr auto kTrampolineSize = 14;

void MessageHandler(SKSE::MessagingInterface::Message *a_msg)
{
	switch (a_msg->type)
	{
	case SKSE::MessagingInterface::kDataLoaded:
        hooks::animEventHandler::Register(false, true);
        hooks::OnMeleeHitHook::install();
        // hooks::InputEventHandler::SinkEventHandlers();
        break;

    case SKSE::MessagingInterface::kPostPostLoad:
        // hooks::OnMeleeHitHook::GetSingleton()->init();
        break;

    default:

		break;
	}
}

void Init() {
    // hooks::OnMeleeHitHook::install_pluginListener();
    auto messaging = SKSE::GetMessagingInterface();
    messaging->RegisterListener("SKSE", MessageHandler);
}

void Load() { hooks::OnMeleeHitHook::install_protected(); }

void onSKSEInit() {
    const auto papyrus = SKSE::GetPapyrusInterface();
    papyrus->Register(hooks::OnMeleeHitHook::BindPapyrusFunctions);
}

void PreLoad() {
    // hooks::CastingHandler::Register();
    // thanks to KoeniglichePM
    if (hooks::SpellEquipManager::registery != nullptr) {
        // if the thread is there, then destroy and delete it
        // if it is joinable and not running it has already finished, but needs to be joined before
        // it can be destroyed savely
        hooks::SpellEquipManager::registery->~thread();
        delete hooks::SpellEquipManager::registery;
        hooks::SpellEquipManager::registery = nullptr;
    }
    hooks::SpellEquipManager::registery = new std::thread(hooks::SpellEquipManager::Register);
    hooks::SpellEquipManager::registery->detach();
}

void InitializeLog() {
    std::shared_ptr<spdlog::sinks::sink> sink;
    if (IsDebuggerPresent()) {
        sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
    } else {
        auto path = SKSE::log::log_directory();
        if (!path) {
            stl::report_and_fail("Failed to find standard logging directory"sv);
        }
        *path /= std::format("{}.log", Plugin::NAME);
        sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);
    }

    auto logger = std::make_shared<spdlog::logger>("Global", std::move(sink));
    logger->set_level(
#ifdef NDEBUG
            spdlog::level::info
#else
            spdlog::level::debug
#endif
    );
    logger->flush_on(spdlog::level::trace);
    spdlog::set_default_logger(std::move(logger));
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%l] [%t] [%s:%#] %v");
}

SKSEPluginInfo(
    .Version = Plugin::VERSION,
    .Name = Plugin::NAME.data(),
    .Author = "LeoneKingzz",
    .StructCompatibility = SKSE::StructCompatibility::Independent,
    .RuntimeCompatibility = SKSE::VersionIndependence::AddressLibrary
);

SKSEPluginLoad(const SKSE::LoadInterface* a_skse) {

    InitializeLog();
    logger::info("Loaded plugin");
    
    SKSE::Init(a_skse, false);
    SKSE::AllocTrampoline(kTrampolineSize);
    PreLoad();
    onSKSEInit();
    Init();
    Load();

    return true;
}
