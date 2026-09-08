#include "hook.h"

void MessageHandler(SKSE::MessagingInterface::Message *a_msg)
{
	switch (a_msg->type)
	{
	case SKSE::MessagingInterface::kDataLoaded:
		hooks::animEventHandler::Register(false, true);
		hooks::OnMeleeHitHook::install();
		//hooks::CombatMagic::Install();
		// hooks::InputEventHandler::SinkEventHandlers();
		//hooks::install_postload();
		//hooks::SpellCastHook::Install();

		break;

	// case SKSE::MessagingInterface::kPostLoad:
	// 	hooks::install_postload();
		

	//     break;

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

void Load(){
	// hooks::OnMeleeHitHook::install_protected();
	hooks::InstallCombat();
	//hooks::OnMeleeHitHook::install_Aim();
	//hooks::SpellCastHook::Install();
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