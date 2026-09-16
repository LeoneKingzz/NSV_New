#include "hook.h"

// constexpr auto kTrampolineSize = 14;

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

void Init()
{
    hooks::OnMeleeHitHook::install_pluginListener();
    auto messaging = SKSE::GetMessagingInterface();
    messaging->RegisterListener("SKSE", MessageHandler);
}

void Load([[maybe_unused]] bool VR)
{
    hooks::OnMeleeHitHook::install_protected();
}

void onSKSEInit()
{
    const auto papyrus = SKSE::GetPapyrusInterface();
    papyrus->Register(hooks::OnMeleeHitHook::BindPapyrusFunctions);
}

void PreLoad()
{
    // hooks::CastingHandler::Register();
    // thanks to KoeniglichePM
    if (hooks::SpellEquipManager::registery != nullptr)
    {
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