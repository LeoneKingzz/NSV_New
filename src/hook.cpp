#include "hook.h"

namespace hooks
{
	void OnMeleeHitHook::Set_iFrames(RE::Actor* actor)
	{
		actor->SetGraphVariableBool("bIframeActive", true);
		actor->SetGraphVariableBool("bInIframe", true);
	}

	void OnMeleeHitHook::Reset_iFrames(RE::Actor* actor)
	{
		actor->SetGraphVariableBool("bIframeActive", false);
		actor->SetGraphVariableBool("bInIframe", false);
	}

	void OnMeleeHitHook::dispelEffect(RE::MagicItem *spellForm, RE::Actor *a_actor)
	{
		if (const auto targetActor = a_actor->AsMagicTarget(); targetActor)
		{
			if (spellForm && spellForm->effects[0] && spellForm->effects[0]->baseEffect && targetActor->HasMagicEffect(spellForm->effects[0]->baseEffect))
			{
				if (auto activeEffects = targetActor->GetActiveEffectList(); activeEffects)
				{
					for (const auto &effect : *activeEffects)
					{
						if (effect && effect->spell && effect->spell == spellForm)
						{
							effect->Dispel(true);
						}
					}
				}
			}
		}
	}

	bool Has_Magiceffect_Keyword(const RE::Actor *a_actor, const RE::BGSKeyword *a_key)
	{
		static RE::TESConditionItem cond;
		static std::once_flag flag;
		std::call_once(flag, [&]()
					   {
        cond.data.functionData.function = RE::FUNCTION_DATA::FunctionID::kHasMagicEffectKeyword;
        cond.data.flags.opCode          = RE::CONDITION_ITEM_DATA::OpCode::kEqualTo;
		cond.data.object                = RE::CONDITIONITEMOBJECT::kSelf;
        cond.data.comparisonValue.f     = 0.0f; });

		ConditionParam cond_param;
		cond_param.form = const_cast<RE::BGSKeyword *>(a_key->As<RE::BGSKeyword>());
		cond.data.functionData.params[0] = std::bit_cast<void *>(cond_param);

		RE::ConditionCheckParams params(const_cast<RE::TESObjectREFR *>(a_actor->As<RE::TESObjectREFR>()), nullptr);
		return cond(params);
	}

	bool IsSpellTarget(const RE::Actor *a_actor, const RE::SpellItem *a_spell, float a_comparison_value)
	{
		static RE::TESConditionItem cond;
		static std::once_flag flag;
		std::call_once(flag, [&]()
					   {
        cond.data.functionData.function = RE::FUNCTION_DATA::FunctionID::kIsSpellTarget;
        cond.data.flags.opCode          = RE::CONDITION_ITEM_DATA::OpCode::kEqualTo;
		cond.data.object                = RE::CONDITIONITEMOBJECT::kSelf;
        cond.data.comparisonValue.f     = a_comparison_value; });

		ConditionParam cond_param;
		cond_param.form = const_cast<RE::SpellItem *>(a_spell->As<RE::SpellItem>());
		cond.data.functionData.params[0] = std::bit_cast<void *>(cond_param);

		RE::ConditionCheckParams params(const_cast<RE::TESObjectREFR *>(a_actor->As<RE::TESObjectREFR>()), nullptr);
		return cond(params);
	}

	bool IsValidLifeState(RE::Actor *a_actor, bool checkDeath)
	{
		if (checkDeath)
		{
			switch (a_actor->AsActorState()->GetLifeState())
			{
			case RE::ACTOR_LIFE_STATE::kDying:
			case RE::ACTOR_LIFE_STATE::kDead:
				return false;

			default:
				return true;
			}
		}
		else
		{
			switch (a_actor->AsActorState()->GetLifeState())
			{
			case RE::ACTOR_LIFE_STATE::kBleedout:
			case RE::ACTOR_LIFE_STATE::kDying:
			case RE::ACTOR_LIFE_STATE::kDead:
			case RE::ACTOR_LIFE_STATE::kUnconcious:
			case RE::ACTOR_LIFE_STATE::kEssentialDown:
				return false;

			default:
				return true;
			}
		}
	}

	bool HasBoundWeaponEquipped(const RE::Actor *a_actor, RE::MagicSystem::CastingSource type)
	{
		static RE::TESConditionItem cond;
		static std::once_flag flag;
		std::call_once(flag, [&]()
					   {
        cond.data.functionData.function = RE::FUNCTION_DATA::FunctionID::kHasBoundWeaponEquipped;
        cond.data.flags.opCode          = RE::CONDITION_ITEM_DATA::OpCode::kEqualTo;
        cond.data.object                = RE::CONDITIONITEMOBJECT::kSelf;
        cond.data.comparisonValue.f     = 0.0f; });

		ConditionParam cond_param;
		cond_param.i = static_cast<int32_t>(type);
		cond.data.functionData.params[0] = std::bit_cast<void *>(cond_param);

		RE::ConditionCheckParams params(const_cast<RE::TESObjectREFR *>(a_actor->As<RE::TESObjectREFR>()), nullptr);
		return cond(params);
	}

	bool GetLineOfSight(const RE::Actor *a_actor, const RE::Actor *a_target, float a_comparison_value)
	{
		static RE::TESConditionItem cond;
		static std::once_flag flag;
		std::call_once(flag, [&]()
					   {
        cond.data.functionData.function = RE::FUNCTION_DATA::FunctionID::kGetLineOfSight;
        cond.data.flags.opCode          = RE::CONDITION_ITEM_DATA::OpCode::kEqualTo;
        cond.data.comparisonValue.f     = a_comparison_value; });

		ConditionParam cond_param;
		cond_param.form = const_cast<RE::TESObjectREFR *>(a_target->As<RE::TESObjectREFR>());
		cond.data.functionData.params[0] = std::bit_cast<void *>(cond_param);

		RE::ConditionCheckParams params(const_cast<RE::TESObjectREFR *>(a_actor->As<RE::TESObjectREFR>()),
										const_cast<RE::TESObjectREFR *>(a_target->As<RE::TESObjectREFR>()));
		return cond(params);
	}

	bool GetIsGhost(const RE::Actor *a_actor, float a_comparison_value)
	{
		static RE::TESConditionItem cond;
		static std::once_flag flag;
		std::call_once(flag, [&]()
					   {
        cond.data.functionData.function = RE::FUNCTION_DATA::FunctionID::kGetIsGhost;
        cond.data.flags.opCode          = RE::CONDITION_ITEM_DATA::OpCode::kEqualTo;
		cond.data.object                = RE::CONDITIONITEMOBJECT::kSelf;
        cond.data.comparisonValue.f     = a_comparison_value; });

		RE::ConditionCheckParams params(const_cast<RE::TESObjectREFR *>(a_actor->As<RE::TESObjectREFR>()), nullptr);
		return cond(params);
	}

	bool IsUndead(const RE::Actor *a_actor, float a_comparison_value)
	{
		static RE::TESConditionItem cond;
		static std::once_flag flag;
		std::call_once(flag, [&]()
					   {
        cond.data.functionData.function = RE::FUNCTION_DATA::FunctionID::kIsUndead;
        cond.data.flags.opCode          = RE::CONDITION_ITEM_DATA::OpCode::kEqualTo;
		cond.data.object                = RE::CONDITIONITEMOBJECT::kSelf;
        cond.data.comparisonValue.f     = a_comparison_value; });

		RE::ConditionCheckParams params(const_cast<RE::TESObjectREFR *>(a_actor->As<RE::TESObjectREFR>()), nullptr);
		return cond(params);
	}

	// float OnMeleeHitHook::GetActorValuePercent(RE::Actor *a_actor, RE::ActorValue a_value)
	// {
	// 	auto result = 0.0f;
	// 	auto cur_value = a_actor->AsActorValueOwner()->GetActorValue(a_value);
	// 	auto perm_value = a_actor->AsActorValueOwner()->GetPermanentActorValue(a_value);

	// 	if (perm_value != 0.0f)
	// 	{
	// 		result = cur_value / perm_value;
	// 	}
	// 	return result;
	// }

	bool OnMeleeHitHook::isHumanoid(RE::Actor* a_actor)
	{
		auto bodyPartData = a_actor->GetRace() ? a_actor->GetRace()->bodyPartData : nullptr;
		return bodyPartData && bodyPartData->GetFormID() == 0x1d;
	}

	void OnMeleeHitHook::Patch_Spell_List()
	{
		auto DS = Settings::GetSingleton();

		auto spellList = DS->general.bWhiteListApproach ? get_valid_spellList<RE::SpellItem>(LookupMods(Settings::GetSingleton()->include_spells_mods.inc_mods), LookupKeywords(Settings::GetSingleton()->include_spells_keywords.inc_keywords), true) : get_valid_spellList<RE::SpellItem>(LookupMods(Settings::GetSingleton()->exclude_spells_mods.exc_mods), LookupKeywords(Settings::GetSingleton()->exclude_spells_keywords.exc_keywords), false);

		const auto dataHandler = RE::TESDataHandler::GetSingleton();

		auto id = dataHandler->LookupFormID(0x800, "NSV Spell Copies.esp");

		// no of records between files = 16775168

		if (id != 0)
		{
            // spellcopies.espFound = true;
        }

		if (const auto spell_copies_list = dataHandler->LookupForm<RE::BGSListForm>(0x800, "NSV Spell Copies.esp"); spell_copies_list)
		{
			spell_copies_list->forms.clear();
		}

		if (const auto playable_spells_list = dataHandler->LookupForm<RE::BGSListForm>(0x98F, "NPC Spell Variance.esp"); playable_spells_list)
		{
			playable_spells_list->forms.clear();

			for (const auto &book : dataHandler->GetFormArray<RE::TESObjectBOOK>())
			{
				if (book)
				{
					const auto spell = book->GetSpell();
					if (!spell)
					{
						continue;
					}
					playable_spells_list->AddForm(spell);
				}
			}
		}

        uint32_t lastFormId = 0;  // last mod
        uint32_t firstFormId = 0; // last mod

        firstFormId = id + 1;
        lastFormId = firstFormId;

        for (auto indv_spell : spellList) {
			if (indv_spell) {
				for (auto indv_effect : indv_spell->effects) {
					if (indv_effect && indv_effect->baseEffect) {
						if (indv_effect->baseEffect->data.associatedForm) {
							auto limboform = indv_effect->baseEffect->data.associatedForm;
							if (limboform && limboform->Is(RE::FormType::Spell)) {
								auto a_spell = limboform->As<RE::SpellItem>();
								if (a_spell) {
									std::vector<RE::SpellItem*>::iterator position = std::find(spellList.begin(), spellList.end(), a_spell);
									if (position != spellList.end()) {
										spellList.erase(position);
									}
								}
							}
						}
					}
				}
			}
		}

		

		for (auto indv_spell : spellList) {
			if (indv_spell && indv_spell->GetSpellType() == RE::MagicSystem::SpellType::kSpell && indv_spell->GetDelivery() != RE::MagicSystem::Delivery::kTouch 
			&& indv_spell->GetCastingType() != RE::MagicSystem::CastingType::kScroll && indv_spell->GetCastingType() != RE::MagicSystem::CastingType::kConstantEffect) {

				bool invalid_spell = false;

				for (auto indv_effect : indv_spell->effects) {
					if (indv_effect && indv_effect->baseEffect) {
						auto Archy = indv_effect->baseEffect->data.archetype;

						if (Archy == RE::EffectSetting::Archetype::kValueAndParts || Archy == RE::EffectSetting::Archetype::kTelekinesis || Archy == RE::EffectSetting::Archetype::kConcussion
						|| Archy == RE::EffectSetting::Archetype::kSlowTime || Archy == RE::EffectSetting::Archetype::kDisguise || Archy == RE::EffectSetting::Archetype::kVampireLord 
						|| Archy == RE::EffectSetting::Archetype::kGrabActor || Archy == RE::EffectSetting::Archetype::kWerewolfFeed|| Archy == RE::EffectSetting::Archetype::kCureAddiction
						|| Archy == RE::EffectSetting::Archetype::kLock|| Archy == RE::EffectSetting::Archetype::kOpen || Archy == RE::EffectSetting::Archetype::kWerewolf
						|| Archy == RE::EffectSetting::Archetype::kSpawnScriptedRef || Archy == RE::EffectSetting::Archetype::kCureDisease|| Archy == RE::EffectSetting::Archetype::kNightEye
						|| Archy == RE::EffectSetting::Archetype::kGuide || Archy == RE::EffectSetting::Archetype::kLight || Archy == RE::EffectSetting::Archetype::kDarkness 
						|| Archy == RE::EffectSetting::Archetype::kDetectLife) {
							invalid_spell = true;
						}
					}
				}

				if (invalid_spell){
					continue;
				}


				RE::Effect* effect = new RE::Effect;
				effect->cost = 0.0f;
				effect->effectItem.area = 0;
				effect->effectItem.duration = 0;
				effect->effectItem.magnitude = 0.0f;

                auto check_CC_TA = GetEffect_CC_TA(indv_spell, lastFormId);
                auto check_CC_TL = GetEffect_CC_TL(indv_spell, lastFormId);
                auto check_CC_Self = GetEffect_CC_Self(indv_spell, lastFormId);
                auto check_CC_Aimed = GetEffect_CC_Aimed(indv_spell, lastFormId);

                auto check_FF_TA = GetEffect_FF_TA(indv_spell, lastFormId);
                auto check_FF_TL = GetEffect_FF_TL(indv_spell, lastFormId);
                auto check_FF_Self = GetEffect_FF_Self(indv_spell, lastFormId);
                auto check_FF_Aimed = GetEffect_FF_Aimed(indv_spell, lastFormId);

                bool unrecognized_spell = false;

				switch (indv_spell->GetCastingType()) {
				case RE::MagicSystem::CastingType::kFireAndForget:

					switch (indv_spell->GetDelivery()) {
					case RE::MagicSystem::Delivery::kAimed:
                        if (check_FF_Aimed.first){
							effect->baseEffect = check_FF_Aimed.second;

						}else{

							unrecognized_spell = true;
						}
						break;

					case RE::MagicSystem::Delivery::kSelf:
					    if (check_FF_Self.first){
							effect->baseEffect = check_FF_Self.second;

						} else {

							unrecognized_spell = true;
						}
						break;

					case RE::MagicSystem::Delivery::kTargetActor:
					    if (check_FF_TA.first){
							effect->baseEffect = check_FF_TA.second;

						} else {

							unrecognized_spell = true;
						}
						break;

					case RE::MagicSystem::Delivery::kTargetLocation:
					    if (check_FF_TL.first){
							effect->baseEffect = check_FF_TL.second;

						} else {

							unrecognized_spell = true;
						}
						break;

					default:
						break;
					}

					break;

				case RE::MagicSystem::CastingType::kConcentration:

					switch (indv_spell->GetDelivery()) {
					case RE::MagicSystem::Delivery::kAimed:
					    if (check_CC_Aimed.first){
							effect->baseEffect = check_CC_Aimed.second;

						} else {

							unrecognized_spell = true;
						}
						break;

					case RE::MagicSystem::Delivery::kSelf:
					    if (check_CC_Self.first){
							effect->baseEffect = check_CC_Self.second;

						} else {

							unrecognized_spell = true;
						}
						break;

					case RE::MagicSystem::Delivery::kTargetActor:
						if (check_CC_TA.first){
							effect->baseEffect = check_CC_TA.second;

						} else {

							unrecognized_spell = true;
						}
						break;

					case RE::MagicSystem::Delivery::kTargetLocation:
					    if (check_CC_TL.first){
							effect->baseEffect = check_CC_TL.second;

						} else {

							unrecognized_spell = true;
						}
						break;

					default:
						break;
					}

					break;

				default:
					break;
				}

				if (unrecognized_spell){
					continue;
				}
				

				indv_spell->effects.emplace_back(effect);
			}
			continue;
		}
	}


	bool OnMeleeHitHook::isPowerAttacking(RE::Actor* a_actor)
	{
		auto currentProcess = a_actor->GetActorRuntimeData().currentProcess;
		if (currentProcess) {
			auto highProcess = currentProcess->high;
			if (highProcess) {
				auto attackData = highProcess->attackData;
				if (attackData) {
					auto flags = attackData->data.flags;
					return flags.any(RE::AttackData::AttackFlag::kPowerAttack);
				}
			}
		}
		return false;
	}


	bool OnMeleeHitHook::IsCasting(RE::Actor* a_actor)
	{
		bool result = false;

		if ((GetBoolVariable(a_actor, "IsCastingRight")) || (GetBoolVariable(a_actor, "IsCastingLeft")) || (GetBoolVariable(a_actor, "IsCastingDual")))
		{
			result = true;
		}

		return result;
	}

	void OnMeleeHitHook::InterruptAttack(RE::Actor* a_actor){
		a_actor->NotifyAnimationGraph("attackStop");
		a_actor->NotifyAnimationGraph("recoilStop");
		a_actor->NotifyAnimationGraph("bashStop");
		a_actor->NotifyAnimationGraph("blockStop");
		a_actor->NotifyAnimationGraph("staggerStop");
	}

	std::vector<RE::TESForm*> OnMeleeHitHook::GetEquippedForm(RE::Actor* actor)
	{
		std::vector<RE::TESForm*> Hen;

		auto limboform = actor->GetActorRuntimeData().currentProcess;

		if (limboform && limboform->GetEquippedLeftHand()) {
			Hen.emplace_back(limboform->GetEquippedLeftHand());
		}
		if (limboform && limboform->GetEquippedRightHand()) {
			Hen.emplace_back(limboform->GetEquippedRightHand());
		}

		return Hen;
	}

	bool OnMeleeHitHook::IsWeaponOut(RE::Actor *actor)
	{
		bool result = false;
		auto form_list = GetEquippedForm(actor);

		if (!form_list.empty()) {
			for (auto form : form_list) {
				if (form) {
					switch (*form->formType) {
					case RE::FormType::Weapon:
						if (const auto equippedWeapon = form->As<RE::TESObjectWEAP>()) {
							switch (equippedWeapon->GetWeaponType()) {
							case RE::WEAPON_TYPE::kOneHandSword:
							case RE::WEAPON_TYPE::kOneHandDagger:
							case RE::WEAPON_TYPE::kOneHandAxe:
							case RE::WEAPON_TYPE::kOneHandMace:
							case RE::WEAPON_TYPE::kTwoHandSword:
							case RE::WEAPON_TYPE::kTwoHandAxe:
							case RE::WEAPON_TYPE::kBow:
							case RE::WEAPON_TYPE::kCrossbow:
								result = true;
								break;
							default:
								break;
							}
						}
						break;
					case RE::FormType::Armor:
						if (auto equippedShield = form->As<RE::TESObjectARMO>()) {
							result = true;
						}
						break;
					default:
						break;
					}
					if (result) {
						break;
					}
				}
				continue;
			}
		}
		return result;
	}

	bool OnMeleeHitHook::IsMeleeOnly(RE::Actor* a_actor)
	{
		using TYPE = RE::CombatInventoryItem::TYPE;

		auto result = false;

		auto combatCtrl = a_actor->GetActorRuntimeData().combatController;
		auto CombatInv = combatCtrl ? combatCtrl->inventory : nullptr;
		if (CombatInv) {
			for (const auto item : CombatInv->equippedItems) {
				if (item.item) {
					switch (item.item->GetType()) {
					case TYPE::kMelee:
						result = true;
						break;
					default:
						break;
					}
				}
				if (result){
					break;
				}
			}
		}

		return result;
	}

	bool OnMeleeHitHook::IsEquipped(RE::Actor *a_actor, RE::TESBoundObject * a_object)
	{
		bool result = false;

		if (auto combatCtrl = a_actor->GetActorRuntimeData().combatController; combatCtrl)
		{
			if (auto CombatInv = combatCtrl->inventory; CombatInv)
			{
				for (auto &invent_item : CombatInv->equippedItems)
				{
					if (invent_item.item && invent_item.item.get() && invent_item.item.get()->item && invent_item.item.get()->item == a_object)
					{
						result = true;

						break;
					}
				}
			}
		}
		return result;
	}

	void OnMeleeHitHook::EquipfromInvent(RE::Actor* a_actor, RE::FormID a_formID)
	{
		auto inv = a_actor->GetInventory();
		for (auto& [item, data] : inv) {
			const auto& [count, entry] = data;
			if (count > 0 && entry->object->formID == a_formID) {
				RE::ActorEquipManager::GetSingleton()->EquipObject(a_actor, entry->object);
				break;
			}
			continue;
		}
	}

	float OnMeleeHitHook::AV_Mod(RE::Actor* a_actor, int actor_value, float input, float mod)
	{
		if (actor_value > 0){
			int k;
			for (k = 0; k <= actor_value; k += 1) {
				input += mod;
			}
		}

		return input;
	}

	bool OnMeleeHitHook::GetBoolVariable(RE::Actor *a_actor, std::string a_string)
	{
		auto result = false;
		a_actor->GetGraphVariableBool(a_string, result);
		return result;
	}

	int OnMeleeHitHook::GetIntVariable(RE::Actor *a_actor, std::string a_string)
	{
		auto result = 0;
		a_actor->GetGraphVariableInt(a_string, result);
		return result;
	}

	float OnMeleeHitHook::GetFloatVariable(RE::Actor *a_actor, std::string a_string)
	{
		auto result = 0.0f;
		a_actor->GetGraphVariableFloat(a_string, result);
		return result;
	}


	class OurEventSink :
		public RE::BSTEventSink<RE::TESSwitchRaceCompleteEvent>,
		public RE::BSTEventSink<RE::TESEquipEvent>,
		public RE::BSTEventSink<RE::TESCombatEvent>,
		public RE::BSTEventSink<RE::TESActorLocationChangeEvent>,
		public RE::BSTEventSink<RE::TESSpellCastEvent>,
		public RE::BSTEventSink<RE::TESDeathEvent>,
		public RE::BSTEventSink<RE::TESCellFullyLoadedEvent>,
		public RE::BSTEventSink<RE::MenuOpenCloseEvent>,
		public RE::BSTEventSink<RE::TESTopicInfoEvent>,
		public RE::BSTEventSink<SKSE::ModCallbackEvent>
	{
		OurEventSink() = default;
		OurEventSink(const OurEventSink&) = delete;
		OurEventSink(OurEventSink&&) = delete;
		OurEventSink& operator=(const OurEventSink&) = delete;
		OurEventSink& operator=(OurEventSink&&) = delete;

	public:
		static OurEventSink* GetSingleton()
		{
			static OurEventSink singleton;
			return &singleton;
		}

		RE::BSEventNotifyControl ProcessEvent(const RE::TESSwitchRaceCompleteEvent* event, RE::BSTEventSource<RE::TESSwitchRaceCompleteEvent>*)
		{
			auto a_actor = event->subject->As<RE::Actor>();

			if (!a_actor) {
				return RE::BSEventNotifyControl::kContinue;
			}

			return RE::BSEventNotifyControl::kContinue;
		}

		RE::BSEventNotifyControl ProcessEvent(const RE::TESCellFullyLoadedEvent *event, RE::BSTEventSource<RE::TESCellFullyLoadedEvent> *)
		{
			
			return RE::BSEventNotifyControl::kContinue;
		}

		RE::BSEventNotifyControl ProcessEvent(const RE::TESDeathEvent *event, RE::BSTEventSource<RE::TESDeathEvent> *)
		{
			auto a_actor = event->actorDying->As<RE::Actor>();

			if (!a_actor)
			{
				return RE::BSEventNotifyControl::kContinue;
			}

			if (a_actor->IsPlayerRef())
			{
				OnMeleeHitHook::GetSingleton()->scan_activeRunes(nullptr, nullptr, false, false, true);
				OnMeleeHitHook::GetSingleton()->ClearUpdates(a_actor, true);
			}else
			{
				OnMeleeHitHook::GetSingleton()->scan_activeRunes(a_actor, nullptr, false, true);
				OnMeleeHitHook::GetSingleton()->ClearUpdates(a_actor);
			}

			return RE::BSEventNotifyControl::kContinue;
		}

		RE::BSEventNotifyControl ProcessEvent(const RE::TESEquipEvent* event, RE::BSTEventSource<RE::TESEquipEvent>*){

			if (!event || !event->actor)
			{
				return RE::BSEventNotifyControl::kContinue;
			}
			auto a_actor = event->actor->As<RE::Actor>();

			if (!a_actor) {
				return RE::BSEventNotifyControl::kContinue;
			}

			if (!(OnMeleeHitHook::GetBoolVariable(a_actor, "bNSV_IsinCombat") && OnMeleeHitHook::GetBoolVariable(a_actor, "bNSV_IsMage")))
			{
				return RE::BSEventNotifyControl::kContinue;
			}

			if (OnMeleeHitHook::IsInVampireLordForm(a_actor))
			{
				return RE::BSEventNotifyControl::kContinue;
				
			}

			auto item = RE::TESForm::LookupByID<RE::TESBoundObject>(event->baseObject);
			if (!item)
			{
				return RE::BSEventNotifyControl::kContinue;
			}
			if (event->equipped)
			{
				// if(item->IsMagicItem() && item->Is(RE::FormType::Spell)){

				// 	if (OnMeleeHitHook::GetBoolVariable(a_actor, "bNSV_Equip_OK"))
				// 	{
				// 		a_actor->SetGraphVariableBool("bNSV_Equip_OK", false);
				// 		OnMeleeHitHook::GetSingleton()->RegisterforUpdate(a_actor, std::forward_as_tuple(nullptr, std::chrono::steady_clock::now(), 4000ms, "EquipUnequip_Update"));
				// 	}
				// }
			}

			return RE::BSEventNotifyControl::kContinue;
		}

		RE::BSEventNotifyControl ProcessEvent(const SKSE::ModCallbackEvent* event, RE::BSTEventSource<SKSE::ModCallbackEvent>*)
		{
			if (!event || event->eventName.empty())
			{
				return RE::BSEventNotifyControl::kContinue;
			}

			if (event->eventName == "KID_KeywordDistributionDone")
			{
				logger::info("recieved KID finished event");

				Settings::GetSingleton()->Load();
				OnMeleeHitHook::GetSingleton()->Patch_Spell_List();
			}

			if (event->eventName == "VLS_Transform_Event")
			{
				// logger::info("recieved VLS Tranform event");

				const auto a_actor = event->sender->As<RE::Actor>();
				if (!a_actor)
				{
					return RE::BSEventNotifyControl::kContinue;
				}
				if (!OnMeleeHitHook::GetBoolVariable(a_actor, "bNSV_IsScanning_SpellList"))
				{
					OnMeleeHitHook::GetSingleton()->Scan_Valid_Caster(a_actor);
				}
			}

			return RE::BSEventNotifyControl::kContinue;
		}

		RE::BSEventNotifyControl ProcessEvent(const RE::TESCombatEvent* event, RE::BSTEventSource<RE::TESCombatEvent>*){
			auto a_actor = event->actor->As<RE::Actor>();

			if (!a_actor || a_actor->IsPlayerRef()) {
				return RE::BSEventNotifyControl::kContinue;
			}

			switch (event->newState.get()) {
			case RE::ACTOR_COMBAT_STATE::kCombat:
				a_actor->SetGraphVariableBool("bNSV_IsinCombat", true);

				OnMeleeHitHook::Vary_AI_Scores(a_actor, true);
				// if (!OnMeleeHitHook::GetBoolVariable(a_actor, "bNSV_Equip_OK"))
				// {
				// 	a_actor->SetGraphVariableBool("bNSV_Equip_OK", true);
				// }

				if (!OnMeleeHitHook::GetBoolVariable(a_actor, "bNSV_IsScanning_SpellList"))
				{
					if (!OnMeleeHitHook::Is_VLSserana(a_actor))
					{
						OnMeleeHitHook::GetSingleton()->Scan_Valid_Caster(a_actor);
					}
				}

				if (OnMeleeHitHook::GetBoolVariable(a_actor, "bNSV_Buff_Cool"))
				{
					a_actor->SetGraphVariableBool("bNSV_Buff_Cool", false);
				}
				if (OnMeleeHitHook::GetBoolVariable(a_actor, "bNSV_DeBuff_Cool"))
				{
					a_actor->SetGraphVariableBool("bNSV_DeBuff_Cool", false);
				}
				if (OnMeleeHitHook::GetBoolVariable(a_actor, "bNSV_Trap_Cool"))
				{
					a_actor->SetGraphVariableBool("bNSV_Trap_Cool", false);
				}
				break;

			case RE::ACTOR_COMBAT_STATE::kSearching:

				a_actor->SetGraphVariableBool("bNSV_IsinCombat", false);
				break;

			case RE::ACTOR_COMBAT_STATE::kNone:

				a_actor->SetGraphVariableBool("bNSV_IsinCombat", false);

				// if (!OnMeleeHitHook::GetBoolVariable(a_actor, "bNSV_Equip_OK"))
				// {
				// 	a_actor->SetGraphVariableBool("bNSV_Equip_OK", true);
				// }
				if (OnMeleeHitHook::GetBoolVariable(a_actor, "bNSV_Buff_Cool"))
				{
					a_actor->SetGraphVariableBool("bNSV_Buff_Cool", false);
				}
				if (OnMeleeHitHook::GetBoolVariable(a_actor, "bNSV_DeBuff_Cool"))
				{
					a_actor->SetGraphVariableBool("bNSV_DeBuff_Cool", false);
				}
				if (OnMeleeHitHook::GetBoolVariable(a_actor, "bNSV_Trap_Cool"))
				{
					a_actor->SetGraphVariableBool("bNSV_Trap_Cool", false);
				}

				OnMeleeHitHook::GetSingleton()->ClearUpdates(a_actor);

				break;

			default:
				break;
			}

			return RE::BSEventNotifyControl::kContinue;
		}

		RE::BSEventNotifyControl ProcessEvent(const RE::TESActorLocationChangeEvent* event, RE::BSTEventSource<RE::TESActorLocationChangeEvent>*)
		{
			auto a_actor = event->actor->As<RE::Actor>();

			if (!a_actor || !a_actor->IsPlayerRef()) {
				return RE::BSEventNotifyControl::kContinue;
			}

			OnMeleeHitHook::GetSingleton()->scan_activeRunes(nullptr, nullptr, false, false, true);

			return RE::BSEventNotifyControl::kContinue;
		}

		RE::BSEventNotifyControl ProcessEvent(const RE::TESSpellCastEvent* event, RE::BSTEventSource<RE::TESSpellCastEvent>*)
		{
			if (!event || !event->object)
			{
				return RE::BSEventNotifyControl::kContinue;
			}
			auto a_actor = event->object->As<RE::Actor>();

			if (!a_actor)
			{
				return RE::BSEventNotifyControl::kContinue;
			}

			if (!OnMeleeHitHook::GetBoolVariable(a_actor, "bNSV_IsinCombat"))
			{
				return RE::BSEventNotifyControl::kContinue;
			}

			auto eSpell = RE::TESForm::LookupByID(event->spell);

			if (eSpell && eSpell->Is(RE::FormType::Spell)) {
				auto rSpell = eSpell->As<RE::SpellItem>();
				switch (rSpell->GetSpellType()) {
				case RE::MagicSystem::SpellType::kSpell:

					break;

				default:
					break;
				}
			}
			return RE::BSEventNotifyControl::kContinue;
		}

		RE::BSEventNotifyControl ProcessEvent(const RE::TESTopicInfoEvent *a_event, RE::BSTEventSource<RE::TESTopicInfoEvent>*)
		{
			if (!a_event || !a_event->speakerRef || a_event->type.get() != RE::TESTopicInfoEvent::TopicInfoEventType::kTopicBegin)
			{
				return RE::BSEventNotifyControl::kContinue;
			}
			auto info = RE::TESForm::LookupByID<RE::TESTopicInfo>(a_event->topicInfoFormID);
			auto actor = a_event->speakerRef->As<RE::Actor>();
			if (!info || !actor || !info->parentTopic)
				return RE::BSEventNotifyControl::kContinue;
			const auto t = info->parentTopic;
			std::string EDID = clib_util::editorID::get_editorID(t);
			switch (hash(EDID.c_str(), EDID.size()))
			{
			case "NSVTacticsSpellSwordYes"_h:
				Settings::GetSingleton()->general.askedH = actor->GetHandle();
				Settings::GetSingleton()->general.askedWant = 1;
				Settings::GetSingleton()->general.asked = true;
				break;

			case "NSVTacticsSpellSwordNo"_h:
				Settings::GetSingleton()->general.askedH = actor->GetHandle();
				Settings::GetSingleton()->general.askedWant = 2;
				Settings::GetSingleton()->general.asked = true;
				break;

			case "NSVTacticsHealerYes"_h:
				Settings::GetSingleton()->general.askedH = actor->GetHandle();
				Settings::GetSingleton()->general.askedWant = 3;
				Settings::GetSingleton()->general.asked = true;
				break;

			case "NSVTacticsHealerNo"_h:
				Settings::GetSingleton()->general.askedH = actor->GetHandle();
				Settings::GetSingleton()->general.askedWant = 4;
				Settings::GetSingleton()->general.asked = true;
				break;

			case "NSVTacticsSupportYes"_h:
				Settings::GetSingleton()->general.askedH = actor->GetHandle();
				Settings::GetSingleton()->general.askedWant = 5;
				Settings::GetSingleton()->general.asked = true;
				break;

			case "NSVTacticsSupportNo"_h:
				Settings::GetSingleton()->general.askedH = actor->GetHandle();
				Settings::GetSingleton()->general.askedWant = 6;
				Settings::GetSingleton()->general.asked = true;
				break;

			default:
				break;
			}

			return RE::BSEventNotifyControl::kContinue;
		}

		RE::BSEventNotifyControl ProcessEvent(const RE::MenuOpenCloseEvent *a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>*)
		{
			if (a_event && a_event->opening && std::string_view(a_event->menuName.c_str()) == RE::DialogueMenu::MENU_NAME)
			{
				SKSE::GetTaskInterface()->AddTask([]
												  { ApplyDialogueSetting(); 
												});
			}
			if (a_event && !a_event->opening && std::string_view(a_event->menuName.c_str()) == RE::DialogueMenu::MENU_NAME && Settings::GetSingleton()->general.asked)
			{
				SKSE::GetTaskInterface()->AddTask([]
												  {
					HandleDialogueChoice();
					
				});
			}
			return RE::BSEventNotifyControl::kContinue;
		}
	};

	void HandleDialogueChoice()
	{
		auto a_actor = Settings::GetSingleton()->general.askedH.get();
		const int want = Settings::GetSingleton()->general.askedWant;

		Settings::GetSingleton()->general.asked = false;
		Settings::GetSingleton()->general.askedH.reset();
		if (!a_actor){
			return;
		}
		// "NSV_Healer"
		// "NSV_Support"
		std::string a_message;

		switch (want)
		{
		case 1:
			if (a_actor->HasKeywordString("NSV_SpellSword"))
			{
				a_message = "is already fighting as a SpellSword";
			}else
			{
				a_actor->GetActorBase()->AddKeyword(RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_SpellSword"));
				a_message = "will now fight as a SpellSword";
			}
			break;

		case 2:
			if (a_actor->HasKeywordString("NSV_SpellSword"))
			{
				a_actor->GetActorBase()->RemoveKeyword(RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_SpellSword"));
				a_message = "will stop fighting as a SpellSword";
			}
			else
			{
				a_message = "isn't currently fighting as a SpellSword";
			}
			break;

		case 3:
			if (a_actor->HasKeywordString("NSV_Healer"))
			{
				a_message = "is already prioritizing supportive heal spells";
			}
			else
			{
				a_actor->GetActorBase()->AddKeyword(RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Healer"));
				a_message = "will begin prioritizing supportive heal spells";
			}
			break;

		case 4:
			if (a_actor->HasKeywordString("NSV_Healer"))
			{
				a_actor->GetActorBase()->RemoveKeyword(RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Healer"));
				a_message = "will stop prioritizing supportive heal spells";
			}
			else
			{
				a_message = "isn't currently prioritizing supportive heal spells";
			}
			break;

		case 5:
			if (a_actor->HasKeywordString("NSV_Support"))
			{
				a_message = "is already demostrating Supportive behaviour";
			}
			else
			{
				a_actor->GetActorBase()->AddKeyword(RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Support"));
				a_message = "will now demostrate Supportive behaviour";
			}
			break;

		case 6:
			if (a_actor->HasKeywordString("NSV_Support"))
			{
				a_actor->GetActorBase()->RemoveKeyword(RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Support"));
				a_message = "will stop demostrating Supportive behaviour";
			}
			else
			{
				a_message = "isn't currently demostrating Supportive behaviour";
			}
			break;

		default:
			break;
		}

        if(!a_message.empty())
		{
			std::string space = " ";
			std::string b_message = a_actor->GetDisplayFullName() + space + a_message;
			RE::SendHUDMessage::ShowHUDMessage(b_message.c_str());
		}
	}

	void ApplyDialogueSetting()
	{
		if (auto a_quest = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESQuest>(0xB0A, "NPC Spell Variance.esp"); a_quest && !a_quest->IsRunning())
		{
			if(a_quest->Start()){

				RE::SendHUDMessage::ShowHUDMessage("Started NSV Tactics Quest");
			}else{

				RE::SendHUDMessage::ShowHUDMessage("Failed to Start NSV Tactics Quest");
			}
		}
	}

	float OnMeleeHitHook::get_angle_he_me(RE::Actor *me, RE::Actor *he, RE::BGSAttackData *attackdata){
		auto he_me = PolarAngle(me->GetPosition() - he->GetPosition());
		auto head = PolarAngle(he->GetHeading(false) * 180.0f / PI);
		if (attackdata)
			head = head.add(attackdata->data.attackAngle);
		auto angle = he_me.sub(head).to_normangle();
		return angle;
	}

	RE::BGSAttackData* OnMeleeHitHook::get_attackData(RE::Actor *a){
		if (!a->GetActorRuntimeData().currentProcess || !a->GetActorRuntimeData().currentProcess->high)
			return nullptr;
		return a->GetActorRuntimeData().currentProcess->high->attackData.get();
	}

	void OnMeleeHitHook::Scan_Valid_Caster(RE::Actor *a_actor)
	{
		a_actor->SetGraphVariableBool("bNSV_IsScanning_SpellList", true);
		//const auto MagicWard = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("MagicWard");

        auto H = RE::TESDataHandler::GetSingleton();
		if (const auto spell_copies_list = H->LookupForm<RE::BGSListForm>(0x800, "NSV Spell Copies.esp"))
		{
			if (const auto playable_spells_list = H->LookupForm<RE::BGSListForm>(0x98F, "NPC Spell Variance.esp"))
			{
				std::vector<RE::SpellItem *> a_result;

				for (const auto &a_form : playable_spells_list->forms)
				{
					if (a_form && a_form->Is(RE::FormType::Spell))
					{
						if (const auto a_spell = a_form->As<RE::SpellItem>(); a_spell)
						{
							if (!a_actor->HasSpell(a_spell))
							{
								continue;
							}
							a_result.emplace_back(a_spell);
						}
					}
				}

				if (a_result.empty() && !Is_VLSserana(a_actor) && !a_actor->HasKeywordString("TLH_Harkon_Key"))
				{
					return;
				}

				std::vector<RE::SpellItem *> shouldhave_spelllist;

				for (const auto &form : spell_copies_list->forms)
				{
					if (form && form->Is(RE::FormType::Spell))
					{
						if (const auto s_spell = form->As<RE::SpellItem>(); s_spell)
						{
							for (const auto &new_spell : a_result)
							{
								if (new_spell)
								{
									if (s_spell->fullName == new_spell->fullName)
									{
										if (s_spell->equipSlot == new_spell->equipSlot)
										{
											if (a_actor->HasKeywordString("NSV_VampireLord"))
											{
												const auto NSV_Tag_VampireLord_Ascendant = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_VampireLord_Ascendant");
												if(Is_VLSserana(a_actor)){
													if (getrace_VLserana(a_actor) || GetBoolVariable(a_actor, "bVLS_IsTransforming"))
													{
														//ascendant form
														if (NSV_Tag_VampireLord_Ascendant && s_spell->HasKeyword(NSV_Tag_VampireLord_Ascendant))
														{
															//add only ascendant spells
															if (!a_actor->HasSpell(s_spell))
															{
																a_actor->AddSpell(s_spell);
                                                                if (Settings::GetSingleton()->general.bDebug) {
																	logger::info("N: {} adding spell: {}"sv, a_actor->GetName(), s_spell->fullName);
                                                                }
															}
														}
													}else{
														//mortal form
														if (NSV_Tag_VampireLord_Ascendant && !s_spell->HasKeyword(NSV_Tag_VampireLord_Ascendant))
														{
															// add only mortal spells
															if (!a_actor->HasKeywordString("NSV_SpellSword"))
															{
																if (!a_actor->HasSpell(s_spell))
																{
																	a_actor->AddSpell(s_spell);
																	if (Settings::GetSingleton()->general.bDebug) {
																		logger::info("N: {} adding spell: {}"sv, a_actor->GetName(), s_spell->fullName);
																	}
																}
															}
															else
															{

																if (a_actor->HasSpell(s_spell))
																{
																	a_actor->RemoveSpell(s_spell);
																	if (Settings::GetSingleton()->general.bDebug) {
																		logger::info("N: {} removing spell: {}"sv, a_actor->GetName(), s_spell->fullName);
																	}
																}
															}
														}
													}
												}
												
											}else{
												if (!a_actor->HasKeywordString("NSV_SpellSword"))
												{
													if (!a_actor->HasSpell(s_spell))
													{
														a_actor->AddSpell(s_spell);
														if (Settings::GetSingleton()->general.bDebug) {
															logger::info("N: {} adding spell: {}"sv, a_actor->GetName(), s_spell->fullName);
														}
													}
												}
												else
												{

													if (a_actor->HasSpell(s_spell))
													{
														a_actor->RemoveSpell(s_spell);
														if (Settings::GetSingleton()->general.bDebug) {
															logger::info("N: {} removing spell: {}"sv, a_actor->GetName(), s_spell->fullName);
														}
													}
												}
											}
											

											shouldhave_spelllist.emplace_back(s_spell);

											break;
										}
									}
								}
							}
						}
					}
				}

				if (shouldhave_spelllist.empty() && !Is_VLSserana(a_actor) && !a_actor->HasKeywordString("TLH_Harkon_Key"))
				{
					return;
				}

				if (!GetBoolVariable(a_actor, "bNSV_IsMage"))
				{
					a_actor->SetGraphVariableBool("bNSV_IsMage", true);
				}

				if (a_actor->HasKeywordString("NSV_SpellSword"))
				{
					GetSingleton()->RegisterforUpdate(a_actor, std::forward_as_tuple(nullptr, std::chrono::steady_clock::now(), 8000ms, "AIScores_Update"));
				}

				// if (!a_actor->HasKeywordString("NSV_ModCombatInvent_Exclude"))
				// {
				// 	GetSingleton()->RegisterforUpdate(a_actor, std::forward_as_tuple(nullptr, std::chrono::steady_clock::now(), 3000ms, "ModCombat_Inventory_Update"));
				// }

				// Toggle_Casting_LE(a_actor, true);

				a_result.clear();

				if (const auto clone_key_list = H->LookupForm<RE::BGSListForm>(0x991, "NPC Spell Variance.esp"))
				{
					for (const auto &form : spell_copies_list->forms)
					{
						if (form && form->Is(RE::FormType::Spell))
						{
							if (const auto s_spell = form->As<RE::SpellItem>(); s_spell)
							{
								if (a_actor->HasSpell(s_spell))
								{
									std::vector<RE::SpellItem *>::iterator position = std::find(shouldhave_spelllist.begin(), shouldhave_spelllist.end(), s_spell);
									if (position == shouldhave_spelllist.end())
									{
										a_actor->RemoveSpell(s_spell);
										if (Settings::GetSingleton()->general.bDebug) {
											logger::info("N: {} removing non-matching spell: {}"sv, a_actor->GetName(), s_spell->fullName);
										}
									}
									else
									{
										if (!s_spell->HasKeywordInList(clone_key_list, false))
										{
											a_actor->RemoveSpell(s_spell);
											if (Settings::GetSingleton()->general.bDebug) {
												logger::info("N: {} clone keyword not found removing spell: {}"sv, a_actor->GetName(), s_spell->fullName);
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}

		a_actor->SetGraphVariableBool("bNSV_IsScanning_SpellList", false);
	}

	void OnMeleeHitHook::getBodyPos(RE::Actor *a_actor, RE::NiPoint3 &pos)
	{
		if (!a_actor->GetActorRuntimeData().race) {
			return;
		}
		RE::BGSBodyPart* bodyPart = a_actor->GetActorRuntimeData().race->bodyPartData->parts[0];
		if (!bodyPart) {
			return;
		}
		auto targetPoint = a_actor->GetNodeByName(bodyPart->targetName.c_str());
		if (!targetPoint) {
			return;
		}

		pos = targetPoint->world.translate;
	}

	void OnMeleeHitHook::Vary_AI_Scores(RE::Actor *a_actor, bool combat_start)
	{
		
		// a_actor->SetGraphVariableBool("bNSV_Fear_Active", true);

		if(combat_start){
			a_actor->SetGraphVariableBool("bNSV_InterruptCast_Cloak", false);
			a_actor->SetGraphVariableBool("bNSV_InterruptCast_Armour", false);
			a_actor->SetGraphVariableBool("bNSV_InterruptCast_Heal", false);
			a_actor->SetGraphVariableBool("bNSV_InterruptCast_Invisibility", false);
			a_actor->SetGraphVariableBool("bNSV_InterruptCast_Summon", false);
			a_actor->SetGraphVariableBool("bNSV_InterruptCast_Ward", false);
			a_actor->SetGraphVariableBool("bNSV_InterruptCast_Survive", false);

			a_actor->SetGraphVariableBool("bNSV_Teleport_Active", static_cast<bool>(GetSingleton()->GenerateRandomInt(0, 1)));
			a_actor->SetGraphVariableBool("bNSV_Trap_Active", static_cast<bool>(GetSingleton()->GenerateRandomInt(0, 1)));
			a_actor->SetGraphVariableBool("bNSV_Absorb_Active", static_cast<bool>(GetSingleton()->GenerateRandomInt(0, 1)));
			a_actor->SetGraphVariableBool("bNSV_Control_Active", static_cast<bool>(GetSingleton()->GenerateRandomInt(0, 1)));
			a_actor->SetGraphVariableBool("bNSV_CalmSeduce_Active", static_cast<bool>(GetSingleton()->GenerateRandomInt(0, 1)));
			a_actor->SetGraphVariableBool("bNSV_Buff_Active", static_cast<bool>(GetSingleton()->GenerateRandomInt(0, 1)));
			a_actor->SetGraphVariableBool("bNSV_DeBuff_Active", static_cast<bool>(GetSingleton()->GenerateRandomInt(0, 1)));
			a_actor->SetGraphVariableBool("bNSV_RConvert_Active", static_cast<bool>(GetSingleton()->GenerateRandomInt(0, 1)));
			a_actor->SetGraphVariableBool("bNSV_Armour_Active", static_cast<bool>(GetSingleton()->GenerateRandomInt(0, 1)));
			a_actor->SetGraphVariableBool("bNSV_Cloak_Active", static_cast<bool>(GetSingleton()->GenerateRandomInt(0, 1)));

			if (a_actor->HasKeywordString("NSV_Support"))
			{
				a_actor->SetGraphVariableBool("bNSV_Damage_Active", static_cast<bool>(GetSingleton()->GenerateRandomInt(0, 1)));
			}else
			{
				a_actor->SetGraphVariableBool("bNSV_Damage_Active", true);
			}

			a_actor->SetGraphVariableInt("iNSV_Score_Counter", GetSingleton()->GenerateRandomInt(1, 5));

		}else{

			a_actor->SetGraphVariableBool("bNSV_Teleport_Active", static_cast<bool>(GetSingleton()->GenerateRandomInt(0, 1)));
			a_actor->SetGraphVariableBool("bNSV_Trap_Active", static_cast<bool>(GetSingleton()->GenerateRandomInt(0, 1)));
			a_actor->SetGraphVariableBool("bNSV_Absorb_Active", static_cast<bool>(GetSingleton()->GenerateRandomInt(0, 1)));
			a_actor->SetGraphVariableBool("bNSV_Control_Active", static_cast<bool>(GetSingleton()->GenerateRandomInt(0, 1)));
			a_actor->SetGraphVariableBool("bNSV_CalmSeduce_Active", static_cast<bool>(GetSingleton()->GenerateRandomInt(0, 1)));
			a_actor->SetGraphVariableBool("bNSV_Buff_Active", static_cast<bool>(GetSingleton()->GenerateRandomInt(0, 1)));
			a_actor->SetGraphVariableBool("bNSV_DeBuff_Active", static_cast<bool>(GetSingleton()->GenerateRandomInt(0, 1)));
			a_actor->SetGraphVariableBool("bNSV_RConvert_Active", static_cast<bool>(GetSingleton()->GenerateRandomInt(0, 1)));
			a_actor->SetGraphVariableBool("bNSV_Armour_Active", static_cast<bool>(GetSingleton()->GenerateRandomInt(0, 1)));
			a_actor->SetGraphVariableBool("bNSV_Cloak_Active", static_cast<bool>(GetSingleton()->GenerateRandomInt(0, 1)));

			if (a_actor->HasKeywordString("NSV_Support"))
			{
				a_actor->SetGraphVariableBool("bNSV_Damage_Active", static_cast<bool>(GetSingleton()->GenerateRandomInt(0, 1)));
			}
			else
			{
				a_actor->SetGraphVariableBool("bNSV_Damage_Active", true);
			}

			a_actor->SetGraphVariableInt("iNSV_Score_Counter", GetSingleton()->GenerateRandomInt(1, 5));
		}
		//Combat_Behaviour(a_actor);

		if (GetBoolVariable(a_actor, "bNSV_IsMage") && !a_actor->HasKeywordString("NSV_ModCombatInvent_Exclude"))
		{
			if (auto combatcontrol = a_actor->GetActorRuntimeData().combatController; combatcontrol)
			{
				Mod_CombatInventory_VaryScores(a_actor, combatcontrol);
			}
		}
	}

	void OnMeleeHitHook::Scan_Soul_Gems(RE::Actor *a_actor)
	{
		bool has_gem = false;
		const auto inv = a_actor->GetInventory();
		for (const auto &[item, data] : inv)
		{
			const auto &[count, entry] = data;
			if (count > 0 && entry->object && entry->object->Is(RE::FormType::SoulGem))
			{
				if (entry->GetSoulLevel() <= RE::SOUL_LEVEL::kNone)
				{
					a_actor->SetGraphVariableBool("bNSV_Self_HasEmptySoulGem", true);
					has_gem = true;
					break;
				}
			}
			continue;
		}
		if (!has_gem)
		{
			a_actor->SetGraphVariableBool("bNSV_Self_HasEmptySoulGem", false);
		}
	}

	void OnMeleeHitHook::Clone_Spell(RE::SpellItem *e_spell, uint32_t &lastFormId)
	{
        auto H = RE::TESDataHandler::GetSingleton();

        if (e_spell->fullName.empty() || !e_spell->equipSlot)
		{
			return;
		}


		if (const auto spell_copies_list = H->LookupForm<RE::BGSListForm>(0x800, "NSV Spell Copies.esp"); spell_copies_list)
		{
			for (const auto &form : spell_copies_list->forms)
			{
				if (form && form->Is(RE::FormType::Spell))
				{
					const auto s_spell = form->As<RE::SpellItem>();

					if (s_spell->fullName == e_spell->fullName)
					{
						if (s_spell->equipSlot == e_spell->equipSlot)
						{
							return;
						}
					}
				}
			}

			if (const auto factory = RE::IFormFactory::GetFormFactoryByType(e_spell->GetFormType()); factory)
			{
				if (const auto &form_copy = factory->Create(); form_copy)
				{
                    if (const auto& exist_form = RE::TESForm::LookupByID(lastFormId); exist_form) {
						if (Settings::GetSingleton()->general.bDebug) {
							logger::info("existing form found. Name: {} ID: {} EDID: {}"sv, exist_form->GetName(), exist_form->formID, clib_util::editorID::get_editorID(exist_form));
						}
                    }

                    form_copy->SetFormID(lastFormId, false);
                    ++lastFormId;

                    form_copy->Copy(e_spell);

					if (const auto &spell_copy = form_copy->As<RE::SpellItem>(); form_copy)
					{
						spell_copy->boundData = e_spell->boundData;
						spell_copy->data = e_spell->data;
						spell_copy->effects = e_spell->effects;
						copyComponent<RE::TESDescription>(e_spell, spell_copy);
						copyComponent<RE::BGSKeywordForm>(e_spell, spell_copy);
						copyComponent<RE::BGSPickupPutdownSounds>(e_spell, spell_copy);
						copyComponent<RE::TESModelTextureSwap>(e_spell, spell_copy);
						copyComponent<RE::TESModel>(e_spell, spell_copy);
						copyComponent<RE::BGSMessageIcon>(e_spell, spell_copy);
						copyComponent<RE::TESIcon>(e_spell, spell_copy);
						copyComponent<RE::TESFullName>(e_spell, spell_copy);
						copyComponent<RE::TESValueForm>(e_spell, spell_copy);
						copyComponent<RE::TESWeightForm>(e_spell, spell_copy);
						copyComponent<RE::BGSDestructibleObjectForm>(e_spell, spell_copy);
						copyComponent<RE::TESEnchantableForm>(e_spell, spell_copy);
						copyComponent<RE::BGSBlockBashData>(e_spell, spell_copy);
						copyComponent<RE::BGSEquipType>(e_spell, spell_copy);
						copyComponent<RE::TESAttackDamageForm>(e_spell, spell_copy);
						copyComponent<RE::TESBipedModelForm>(e_spell, spell_copy);

						spell_copies_list->AddForm(spell_copy);
						if (Settings::GetSingleton()->general.bDebug) {
							logger::info("New clone: {} ID: {} EDID: {}"sv, spell_copy->GetName(), spell_copy->formID, clib_util::editorID::get_editorID(spell_copy));
						}
                    }
				}
			}
		}
	}

	void OnMeleeHitHook::Scan_Teammates_NeedHealing(RE::Actor *a_actor)
	{
		if (const auto combatGroup = a_actor->GetCombatGroup())
		{
			bool team_needheals = false;
			int within_dist_counter = 0;
			for (auto &memberData : combatGroup->members)
			{
				if (auto ally = memberData.memberHandle.get(); ally)
				{
					RE::Actor *refr = ally.get();

					if (refr == a_actor || refr->IsDead())
					{
						continue;
					}
					auto cur_health = refr->AsActorValueOwner()->GetActorValue(RE::ActorValue::kHealth);
					auto perm_health = refr->AsActorValueOwner()->GetPermanentActorValue(RE::ActorValue::kHealth);
					
					if (perm_health <= 0 || cur_health <= 0){
						continue;
					}
					auto h_percent = cur_health / perm_health;
					
					if (a_actor->GetPosition().GetDistance(refr->GetPosition()) <= 320.0f)
					{
						within_dist_counter += 1;
					}else{
						if (within_dist_counter > 0)
						{
							within_dist_counter -= 1;
						}
					}

					if (GetIsGhost(refr, 0.0) && h_percent <= 0.9f)
					{
						if (within_dist_counter > 0)
						{
							refr->SetGraphVariableBool("bNSV_Teammate_healtarget", true);
							if (h_percent <= 0.65f || a_actor->HasKeywordString("NSV_Healer"))
							{
								team_needheals = true;
							}

						}else if (!isBackFacing(a_actor, refr) || a_actor->HasKeywordString("NSV_Support"))
						{
							refr->SetGraphVariableBool("bNSV_Teammate_healtarget", true);
							if (h_percent <= 0.65f || a_actor->HasKeywordString("NSV_Healer"))
							{
								team_needheals = true;
							}
						}
						

					}else{
						refr->SetGraphVariableBool("bNSV_Teammate_healtarget", false);
					}
				}
				continue;
			}
			if (team_needheals){
				
				a_actor->SetGraphVariableBool("bNSV_Teammate_needhealing", true);

				if (within_dist_counter > 0)
				{
					a_actor->SetGraphVariableBool("bNSV_Self_Teammate_needhealing", true);
				}
			
			}else{
				a_actor->SetGraphVariableBool("bNSV_Teammate_needhealing", false);
				a_actor->SetGraphVariableBool("bNSV_Self_Teammate_needhealing", false);
			}
		}
	}

	void OnMeleeHitHook::Scan_Teammates_HasWeapon(RE::Actor *a_actor){
		
		if (const auto combatGroup = a_actor->GetCombatGroup())
		{
			bool team_hasweapon = false;
			for (auto &memberData : combatGroup->members)
			{
				if (auto ally = memberData.memberHandle.get(); ally)
				{
					RE::Actor *refr = ally.get();

					if (refr == a_actor || refr->IsDead())
					{
						continue;
					}

					if (IsWeaponOut(refr) && GetIsGhost(refr, 0.0))
					{
						refr->SetGraphVariableBool("bNSV_Teammate_isWeaponOut", true);
						team_hasweapon = true;
					}
					else
					{
						refr->SetGraphVariableBool("bNSV_Teammate_isWeaponOut", false);
					}
				}
				continue;
			}
			if (team_hasweapon)
			{
				a_actor->SetGraphVariableBool("bNSV_Team_hasWeapon_member", true);
				a_actor->SetGraphVariableBool("bNSV_Self_Team_hasWeapon_member", true);
			}
			else
			{
				a_actor->SetGraphVariableBool("bNSV_Team_hasWeapon_member", false);
				a_actor->SetGraphVariableBool("bNSV_Self_Team_hasWeapon_member", false);
			}
		}
	}

	void OnMeleeHitHook::Scan_Teammates_Undead(RE::Actor *a_actor){
		
		if (const auto combatGroup = a_actor->GetCombatGroup())
		{
			bool team_hasweapon = false;
			for (auto &memberData : combatGroup->members)
			{
				if (auto ally = memberData.memberHandle.get(); ally)
				{
					RE::Actor *refr = ally.get();

					if (refr->IsDead())
					{
						continue;
					}

					if (refr->HasKeywordString("ActorTypeUndead") || IsUndead(refr, 1.0))
					{
						refr->SetGraphVariableBool("bNSV_Teammate_isUndead", true);
						team_hasweapon = true;
					}
					else
					{
						refr->SetGraphVariableBool("bNSV_Teammate_isUndead", false);
					}
				}
				continue;
			}
			if (team_hasweapon)
			{
				a_actor->SetGraphVariableBool("bNSV_Team_hasUndead_member", true);
				a_actor->SetGraphVariableBool("bNSV_Self_Team_hasUndead_member", true);
			}
			else
			{
				a_actor->SetGraphVariableBool("bNSV_Team_hasUndead_member", false);
				a_actor->SetGraphVariableBool("bNSV_Self_Team_hasUndead_member", false);
			}
		}
	}

	void OnMeleeHitHook::Scan_Teammates_CommandedActor(RE::Actor *a_actor){
		
		if (const auto combatGroup = a_actor->GetCombatGroup())
		{
			bool team_hasweapon = false;
			for (auto &memberData : combatGroup->members)
			{
				if (auto ally = memberData.memberHandle.get(); ally)
				{
					RE::Actor *refr = ally.get();

					if (refr->IsDead())
					{
						continue;
					}

					if (refr->IsCommandedActor())
					{
						refr->SetGraphVariableBool("bNSV_Teammate_isSummoned", true);
						team_hasweapon = true;
					}
					else
					{
						refr->SetGraphVariableBool("bNSV_Teammate_isSummoned", false);
					}
				}
				continue;
			}
			if (team_hasweapon)
			{
				a_actor->SetGraphVariableBool("bNSV_Team_hasSummoned_member", true);
				a_actor->SetGraphVariableBool("bNSV_Self_Team_hasSummoned_member", true);
			}
			else
			{
				a_actor->SetGraphVariableBool("bNSV_Team_hasSummoned_member", false);
				a_actor->SetGraphVariableBool("bNSV_Self_Team_hasSummoned_member", false);
			}
		}
	}

	void OnMeleeHitHook::Scan_Teammates_NeedBoundWeapon(RE::Actor *a_actor){
		
		if (const auto combatGroup = a_actor->GetCombatGroup())
		{
			bool team_hasweapon = false;
			int within_dist_counter = 0;
			for (auto &memberData : combatGroup->members)
			{
				if (auto ally = memberData.memberHandle.get(); ally)
				{
					RE::Actor *refr = ally.get();

					if (refr == a_actor || refr->IsDead())
					{
						continue;
					}
					if (a_actor->GetPosition().GetDistance(refr->GetPosition()) <= 320.0f)
					{
						within_dist_counter += 1;
					}else{
						if (within_dist_counter > 0)
						{
							within_dist_counter -= 1;
						}
					}

					if (HasBoundWeaponEquipped(refr, RE::MagicSystem::CastingSource::kLeftHand) && HasBoundWeaponEquipped(refr, RE::MagicSystem::CastingSource::kRightHand) && GetIsGhost(refr, 0.0))
					{
						if (within_dist_counter > 0)
						{
							refr->SetGraphVariableBool("bNSV_Teammate_boundweapontarget", true);
							team_hasweapon = true;
						}
						else if (!isBackFacing(a_actor, refr) || a_actor->HasKeywordString("NSV_Support"))
						{
							refr->SetGraphVariableBool("bNSV_Teammate_boundweapontarget", true);
							team_hasweapon = true;
						}
					}
					else
					{
						refr->SetGraphVariableBool("bNSV_Teammate_boundweapontarget", false);
					}
				}
				continue;
			}
			if (team_hasweapon)
			{
				a_actor->SetGraphVariableBool("bNSV_Team_needBoundWeapon_member", true);

				if (within_dist_counter > 0)
				{
					a_actor->SetGraphVariableBool("bNSV_Self_Team_needBoundWeapon_member", true);
				}
			}
			else
			{
				a_actor->SetGraphVariableBool("bNSV_Team_needBoundWeapon_member", false);
				a_actor->SetGraphVariableBool("bNSV_Self_Team_needBoundWeapon_member", false);
			}
		}
	}

	void OnMeleeHitHook::Scan_Teammates_NeedWard(RE::Actor *a_actor){
		
		if (const auto combatGroup = a_actor->GetCombatGroup())
		{
			bool team_hasweapon = false;
			int within_dist_counter = 0;
			for (auto &memberData : combatGroup->members)
			{
				if (auto ally = memberData.memberHandle.get(); ally)
				{
					RE::Actor *refr = ally.get();

					if (refr == a_actor || refr->IsDead())
					{
						continue;
					}

					if (a_actor->GetPosition().GetDistance(refr->GetPosition()) <= 320.0f)
					{
						within_dist_counter += 1;
					}else{
						if (within_dist_counter > 0)
						{
							within_dist_counter -= 1;
						}
					}

					if (refr->AsMagicTarget() && !HasMagicEffectWithKeyword(refr->AsMagicTarget(), RE::TESForm::LookupByEditorID<RE::BGSKeyword>("MagicWard"), 0) && GetIsGhost(refr, 0.0))
					{
						if (within_dist_counter > 0)
						{
							refr->SetGraphVariableBool("bNSV_Teammate_wardtarget", true);
							team_hasweapon = true;
						}
						else if (!isBackFacing(a_actor, refr) || a_actor->HasKeywordString("NSV_Support"))
						{
							refr->SetGraphVariableBool("bNSV_Teammate_wardtarget", true);
							team_hasweapon = true;
						}
					}
					else
					{
						refr->SetGraphVariableBool("bNSV_Teammate_wardtarget", false);
					}
				}
				continue;
			}
			if (team_hasweapon)
			{
				a_actor->SetGraphVariableBool("bNSV_Teammate_needward", true);

				if (within_dist_counter > 0)
				{
					a_actor->SetGraphVariableBool("bNSV_Self_Teammate_needward", true);
				}
			}
			else
			{
				a_actor->SetGraphVariableBool("bNSV_Teammate_needward", false);
				a_actor->SetGraphVariableBool("bNSV_Self_Teammate_needward", false);
			}
		}
	}

	void OnMeleeHitHook::Scan_Teammates_NeedInvisibility(RE::Actor *a_actor){
		
		if (const auto combatGroup = a_actor->GetCombatGroup())
		{
			bool team_hasweapon = false;
			int within_dist_counter = 0;
			for (auto &memberData : combatGroup->members)
			{
				if (auto ally = memberData.memberHandle.get(); ally)
				{
					RE::Actor *refr = ally.get();

					if (refr == a_actor || refr->IsDead())
					{
						continue;
					}

					if (a_actor->GetPosition().GetDistance(refr->GetPosition()) <= 320.0f)
					{
						within_dist_counter += 1;
					}else{
						if (within_dist_counter > 0)
						{
							within_dist_counter -= 1;
						}
					}

					if ((refr->AsActorValueOwner()->GetActorValue(RE::ActorValue::kInvisibility) == 0) && refr->AsMagicTarget() && !HasMagicEffectWithKeyword(refr->AsMagicTarget(), RE::TESForm::LookupByEditorID<RE::BGSKeyword>("MagicInvisibility"), 0) && !HasMagicEffectWithKeyword(refr->AsMagicTarget(), RE::TESForm::LookupByEditorID<RE::BGSKeyword>("MagicCloak"), 0) && GetIsGhost(refr, 0.0)) //
					{
						if (within_dist_counter > 0)
						{
							refr->SetGraphVariableBool("bNSV_Teammate_invisibilitytarget", true);
							team_hasweapon = true;
						}
						else if (!isBackFacing(a_actor, refr) || a_actor->HasKeywordString("NSV_Support"))
						{
							refr->SetGraphVariableBool("bNSV_Teammate_invisibilitytarget", true);
							team_hasweapon = true;
						}
					}
					else
					{
						refr->SetGraphVariableBool("bNSV_Teammate_invisibilitytarget", false);
					}
				}
				continue;
			}
			if (team_hasweapon)
			{
				a_actor->SetGraphVariableBool("bNSV_Teammate_needinvisibility", true);

				if (within_dist_counter > 0)
				{
					a_actor->SetGraphVariableBool("bNSV_Self_Teammate_needinvisibility", true);
				}
			}
			else
			{
				a_actor->SetGraphVariableBool("bNSV_Teammate_needinvisibility", false);
				a_actor->SetGraphVariableBool("bNSV_Self_Teammate_needinvisibility", false);
			}
		}
	}

	float OnMeleeHitHook::get_personal_survivalRatio(RE::Actor *protagonist, RE::Actor *combat_target)
	{
		float result = 0.0f;
		float MyTeam_total_threat = 0.0f;
		float EnemyTeam_total_threat = 0.0f;
		float personal_threat = 0.0f;

		if (const auto combatGroup = protagonist->GetCombatGroup())
		{
			for (auto &memberData : combatGroup->members)
			{
				if (auto ally = memberData.memberHandle.get(); ally)
				{
					if (auto value = memberData.threatValue; value)
					{
						MyTeam_total_threat += value;
						if (ally.get() == protagonist)
						{
							personal_threat += value;
						}
					}
				}
				continue;
			}
		}

		if (const auto combatGroup = combat_target->GetCombatGroup())
		{
			for (auto &memberData : combatGroup->members)
			{
				if (auto ally = memberData.memberHandle.get(); ally)
				{
					if (auto value = memberData.threatValue; value)
					{
						EnemyTeam_total_threat += value;
					}
				}
				continue;
			}
		}

		if (MyTeam_total_threat > 0 && EnemyTeam_total_threat > 0 && personal_threat > 0)
		{

			auto personal_survival = personal_threat / EnemyTeam_total_threat;
			auto Enemy_groupSurvival = EnemyTeam_total_threat / MyTeam_total_threat;

			result = personal_survival / Enemy_groupSurvival;
		}

		return result;
	}
	float OnMeleeHitHook::get_personal_threatRatio(RE::Actor *protagonist, RE::Actor *combat_target)
	{
		float result = 0.0f;
		float personal_threat = 0.0f;
		float CTarget_threat = 0.0f;

		if (const auto combatGroup = protagonist->GetCombatGroup())
		{
			for (auto &memberData : combatGroup->members)
			{
				if (auto ally = memberData.memberHandle.get(); ally)
				{
					if (ally.get() == protagonist)
					{
						if (auto value = memberData.threatValue; value)
						{
							personal_threat += value;
							break;
						}
					}
				}
				continue;
			}
		}

		if (const auto combatGroup = combat_target->GetCombatGroup())
		{
			for (auto &memberData : combatGroup->members)
			{
				if (auto ally = memberData.memberHandle.get(); ally)
				{
					if (ally.get() == combat_target)
					{
						if (auto value = memberData.threatValue; value)
						{
							CTarget_threat += value;
							break;
						}
					}
				}
				continue;
			}
		}

		if (personal_threat > 0 && CTarget_threat > 0)
		{
			result = personal_threat / CTarget_threat;
		}

		return result;
	}
	float OnMeleeHitHook::confidence_threshold(RE::Actor *a_actor, int confidence, bool inverse)
	{
		float result = 0.0f;

		if (inverse)
		{
			switch (confidence)
			{
			case 0:
				result = 0.1f;
				break;

			case 1:
				result = 0.3f;
				break;

			case 2:
				result = 0.5f;
				break;

			case 3:
				result = 0.7f;
				break;

			case 4:
				result = 0.9f;
				break;

			default:
				break;
			}
		}
		else
		{
			switch (confidence)
			{
			case 0:
				result = 1.25f;
				break;

			case 1:
				result = 1.0f;
				break;

			case 2:
				result = 0.75f;
				break;

			case 3:
				result = 0.5f;
				break;

			case 4:
				result = 0.25f;
				break;

			default:
				break;
			}
		}
		return result;
	}

	float OnMeleeHitHook::getSpellArea(RE::SpellItem*a_spell)
	{
		uint32_t a_area = 0;

		for (const auto &a_effect: a_spell->effects)
		{
			if (a_effect && a_effect->GetArea() && a_effect->GetArea() > a_area)
			{
				a_area = a_effect->GetArea();
				break;
			}
		}

		return a_area > 0 ? static_cast<float>(a_area / 0.046875) : static_cast<float>(a_area);
	}

	std::pair<bool, double> OnMeleeHitHook::Get_Teammate_Heal(RE::Actor *a_actor, RE::SpellItem *a_spell, bool personal)
	{
		bool self = false;
		double result = 0.0;

		if(!personal)
		{
			if (const auto combatGroup = a_actor->GetCombatGroup(); combatGroup)
			{
				for (auto &memberData : combatGroup->members)
				{
					if (auto ally = memberData.memberHandle.get(); ally)
					{
						RE::Actor *refr = ally.get();

						if (refr == a_actor || refr->IsDead())
						{
							continue;
						}
						if (!GetBoolVariable(refr, "bNSV_Teammate_healtarget"))
						{
							continue;
						}
						if (EC_CheckKeywords(refr, a_spell, RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Undead"), RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Summoned")))
						{
							continue;
						}

						auto cur_health = refr->AsActorValueOwner()->GetActorValue(RE::ActorValue::kHealth);
						auto perm_health = refr->AsActorValueOwner()->GetPermanentActorValue(RE::ActorValue::kHealth);
						auto heal = static_cast<double>(perm_health - cur_health);

						if (heal > result)
						{
							result = heal;
						}
					}
					continue;
				}
			}

			if (result <= 0.0)
			{
				if (!EC_CheckKeywords(a_actor, a_spell, RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Undead"), RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Summoned")))
				{
					auto cur_health = a_actor->AsActorValueOwner()->GetActorValue(RE::ActorValue::kHealth);
					auto perm_health = a_actor->AsActorValueOwner()->GetPermanentActorValue(RE::ActorValue::kHealth);
					auto heal = static_cast<double>(perm_health - cur_health);

					if (heal > result)
					{
						self = true;
						result = heal;
					}
				}
			}

		}else
		{
			if (!EC_CheckKeywords(a_actor, a_spell, RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Undead"), RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Summoned")))
			{
				auto cur_health = a_actor->AsActorValueOwner()->GetActorValue(RE::ActorValue::kHealth);
				auto perm_health = a_actor->AsActorValueOwner()->GetPermanentActorValue(RE::ActorValue::kHealth);
				auto heal = static_cast<double>(perm_health - cur_health);

				if (heal > result)
				{
					self = true;
					result = heal;
				}
			}
		}

		return {self, result};
	}

	bool OnMeleeHitHook::Scan_Teammates_Nearby(RE::Actor *a_actor, float a_area, std::string variable)
	{
		bool valid_distance = false;
		if (const auto combatGroup = a_actor->GetCombatGroup(); combatGroup)
		{
			int within_dist_counter = 0;
			for (auto &memberData : combatGroup->members)
			{
				if (auto ally = memberData.memberHandle.get(); ally)
				{
					RE::Actor *refr = ally.get();

					if (refr == a_actor || refr->IsDead())
					{
						continue;
					}
					if(!variable.empty())
					{
						if (!GetBoolVariable(refr, variable))
						{
							continue;
						}
					}

					if (a_actor->GetPosition().GetDistance(refr->GetPosition()) <= a_area)
					{
						within_dist_counter += 1;
					}
					else
					{
						if (within_dist_counter > 0)
						{
							within_dist_counter -= 1;
						}
					}
				}
				continue;
			}
			if (within_dist_counter > 0)
			{
				valid_distance = true;
			}
			else
			{
				valid_distance = false;
			}
		}

		return valid_distance;
	}

	void OnMeleeHitHook::Scan_Teammates_inRange(RE::Actor *a_actor, float a_area)
	{
		if (const auto combatGroup = a_actor->GetCombatGroup(); combatGroup)
		{
			int within_dist_counter = 0;
			
			for (auto &memberData : combatGroup->members)
			{
				if (auto ally = memberData.memberHandle.get(); ally)
				{
					RE::Actor *refr = ally.get();

					if (refr == a_actor || refr->IsDead())
					{
						continue;
					}
					if (a_actor->GetPosition().GetDistance(refr->GetPosition()) <= a_area)
					{
						within_dist_counter += 1;
					}
					else
					{
						if (within_dist_counter > 0)
						{
							within_dist_counter -= 1;
						}
					}
				}
				continue;
			}

			a_actor->SetGraphVariableInt("iNSV_AlliesInRange", within_dist_counter);
		}
	}

	void OnMeleeHitHook::Scan_Teammates_NeedSurvival(RE::Actor *a_actor){
		
		if (const auto combatGroup = a_actor->GetCombatGroup())
		{
			bool team_needheals = false;
			int within_dist_counter = 0;
			for (auto &memberData : combatGroup->members)
			{
				if (auto ally = memberData.memberHandle.get(); ally)
				{
					RE::Actor *refr = ally.get();

					if (refr == a_actor || refr->IsDead())
					{
						continue;
					}
					if (a_actor->GetPosition().GetDistance(refr->GetPosition()) <= 320.0f)
					{
						within_dist_counter += 1;
					}else{
						if (within_dist_counter > 0)
						{
							within_dist_counter -= 1;
						}
					}
					auto cur_health = refr->AsActorValueOwner()->GetActorValue(RE::ActorValue::kHealth);
					auto perm_health = refr->AsActorValueOwner()->GetPermanentActorValue(RE::ActorValue::kHealth);
					auto cur_stamina = refr->AsActorValueOwner()->GetActorValue(RE::ActorValue::kStamina);
					auto perm_stamina = refr->AsActorValueOwner()->GetPermanentActorValue(RE::ActorValue::kStamina);
					if (perm_health <= 0 || cur_health <= 0)
					{
						continue;
					}
					auto refr_target = refr->GetActorRuntimeData().currentCombatTarget.get().get();
					if (!refr_target){
						continue;
					}
					auto R = GetSingleton();
					auto personal_survival = R->get_personal_survivalRatio(refr, refr_target);
					auto personal_threat = R->get_personal_threatRatio(refr, refr_target);
					auto confidence = static_cast<int>(refr->AsActorValueOwner()->GetActorValue(RE::ActorValue::kConfidence));

					if ((personal_survival <= R->confidence_threshold(refr, confidence)) || (personal_threat <= R->confidence_threshold(a_actor, confidence)) 
					|| ((cur_health / perm_health) < 0.35f) || ((cur_stamina / perm_stamina) < 0.05f) || (refr->AsActorState()->GetKnockState() != RE::KNOCK_STATE_ENUM::kNormal) 
					|| (refr->AsActorValueOwner()->GetActorValue(RE::ActorValue::kParalysis) > 0) && GetIsGhost(refr, 0.0))
					{
						if (within_dist_counter > 0)
						{
							refr->SetGraphVariableBool("bNSV_Teammate_survivaltarget", true);
							team_needheals = true;
						}
						else if (!isBackFacing(a_actor, refr) || a_actor->HasKeywordString("NSV_Support"))
						{
							refr->SetGraphVariableBool("bNSV_Teammate_survivaltarget", true);
							team_needheals = true;
						}
					}
					else
					{
						refr->SetGraphVariableBool("bNSV_Teammate_survivaltarget", false);
					}
				}
				continue;
			}
			if (team_needheals)
			{
				a_actor->SetGraphVariableBool("bNSV_Teammate_needsurvival", true);

				if (within_dist_counter > 0)
				{
					a_actor->SetGraphVariableBool("bNSV_Self_Teammate_needsurvival", true);
				}
			}
			else
			{
				a_actor->SetGraphVariableBool("bNSV_Teammate_needsurvival", false);
				a_actor->SetGraphVariableBool("bNSV_Self_Teammate_needsurvival", false);
			}
		}
	}

	void OnMeleeHitHook::Scan_Teammates_NeedCloak(RE::Actor *a_actor){
		
		if (const auto combatGroup = a_actor->GetCombatGroup())
		{
			bool team_hasweapon = false;
			int within_dist_counter = 0;
			for (auto &memberData : combatGroup->members)
			{
				if (auto ally = memberData.memberHandle.get(); ally)
				{
					RE::Actor *refr = ally.get();

					if (refr == a_actor || refr->IsDead())
					{
						continue;
					}

					if (a_actor->GetPosition().GetDistance(refr->GetPosition()) <= 320.0f)
					{
						within_dist_counter += 1;
					}else{
						if (within_dist_counter > 0)
						{
							within_dist_counter -= 1;
						}
					}

					if (refr->AsMagicTarget() && !HasMagicEffectWithKeyword(refr->AsMagicTarget(), RE::TESForm::LookupByEditorID<RE::BGSKeyword>("MagicCloak"), 0) && GetIsGhost(refr, 0.0))
					{
						if (within_dist_counter > 0)
						{
							refr->SetGraphVariableBool("bNSV_Teammate_cloaktarget", true);
							team_hasweapon = true;
						}
						else if (!isBackFacing(a_actor, refr) || a_actor->HasKeywordString("NSV_Support"))
						{
							refr->SetGraphVariableBool("bNSV_Teammate_cloaktarget", true);
							team_hasweapon = true;
						}
					}
					else
					{
						refr->SetGraphVariableBool("bNSV_Teammate_cloaktarget", false);
					}
				}
				continue;
			}
			if (team_hasweapon)
			{
				a_actor->SetGraphVariableBool("bNSV_Teammate_needcloak", true);

				if (within_dist_counter > 0)
				{
					a_actor->SetGraphVariableBool("bNSV_Self_Teammate_needcloak", true);
				}
			}
			else
			{
				a_actor->SetGraphVariableBool("bNSV_Teammate_needcloak", false);
				a_actor->SetGraphVariableBool("bNSV_Self_Teammate_needcloak", false);
			}
		}
	}

	void OnMeleeHitHook::Scan_Teammates_NeedArmour(RE::Actor *a_actor){
		
		if (const auto combatGroup = a_actor->GetCombatGroup())
		{
			bool team_hasweapon = false;
			int within_dist_counter = 0;
			for (auto &memberData : combatGroup->members)
			{
				if (auto ally = memberData.memberHandle.get(); ally)
				{
					RE::Actor *refr = ally.get();

					if (refr == a_actor || refr->IsDead())
					{
						continue;
					}

					if (a_actor->GetPosition().GetDistance(refr->GetPosition()) <= 320.0f)
					{
						within_dist_counter += 1;
					}else{
						if (within_dist_counter > 0)
						{
							within_dist_counter -= 1;
						}
					}

					if (refr->AsMagicTarget() && !HasMagicEffectWithKeyword(refr->AsMagicTarget(), RE::TESForm::LookupByEditorID<RE::BGSKeyword>("MagicArmorSpell"), 0) && GetIsGhost(refr, 0.0))
					{
						if (within_dist_counter > 0)
						{
							refr->SetGraphVariableBool("bNSV_Teammate_armourtarget", true);
							team_hasweapon = true;
						}
						else if (!isBackFacing(a_actor, refr) || a_actor->HasKeywordString("NSV_Support"))
						{
							refr->SetGraphVariableBool("bNSV_Teammate_armourtarget", true);
							team_hasweapon = true;
						}
					}
					else
					{
						refr->SetGraphVariableBool("bNSV_Teammate_armourtarget", false);
					}
				}
				continue;
			}
			if (team_hasweapon)
			{
				a_actor->SetGraphVariableBool("bNSV_Teammate_needarmour", true);

				if (within_dist_counter > 0)
				{
					a_actor->SetGraphVariableBool("bNSV_Self_Teammate_needarmour", true);
				}
			}
			else
			{
				a_actor->SetGraphVariableBool("bNSV_Teammate_needarmour", false);
				a_actor->SetGraphVariableBool("bNSV_Self_Teammate_needarmour", false);
			}
		}
	}

	void OnMeleeHitHook::Combat_Behaviour(RE::Actor *a_actor)
	{

		// auto combat_target = a_actor->GetActorRuntimeData().currentCombatTarget.get().get();

		// auto cur_health = a_actor->AsActorValueOwner()->GetActorValue(RE::ActorValue::kHealth);
		// auto perm_health = a_actor->AsActorValueOwner()->GetPermanentActorValue(RE::ActorValue::kHealth);
		
		// if (!(perm_health <= 0 || cur_health <= 0) && ((cur_health / perm_health) < 1.0f))
		// {
		// 	a_actor->SetGraphVariableBool("bNSV_Heal_Active", true);
		// }
		// else if (GetBoolVariable(a_actor, "bNSV_Self_Teammate_needhealing") || (combat_target && GetBoolVariable(combat_target, "bNSV_Teammate_needhealing")))
		// {
		// 	a_actor->SetGraphVariableBool("bNSV_Heal_Active", true);

		// }else{
		// 	a_actor->SetGraphVariableBool("bNSV_Heal_Active", false);
		// }

		// if (combat_target && GetLineOfSight(a_actor, combat_target, 1.0f) && GetActorValuePercent(a_actor, RE::ActorValue::kMagicka) >= 0.35f)
		// {
		// 	a_actor->SetGraphVariableBool("bNSV_DeBuff_Active", true);
			
		// }
		// else
		// {
		// 	a_actor->SetGraphVariableBool("bNSV_DeBuff_Active", false);
		// }

		// if (combat_target && a_actor->GetPosition().GetDistance(combat_target->GetPosition()) > 350.0f && GetActorValuePercent(a_actor, RE::ActorValue::kMagicka) >= 0.35f)
		// {
		// 	a_actor->SetGraphVariableBool("bNSV_Buff_Active", true);
		// }
		// else
		// {
		// 	a_actor->SetGraphVariableBool("bNSV_Buff_Active", false);
		// }

	}

	std::string OnMeleeHitHook::GetTargetVariable(RE::SpellItem *a_spell)
	{
		std::string AI_Variable;

		const auto NSV_Magic_Cloak = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Cloak");
		const auto NSV_Magic_Armour = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Armour");
		const auto NSV_Magic_Heal = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Heal");
		const auto NSV_Magic_Invisibility = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Invisibility");
		const auto NSV_Magic_Summon = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Summon");
		const auto NSV_Magic_Ward = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Ward");
		const auto NSV_Magic_Teleport = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Teleport");
		const auto NSV_Survival_Lethal = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Survival_Lethal");
		const auto NSV_Generic_Buff = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Generic_Buff");
		const auto NSV_Tag_Weapon = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Weapon");
		const auto NSV_Tag_Ally = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Ally");

		if (NSV_Magic_Cloak && a_spell->HasKeyword(NSV_Magic_Cloak))
		{
			AI_Variable = "bNSV_Teammate_cloaktarget";
			
		}
		else if (NSV_Magic_Armour && a_spell->HasKeyword(NSV_Magic_Armour))
		{
			AI_Variable = "bNSV_Teammate_armourtarget";
			
		}
		else if (NSV_Magic_Heal && a_spell->HasKeyword(NSV_Magic_Heal))
		{
			AI_Variable = "bNSV_Teammate_healtarget";

		}
		else if (NSV_Magic_Invisibility && a_spell->HasKeyword(NSV_Magic_Invisibility))
		{
			AI_Variable = "bNSV_Teammate_invisibilitytarget";
			
		}
		else if (NSV_Magic_Summon && a_spell->HasKeyword(NSV_Magic_Summon))
		{
			if (NSV_Tag_Weapon && a_spell->HasKeyword(NSV_Tag_Weapon))
			{
				AI_Variable = "bNSV_Teammate_boundweapontarget";
			}
		}
		else if (NSV_Magic_Ward && a_spell->HasKeyword(NSV_Magic_Ward))
		{
			AI_Variable = "bNSV_Teammate_wardtarget";
			
		}
		else if (NSV_Magic_Teleport && a_spell->HasKeyword(NSV_Magic_Teleport))
		{
			if (NSV_Tag_Ally && a_spell->HasKeyword(NSV_Tag_Ally))
			{
				AI_Variable = "bNSV_Teammate_survivaltarget";
			}
		}
		else if (NSV_Survival_Lethal && a_spell->HasKeyword(NSV_Survival_Lethal))
		{
			AI_Variable = "bNSV_Teammate_survivaltarget";
			
		}
		else if (NSV_Generic_Buff && a_spell->HasKeyword(NSV_Generic_Buff))
		{
			if (NSV_Tag_Weapon && a_spell->HasKeyword(NSV_Tag_Weapon))
			{
				AI_Variable = "bNSV_Teammate_isWeaponOut";
			}
		}

		return AI_Variable;
	}

	bool OnMeleeHitHook::EC_CheckKeywords(RE::Actor *refr, RE::SpellItem *a_spell, RE::BGSKeyword *NSV_Tag_Undead, RE::BGSKeyword *NSV_Tag_Summoned)
	{

		if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
		{
			if (!OnMeleeHitHook::GetBoolVariable(refr, "bNSV_Teammate_isUndead"))
			{
				return true;
			}
		}
		else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
		{

			if (!OnMeleeHitHook::GetBoolVariable(refr, "bNSV_Teammate_isSummoned"))
			{
				return true;
			}
		}
		else if (!((NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned)) || (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))))
		{

			if (OnMeleeHitHook::GetBoolVariable(refr, "bNSV_Teammate_isSummoned") || OnMeleeHitHook::GetBoolVariable(refr, "bNSV_Teammate_isUndead"))
			{
				return true;
			}
		}

		return false;
	}

	void OnMeleeHitHook::TrackAlliedConcSpells(RE::Actor *a_actor, RE::MagicCaster* &caster, RE::Actor *refr, RE::SpellItem *a_spell, RE::BGSKeyword *NSV_Magic_Heal, RE::BGSKeyword *NSV_Magic_Ward)
	{
		const auto MagicRestoreHealth = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("MagicRestoreHealth");
		const auto MagicWard = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("MagicWard");

		if (a_spell->HasKeyword(NSV_Magic_Heal))
		{
			auto Effect_List = a_spell->effects;
			for (auto Effect : Effect_List)
			{
				if (Effect && Effect->baseEffect)
				{
					if (Effect->baseEffect->HasKeyword(MagicRestoreHealth))
					{
						auto cur_health = refr->AsActorValueOwner()->GetActorValue(RE::ActorValue::kHealth);
						auto perm_health = refr->AsActorValueOwner()->GetPermanentActorValue(RE::ActorValue::kHealth);
						auto heal = static_cast<double>(perm_health - cur_health);
						if (heal <= 0)
						{
							heal = 1.0;
						}
						auto heal_intensity = static_cast<double>(Effect->GetMagnitude());

						if (heal_intensity <= 0)
						{
							heal_intensity = 1.0;
						}

						auto val = heal / heal_intensity;

						auto total = GetSingleton()->GenerateRandomDouble(val / 4.0, val);

						auto required = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::duration<double>(total));

						GetSingleton()->RegisterforUpdate(a_actor, std::forward_as_tuple(caster, std::chrono::steady_clock::now(), required, "HealConc_Update"));
						break;
					}
				}
				continue;
			}
		}
		else if (a_spell->HasKeyword(NSV_Magic_Ward))
		{
			auto Effect_List = a_spell->effects;
			for (auto Effect : Effect_List)
			{
				if (Effect && Effect->baseEffect)
				{
					if (Effect->baseEffect->HasKeyword(MagicWard))
					{
						auto num = GetSingleton()->GenerateRandomDouble(4.0, 8.0);


						auto required = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::duration<double>(num));

						GetSingleton()->RegisterforUpdate(a_actor, std::forward_as_tuple(caster, std::chrono::steady_clock::now(), required, "WardConc_Update"));
						break;
					}
				}
				continue;
			}
		}
		else
		{

			auto num = GetSingleton()->GenerateRandomDouble(7.0, 12.0);

			auto required = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::duration<double>(num));

			GetSingleton()->RegisterforUpdate(a_actor, std::forward_as_tuple(caster, std::chrono::steady_clock::now(), required, "GenericConc_Update"));
		}
	}

	bool OnMeleeHitHook::isBackFacing(RE::Actor *actor1, RE::Actor *actor2)
	{
		auto angle = actor1->GetHeadingAngle(actor2->GetPosition(), false);
		if (90 < angle || angle < -90)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	void OnMeleeHitHook::PrepareRuneCast(RE::Actor *a_actor, RE::SpellItem *a_spell, bool lefthand)
	{
		const auto MagicRune = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("MagicRune");

		const auto caster = lefthand ? a_actor->GetMagicCaster(RE::MagicSystem::CastingSource::kLeftHand) : a_actor->GetMagicCaster(RE::MagicSystem::CastingSource::kRightHand);

		const auto Effect_List = a_spell->effects;

		for (const auto &Effect : Effect_List)
		{
			if (Effect && Effect->baseEffect)
			{
				if (Effect->baseEffect->HasKeyword(MagicRune))
				{
					caster->PrepareSound(RE::MagicSystem::SoundID::kRelease, a_spell->As<RE::MagicItem>());
					break;
				}
			}
		}
	}

	void OnMeleeHitHook::Execute_Cast(RE::Actor *a_actor, RE::SpellItem *a_spell, bool lefthand, bool FF_aimed, bool FF_TA, bool FF_TL, bool CC_aimed, bool CC_TA, bool CC_TL, bool CC_Self)
	{
		if (const auto combatGroup = a_actor->GetCombatGroup(); combatGroup)
		{
			const auto NSV_Magic_Cloak = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Cloak");
			const auto NSV_Magic_Armour = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Armour");
			const auto NSV_Magic_Heal = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Heal");
			const auto NSV_Magic_Invisibility = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Invisibility");
			const auto NSV_Magic_Summon = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Summon");
			const auto NSV_Magic_Ward = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Ward");
			const auto NSV_Magic_Teleport = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Teleport");
			const auto NSV_Survival_Lethal = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Survival_Lethal");
			const auto NSV_Generic_Buff = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Generic_Buff");
			const auto NSV_Tag_Undead = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Undead");
			// const auto NSV_Tag_Vampiric = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Vampiric");
			const auto NSV_Tag_Summoned = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Summoned");
			const auto NSV_Tag_Weapon = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Weapon");
			const auto NSV_Tag_Ally = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Ally");

			bool successful_cast = false;
			bool need_allied_targeting = false;
			auto caster = lefthand ? a_actor->GetMagicCaster(RE::MagicSystem::CastingSource::kLeftHand) : a_actor->GetMagicCaster(RE::MagicSystem::CastingSource::kRightHand);
			RE::Actor *focus_ally = nullptr;
			// RE::Actor *backup_ally = nullptr;
			std::string AI_Variable;

			for (const auto &memberData : combatGroup->members)
			{
				if (const auto ally = memberData.memberHandle.get(); ally)
				{
					RE::Actor *refr = ally.get();

					if (refr == a_actor)
					{
						continue;
					}
					// if(!backup_ally)
					// {
					// 	backup_ally = refr;
					// }

					if (!(FF_TL || CC_TL))
					{
						if (NSV_Magic_Cloak && a_spell->HasKeyword(NSV_Magic_Cloak))
						{
							AI_Variable = "bNSV_Teammate_needcloak";
							if (!OnMeleeHitHook::GetBoolVariable(refr, "bNSV_Teammate_cloaktarget") || IsSpellTarget(refr, a_spell, 1.0))
							{
								continue;

							}else
							{
								if (EC_CheckKeywords(refr, a_spell, NSV_Tag_Undead, NSV_Tag_Summoned))
								{
									continue;
								}
							}
							need_allied_targeting = true;

						}else if (NSV_Magic_Armour && a_spell->HasKeyword(NSV_Magic_Armour))
						{
							AI_Variable = "bNSV_Teammate_needarmour";
							if (!OnMeleeHitHook::GetBoolVariable(refr, "bNSV_Teammate_armourtarget") || IsSpellTarget(refr, a_spell, 1.0))
							{
								continue;

							}else
							{
								if (EC_CheckKeywords(refr, a_spell, NSV_Tag_Undead, NSV_Tag_Summoned))
								{
									continue;
								}
							}
							need_allied_targeting = true;

						}else if (NSV_Magic_Heal && a_spell->HasKeyword(NSV_Magic_Heal))
						{
							AI_Variable = "bNSV_Teammate_needhealing";

							if (!OnMeleeHitHook::GetBoolVariable(refr, "bNSV_Teammate_healtarget") || IsSpellTarget(refr, a_spell, 1.0))
							{
								continue;

							}else
							{
								if (EC_CheckKeywords(refr, a_spell, NSV_Tag_Undead, NSV_Tag_Summoned))
								{
									continue;
								}
							}
							need_allied_targeting = true;

						}else if (NSV_Magic_Invisibility && a_spell->HasKeyword(NSV_Magic_Invisibility))
						{
							AI_Variable = "bNSV_Teammate_needinvisibility";
							if (!OnMeleeHitHook::GetBoolVariable(refr, "bNSV_Teammate_invisibilitytarget") || IsSpellTarget(refr, a_spell, 1.0))
							{
								continue;

							}else
							{
								if (EC_CheckKeywords(refr, a_spell, NSV_Tag_Undead, NSV_Tag_Summoned))
								{
									continue;
								}
							} 
							need_allied_targeting = true;

						}else if (NSV_Magic_Summon && a_spell->HasKeyword(NSV_Magic_Summon))
						{
							if (NSV_Tag_Weapon && a_spell->HasKeyword(NSV_Tag_Weapon))
							{
								AI_Variable = "bNSV_Team_needBoundWeapon_member";
								if (!OnMeleeHitHook::GetBoolVariable(refr, "bNSV_Teammate_boundweapontarget") || IsSpellTarget(refr, a_spell, 1.0))
								{
									continue;

								}else
								{
									if (EC_CheckKeywords(refr, a_spell, NSV_Tag_Undead, NSV_Tag_Summoned))
									{
										continue;
									}
								}
								need_allied_targeting = true;
							}
							else
							{
								// summoning ally most likely
								return;
							}

						}else if (NSV_Magic_Ward && a_spell->HasKeyword(NSV_Magic_Ward))
						{
							AI_Variable = "bNSV_Teammate_needward";
							if (!OnMeleeHitHook::GetBoolVariable(refr, "bNSV_Teammate_wardtarget") || IsSpellTarget(refr, a_spell, 1.0))
							{
								continue;

							}else
							{
								if (EC_CheckKeywords(refr, a_spell, NSV_Tag_Undead, NSV_Tag_Summoned))
								{
									continue;
								}
							}
							need_allied_targeting = true;

						}else if (NSV_Magic_Teleport && a_spell->HasKeyword(NSV_Magic_Teleport))
						{
							if (NSV_Tag_Ally && a_spell->HasKeyword(NSV_Tag_Ally))
							{
								AI_Variable = "bNSV_Teammate_needsurvival";
								if (!OnMeleeHitHook::GetBoolVariable(refr, "bNSV_Teammate_survivaltarget") || IsSpellTarget(refr, a_spell, 1.0))
								{
									continue;
								}
								else
								{
									if (EC_CheckKeywords(refr, a_spell, NSV_Tag_Undead, NSV_Tag_Summoned))
									{
										continue;
									}
								}
								need_allied_targeting = true;

							}else{
								// teleporting enemy most likely
								return;
							}

						}else if (NSV_Survival_Lethal && a_spell->HasKeyword(NSV_Survival_Lethal))
						{
							AI_Variable = "bNSV_Teammate_needsurvival";
							if (!OnMeleeHitHook::GetBoolVariable(refr, "bNSV_Teammate_survivaltarget") || IsSpellTarget(refr, a_spell, 1.0))
							{
								continue;

							}else
							{
								if (EC_CheckKeywords(refr, a_spell, NSV_Tag_Undead, NSV_Tag_Summoned))
								{
									continue;
								}
							}
							need_allied_targeting = true;

						}else if (NSV_Generic_Buff && a_spell->HasKeyword(NSV_Generic_Buff))
						{
							if (NSV_Tag_Weapon && a_spell->HasKeyword(NSV_Tag_Weapon))
							{
								if (!OnMeleeHitHook::GetBoolVariable(refr, "bNSV_Teammate_isWeaponOut") || IsSpellTarget(refr, a_spell, 1.0))
								{
									continue;

								}else
								{
									if (EC_CheckKeywords(refr, a_spell, NSV_Tag_Undead, NSV_Tag_Summoned))
									{
										continue;
									}
								}
							}
							else
							{
								if (IsSpellTarget(refr, a_spell, 1.0))
								{
									continue;
								}else
								{
									if (EC_CheckKeywords(refr, a_spell, NSV_Tag_Undead, NSV_Tag_Summoned))
									{
										continue;
									}
								}
							}

							need_allied_targeting = true;
						}
					}
					else
					{
						if (NSV_Magic_Summon && a_spell->HasKeyword(NSV_Magic_Summon))
						{
							if (NSV_Tag_Ally && a_spell->HasKeyword(NSV_Tag_Ally))
							{
								return;
							}
							else if (NSV_Tag_Weapon && !a_spell->HasKeyword(NSV_Tag_Weapon))
							{
								return;
							}
						}
					}

					successful_cast = true;

					if (!(FF_TL || CC_TL))
					{
						if (need_allied_targeting)
						{
							focus_ally = refr;
						}
					}

					break;
				}
				continue;
			}

			if (successful_cast && focus_ally)
			{
				caster->desiredTarget = focus_ally;

				if (!(FF_TL || CC_TL) && need_allied_targeting)
				{
					if (auto combatCtrl = a_actor->GetActorRuntimeData().combatController; combatCtrl)
					{
						if (auto friend_handle = focus_ally->GetHandle(); friend_handle)
						{
							for (auto aim_controller : combatCtrl->aimControllers)
							{
								if (aim_controller && aim_controller->mcaster && aim_controller->mcaster->currentSpell && aim_controller->mcaster->currentSpell == a_spell->As<RE::MagicItem>())
								{
									aim_controller->target = friend_handle;

									if (CC_aimed || CC_TA)
									{
										TrackAlliedConcSpells(a_actor, caster, focus_ally, a_spell, NSV_Magic_Heal, NSV_Magic_Ward);
									}
									// found correct aim controller, break;
									break;
								}
							}
						}
					}
				}
			}
			else
			{
				// RE::ActorEquipManager::GetSingleton()->UnequipObject(a_actor, a_spell);
				
				if(!AI_Variable.empty()){

					if (GetBoolVariable(a_actor, AI_Variable))
					{
						a_actor->SetGraphVariableBool(AI_Variable, false);
					}

					switch (hash(AI_Variable.c_str(), AI_Variable.size()))
					{
					case "bNSV_Teammate_needcloak"_h:
						a_actor->SetGraphVariableBool("bNSV_InterruptCast_Cloak", true);
						GetSingleton()->RegisterforUpdate(a_actor, std::forward_as_tuple(nullptr, std::chrono::steady_clock::now(), 3000ms, "Cloak_Active_Update"));
						break;

					case "bNSV_Teammate_needarmour"_h:
						a_actor->SetGraphVariableBool("bNSV_InterruptCast_Armour", true);
						GetSingleton()->RegisterforUpdate(a_actor, std::forward_as_tuple(nullptr, std::chrono::steady_clock::now(), 3000ms, "Armour_Active_Update"));
						break;

					case "bNSV_Teammate_needhealing"_h:
						a_actor->SetGraphVariableBool("bNSV_InterruptCast_Heal", true);
						GetSingleton()->RegisterforUpdate(a_actor, std::forward_as_tuple(nullptr, std::chrono::steady_clock::now(), 3000ms, "Heal_Active_Update"));
						break;

					case "bNSV_Teammate_needinvisibility"_h:
						a_actor->SetGraphVariableBool("bNSV_InterruptCast_Invisibility", true);
						GetSingleton()->RegisterforUpdate(a_actor, std::forward_as_tuple(nullptr, std::chrono::steady_clock::now(), 3000ms, "Invisibility_Active_Update"));
						break;

					case "bNSV_Team_needBoundWeapon_member"_h:
						a_actor->SetGraphVariableBool("bNSV_InterruptCast_Summon", true);
						GetSingleton()->RegisterforUpdate(a_actor, std::forward_as_tuple(nullptr, std::chrono::steady_clock::now(), 3000ms, "Summon_Active_Update"));
						break;

					case "bNSV_Teammate_needward"_h:
						a_actor->SetGraphVariableBool("bNSV_InterruptCast_Ward", true);
						GetSingleton()->RegisterforUpdate(a_actor, std::forward_as_tuple(nullptr, std::chrono::steady_clock::now(), 3000ms, "Ward_Active_Update"));
						break;

					case "bNSV_Teammate_needsurvival"_h:
						a_actor->SetGraphVariableBool("bNSV_InterruptCast_Survive", true);
						GetSingleton()->RegisterforUpdate(a_actor, std::forward_as_tuple(nullptr, std::chrono::steady_clock::now(), 3000ms, "Survive_Active_Update"));
						break;

					default:
						break;
					}
				}

				Vary_AI_Scores(a_actor);
				InterruptCast(caster, true);
			}
		}
	}

	bool OnMeleeHitHook::GetEquipped(RE::Actor *a_actor, RE::TESForm *item)
	{
		auto result = false;
		if (const auto combatCtrl = a_actor->GetActorRuntimeData().combatController; combatCtrl)
		{
			if (const auto CombatInv = combatCtrl->inventory; CombatInv)
			{
				for (const auto &invent_item : CombatInv->equippedItems)
				{
					if (invent_item.item && invent_item.item.get() && invent_item.item.get()->item && invent_item.item.get()->item == item)
					{
						result = true;
						break;
					}
				}
			}
		}
		return result;
	}

	bool OnMeleeHitHook::HasSpellEquipped(RE::Actor *a_actor, bool lefthand)
	{

		bool equipped = false;

		if (const auto limbospell = a_actor->GetActorRuntimeData().currentProcess; limbospell)
		{
			if (lefthand)
			{
				if (const auto eSpell = limbospell->GetEquippedLeftHand(); eSpell && eSpell->Is(RE::FormType::Spell))
				{
					equipped = true;
				}
			}
			else
			{
				if (const auto eSpell = limbospell->GetEquippedRightHand(); eSpell && eSpell->Is(RE::FormType::Spell))
				{
					equipped = true;
				}
			}
		}

		return equipped;
	}

	bool OnMeleeHitHook::HasWeaponEquipped(RE::Actor *a_actor, bool lefthand)
	{

		bool equipped = false;

		if (const auto limbospell = a_actor->GetActorRuntimeData().currentProcess; limbospell)
		{
			if (lefthand)
			{
				if (const auto eSpell = limbospell->GetEquippedLeftHand(); eSpell && eSpell->Is(RE::FormType::Weapon))
				{
					equipped = true;
				}
			}
			else
			{
				if (const auto eSpell = limbospell->GetEquippedRightHand(); eSpell && eSpell->Is(RE::FormType::Weapon))
				{
					equipped = true;
				}
			}
		}

		return equipped;
	}

	RE::SpellItem* OnMeleeHitHook::GetEquippedSpell(RE::Actor *a_actor, bool lefthand)
	{

		RE::SpellItem *a_object = nullptr;

		if (const auto limbospell = a_actor->GetActorRuntimeData().currentProcess; limbospell)
		{
			if (lefthand)
			{
				if (const auto eSpell = limbospell->GetEquippedLeftHand(); eSpell && eSpell->Is(RE::FormType::Spell))
				{
					a_object = eSpell->As<RE::SpellItem>();
				}
			}
			else
			{
				if (const auto eSpell = limbospell->GetEquippedRightHand(); eSpell && eSpell->Is(RE::FormType::Spell))
				{
					a_object = eSpell->As<RE::SpellItem>();
				}
			}
		}

		return a_object;
	}

	bool SpellEquipManager::ShouldEquipSpell(RE::ActorEquipManager *a_this, RE::Actor *a_actor, RE::SpellItem *a_spell, const RE::BGSEquipSlot *a_slot)
	{

		bool shouldEquip = true;
		// RE::SpellItem *alt_spell = nullptr;

		if (!OnMeleeHitHook::GetBoolVariable(a_actor, "bNSV_Equip_ByPass") && OnMeleeHitHook::GetBoolVariable(a_actor, "bNSV_IsinCombat") && OnMeleeHitHook::GetBoolVariable(a_actor, "bNSV_IsMage"))
		{
			auto H = RE::TESDataHandler::GetSingleton();
			const auto lefthand = H->LookupForm<RE::BGSEquipSlot>(0x13F43, "Skyrim.esm");
			const auto righthand = H->LookupForm<RE::BGSEquipSlot>(0x13F42, "Skyrim.esm");
			// const auto eitherhand = H->LookupForm<RE::BGSEquipSlot>(0x13F44, "Skyrim.esm");
			const auto bothhands = H->LookupForm<RE::BGSEquipSlot>(0x13F45, "Skyrim.esm");

			// a_actor->HasKeywordString("NSV_SpellSword");
			// getrace_VLserana(a_actor)

			if (a_actor->HasKeywordString("NSV_SpellSword"))
			{
				if (a_slot == lefthand && ((OnMeleeHitHook::HasSpellEquipped(a_actor, true) && OnMeleeHitHook::GetBoolVariable(a_actor, "bWantCastLeft")) || (OnMeleeHitHook::HasWeaponEquipped(a_actor, false) && (a_actor->AsActorState()->GetAttackState() > RE::ATTACK_STATE_ENUM::kNone || OnMeleeHitHook::GetBoolVariable(a_actor, "IsAttacking")))))
				{
					shouldEquip = false;
				}
				else if (a_slot == righthand && ((OnMeleeHitHook::HasSpellEquipped(a_actor, false) && OnMeleeHitHook::GetBoolVariable(a_actor, "bWantCastRight")) || (OnMeleeHitHook::HasWeaponEquipped(a_actor, false) && (a_actor->AsActorState()->GetAttackState() > RE::ATTACK_STATE_ENUM::kNone || OnMeleeHitHook::GetBoolVariable(a_actor, "IsAttacking")))))
				{
					shouldEquip = false;
				}
				else if (a_slot == bothhands && ((OnMeleeHitHook::HasSpellEquipped(a_actor, false) && (OnMeleeHitHook::GetBoolVariable(a_actor, "bWantCastLeft") || OnMeleeHitHook::GetBoolVariable(a_actor, "bWantCastRight"))) || (OnMeleeHitHook::HasWeaponEquipped(a_actor, false) && (a_actor->AsActorState()->GetAttackState() > RE::ATTACK_STATE_ENUM::kNone || OnMeleeHitHook::GetBoolVariable(a_actor, "IsAttacking")))))
				{
					shouldEquip = false;
				}

			}else
			{
				if (a_slot == lefthand && OnMeleeHitHook::HasSpellEquipped(a_actor, true) && OnMeleeHitHook::GetBoolVariable(a_actor, "bWantCastLeft"))
				{
					shouldEquip = false;
				}
				else if (a_slot == righthand && OnMeleeHitHook::HasSpellEquipped(a_actor, false) && OnMeleeHitHook::GetBoolVariable(a_actor, "bWantCastRight"))
				{
					shouldEquip = false;
				}
				else if (a_slot == bothhands && OnMeleeHitHook::HasSpellEquipped(a_actor, false) && (OnMeleeHitHook::GetBoolVariable(a_actor, "bWantCastLeft") || OnMeleeHitHook::GetBoolVariable(a_actor, "bWantCastRight")))
				{
					shouldEquip = false;
				}
			}
		}
		
		return shouldEquip;
	}


	void OnMeleeHitHook::scan_activeRunes([[maybe_unused]] RE::Actor *a_actor, [[maybe_unused]] RE::Projectile *a_rune, bool insert, bool clear, bool clear_all)
	{
		// uniqueLocker lock(mtx_RunesCast);
		if (clear_all)
		{
			for (auto &it : _RunesCast)
			{
				if (!it.second.empty())
				{
					for (auto rune : it.second)
					{
						if (rune)
						{
							rune->GetProjectileRuntimeData().explosion = nullptr;
							rune->GetProjectileRuntimeData().avEffect = nullptr;
							rune->GetProjectileRuntimeData().spell = nullptr;
                            rune->GetProjectileRuntimeData().flags.set(RE::Projectile::Flags::kFading);
                            //= 1 << 5;
							rune->GetProjectileRuntimeData().transparency = 1000000.0f;
							rune->GetProjectileRuntimeData().scale = 0.0f;
							rune->GetProjectileRuntimeData().livingTime = 0.0f;
							rune = nullptr;
						}
					}
					it.second.clear();
				}
			}
			_RunesCast.clear();
			
		}else
		{
			const auto &it = _RunesCast.find(a_actor);
			if (it == _RunesCast.end())
			{
				if (insert)
				{
					std::vector<RE::Projectile *> Hen;
					_RunesCast.emplace(a_actor, Hen);
				}
			}
			else
			{
				if (insert)
				{
					if (!it->second.empty())
					{
						for (auto rune : it->second)
						{
							if (rune)
							{
								rune->GetProjectileRuntimeData().explosion = nullptr;
								rune->GetProjectileRuntimeData().avEffect = nullptr;
								rune->GetProjectileRuntimeData().spell = nullptr;
                                rune->GetProjectileRuntimeData().flags.set(RE::Projectile::Flags::kFading);
                                rune->GetProjectileRuntimeData().transparency = 1000000.0f;
								rune->GetProjectileRuntimeData().scale = 0.0f;
								rune->GetProjectileRuntimeData().livingTime = 0.0f;
								rune = nullptr;
							}
						}
						it->second.clear();
					}
					it->second.emplace_back(a_rune);
				}
				else if (clear)
				{
					if (!it->second.empty())
					{
						for (auto rune : it->second)
						{
							if (rune)
							{
								rune->GetProjectileRuntimeData().explosion = nullptr;
								rune->GetProjectileRuntimeData().avEffect = nullptr;
								rune->GetProjectileRuntimeData().spell = nullptr;
                                rune->GetProjectileRuntimeData().flags.set(RE::Projectile::Flags::kFading);
                                rune->GetProjectileRuntimeData().transparency = 1000000.0f;
								rune->GetProjectileRuntimeData().scale = 0.0f;
								rune->GetProjectileRuntimeData().livingTime = 0.0f;
								rune = nullptr;
							}
						}
						it->second.clear();
					}
					_RunesCast.erase(it);
				}
			}
		}
	}

	void hooks::OnMeleeHitHook::Cast_Rune(RE::Actor *actor, RE::SpellItem *a_spell, RE::BGSProjectile *a_projectile, bool lefthand){
		auto Rune = actor->PlaceObjectAtMe(a_projectile, false).get()->AsProjectile();
		if (Rune){
			if (!OnMeleeHitHook::GetBoolVariable(actor, "bNSV_Trap_Cool"))
			{
				actor->SetGraphVariableBool("bNSV_Trap_Cool", true);
				OnMeleeHitHook::GetSingleton()->RegisterforUpdate(actor, std::forward_as_tuple(nullptr, std::chrono::steady_clock::now(), 10000ms, "TrapCooldown_Update"));
				// if (!OnMeleeHitHook::GetBoolVariable(actor, "bNSV_Equip_OK"))
				// {
				// 	actor->SetGraphVariableBool("bNSV_Equip_OK", true);
				// }
			}
			GetSingleton()->scan_activeRunes(actor, Rune, true);
			Rune->SetActorCause(actor->GetActorCause());
			Rune->GetProjectileRuntimeData().shooter = actor;
			Rune->GetProjectileRuntimeData().spell = a_spell->As<RE::MagicItem>();
			Rune->GetProjectileRuntimeData().avEffect = a_spell->As<RE::MagicItem>()->avEffectSetting;
			//Rune->GetProjectileRuntimeData().flags = 1 << 19;
			auto combat_target = actor->GetActorRuntimeData().currentCombatTarget.get().get();
			if (combat_target){
				Rune->GetProjectileRuntimeData().desiredTarget = combat_target;
			}
			actor->AsActorValueOwner()->RestoreActorValue(RE::ActorValue::kMagicka, -(a_spell->As<RE::MagicItem>()->CalculateMagickaCost(actor)));
		}
	}

	void OnMeleeHitHook::Cast_PotentialRune(RE::Actor *actor, bool lefthand)
	{
		auto limbospell = actor->GetActorRuntimeData().currentProcess;
		const auto MagicRune = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("MagicRune");
		const auto NSV_Magic_Trap = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Trap");

		if (limbospell)
		{
			if (lefthand)
			{
				auto eSpell = limbospell->GetEquippedLeftHand();
				if (eSpell && eSpell->Is(RE::FormType::Spell))
				{
					auto rSpell = eSpell->As<RE::SpellItem>();
					if (rSpell->HasKeyword(NSV_Magic_Trap) && rSpell->GetDelivery() == RE::MagicSystem::Delivery::kTargetLocation)
					{
						auto Effect_List = rSpell->effects;
						for (auto Effect : Effect_List)
						{
							if (Effect && Effect->baseEffect)
							{
								if (Effect->baseEffect->HasKeyword(MagicRune))
								{
                                    if (GetBoolVariable(actor, "IsCastingDual"))
									{
										if (Effect->baseEffect->data.dualCastData && Effect->baseEffect->data.dualCastData->data.pProjectile)
										{
											Cast_Rune(actor, rSpell, Effect->baseEffect->data.dualCastData->data.pProjectile, lefthand);
										}else{
											if (Effect->baseEffect->data.projectileBase)
											{
												Cast_Rune(actor, rSpell, Effect->baseEffect->data.projectileBase, lefthand);
											}
										}
									}else{
										if (Effect->baseEffect->data.projectileBase)
										{
											Cast_Rune(actor, rSpell, Effect->baseEffect->data.projectileBase, lefthand);
										}
									}
									break;
								}
							}
							continue;
						}
					}
				}
			}
			else
			{
				auto eSpell = limbospell->GetEquippedRightHand();
				if (eSpell && eSpell->Is(RE::FormType::Spell))
				{
					auto rSpell = eSpell->As<RE::SpellItem>();
					if (rSpell->HasKeyword(NSV_Magic_Trap) && rSpell->GetDelivery() == RE::MagicSystem::Delivery::kTargetLocation)
					{
						auto Effect_List = rSpell->effects;
						for (auto Effect : Effect_List)
						{
							if (Effect && Effect->baseEffect)
							{
								if (Effect->baseEffect->HasKeyword(MagicRune))
								{
									if (GetBoolVariable(actor, "IsCastingDual"))
									{
										if (Effect->baseEffect->data.dualCastData && Effect->baseEffect->data.dualCastData->data.pProjectile)
										{
											Cast_Rune(actor, rSpell, Effect->baseEffect->data.dualCastData->data.pProjectile, lefthand);
										}else{
											if (Effect->baseEffect->data.projectileBase)
											{
												Cast_Rune(actor, rSpell, Effect->baseEffect->data.projectileBase, lefthand);
											}
										}
									}else{
										if (Effect->baseEffect->data.projectileBase)
										{
											Cast_Rune(actor, rSpell, Effect->baseEffect->data.projectileBase, lefthand);
										}
									}
									break;
								}
							}
							continue;
						}
					}
				}
			}
		}
	}


	void OnMeleeHitHook::Track_ConcentrationSpells(RE::Actor *actor, bool lefthand)
	{
		auto limbospell = actor->GetActorRuntimeData().currentProcess;
		const auto MagicRestoreHealth = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("MagicRestoreHealth");
		const auto NSV_Magic_Heal = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Heal");
		const auto NSV_Magic_Damage = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Damage");
		const auto MagicWard = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("MagicWard");
		const auto NSV_Magic_Ward = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Ward");
		const auto NSV_Generic_Buff = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Generic_Buff");
		const auto NSV_Generic_Debuff = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Generic_Debuff");
		const auto NSV_Tag_Ally = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Ally");
		auto caster = lefthand ? actor->GetMagicCaster(RE::MagicSystem::CastingSource::kLeftHand) : actor->GetMagicCaster(RE::MagicSystem::CastingSource::kRightHand);
		auto H = RE::TESDataHandler::GetSingleton();

		if (limbospell)
		{
			RE::TESForm *eSpell = lefthand ? limbospell->GetEquippedLeftHand() : limbospell->GetEquippedRightHand();

			if (eSpell && eSpell->Is(RE::FormType::Spell))
			{
				auto rSpell = eSpell->As<RE::SpellItem>();

				switch (rSpell->GetCastingType())
				{
				case RE::MagicSystem::CastingType::kConcentration:
					switch (rSpell->GetDelivery())
					{
					case RE::MagicSystem::Delivery::kSelf:
						if (rSpell->HasKeyword(NSV_Magic_Heal))
						{
							auto Effect_List = rSpell->effects;
							for (auto Effect : Effect_List)
							{
								if (Effect && Effect->baseEffect)
								{
									if (Effect->baseEffect->HasKeyword(MagicRestoreHealth))
									{
										auto healingInfo = rSpell->HasKeyword(NSV_Tag_Ally) ? Get_Teammate_Heal(actor, rSpell) : Get_Teammate_Heal(actor, rSpell, true);
										auto heal = healingInfo.second;

										if (heal <= 0)
										{
											heal = 1.0;
										}
										auto heal_intensity = static_cast<double>(Effect->GetMagnitude());

										if (heal_intensity <= 0)
										{
											heal_intensity = 1.0;
										}

										auto val = heal / heal_intensity;
										
										auto total = 1.0;

										if (rSpell->HasKeyword(NSV_Tag_Ally))
										{
											auto withinRange = Scan_Teammates_Nearby(actor, getSpellArea(rSpell), GetTargetVariable(rSpell));
											total = withinRange || healingInfo.first ? GetSingleton()->GenerateRandomDouble(val / 4.0, val) : 1.0;
										}
										else
										{
											total = GetSingleton()->GenerateRandomDouble(val / 4.0, val);
										}

										auto required = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::duration<double>(total));

										GetSingleton()->RegisterforUpdate(actor, std::forward_as_tuple(caster, std::chrono::steady_clock::now(), required, "HealConc_Update"));
										break;
									}
								}
								continue;
							}
						}
						else if (rSpell->HasKeyword(NSV_Magic_Ward))
						{
							auto Effect_List = rSpell->effects;
							for (auto Effect : Effect_List)
							{
								if (Effect && Effect->baseEffect)
								{
									if (Effect->baseEffect->HasKeyword(MagicWard))
									{
										auto num = 1.0;

										if (rSpell->HasKeyword(NSV_Tag_Ally))
										{
											auto withinRange = Scan_Teammates_Nearby(actor, getSpellArea(rSpell), GetTargetVariable(rSpell));
											
											if (withinRange)
											{
												num = GetSingleton()->GenerateRandomDouble(4.0, 8.0);
											}
										}
										else
										{
											num = GetSingleton()->GenerateRandomDouble(4.0, 8.0);
										}

										auto required = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::duration<double>(num));

										GetSingleton()->RegisterforUpdate(actor, std::forward_as_tuple(caster, std::chrono::steady_clock::now(), required, "WardConc_Update"));
										break;
									}
								}
								continue;
							}
						}
						else if (rSpell->hostileCount <= 0)
						{
							auto num = 1.0;

							if (rSpell->HasKeyword(NSV_Tag_Ally))
							{
								auto withinRange = Scan_Teammates_Nearby(actor, getSpellArea(rSpell), GetTargetVariable(rSpell));

								if (withinRange)
								{
									num = GetSingleton()->GenerateRandomDouble(7.0, 12.0);
								}
							}
							else
							{
								num = GetSingleton()->GenerateRandomDouble(7.0, 12.0);
							}
							auto required = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::duration<double>(num));

							GetSingleton()->RegisterforUpdate(actor, std::forward_as_tuple(caster, std::chrono::steady_clock::now(), required, "GenericConc_Update"));
						}

						break;

					default:
						break;
					}
					break;

				case RE::MagicSystem::CastingType::kFireAndForget:
					if (rSpell->HasKeyword(NSV_Generic_Buff))
					{
						if (!OnMeleeHitHook::GetBoolVariable(actor, "bNSV_Buff_Cool"))
						{
							actor->SetGraphVariableBool("bNSV_Buff_Cool", true);
							OnMeleeHitHook::GetSingleton()->RegisterforUpdate(actor, std::forward_as_tuple(nullptr, std::chrono::steady_clock::now(), 6000ms, "BuffCooldown_Update"));
							// if (!OnMeleeHitHook::GetBoolVariable(actor, "bNSV_Equip_OK"))
							// {
							// 	actor->SetGraphVariableBool("bNSV_Equip_OK", true);
							// }
						}
					}
					if (rSpell->HasKeyword(NSV_Generic_Debuff))
					{
						if (!OnMeleeHitHook::GetBoolVariable(actor, "bNSV_DeBuff_Cool"))
						{
							actor->SetGraphVariableBool("bNSV_DeBuff_Cool", true);
							OnMeleeHitHook::GetSingleton()->RegisterforUpdate(actor, std::forward_as_tuple(nullptr, std::chrono::steady_clock::now(), 6000ms, "DeBuffCooldown_Update"));
							// if (!OnMeleeHitHook::GetBoolVariable(actor, "bNSV_Equip_OK"))
							// {
							// 	actor->SetGraphVariableBool("bNSV_Equip_OK", true);
							// }
						}
					}
					break;

				default:
					break;
				}
			}
		}
	}

	void OnMeleeHitHook::GetAttackSpell(RE::Actor *a_actor, bool lefthand)
	{
		Scan_Teammates_Undead(a_actor);
		Scan_Teammates_CommandedActor(a_actor);
		Scan_Teammates_inRange(a_actor, 320.0f);

		Scan_Teammates_NeedHealing(a_actor);
		Scan_Teammates_NeedWard(a_actor);
		Scan_Teammates_NeedBoundWeapon(a_actor);
		Scan_Teammates_HasWeapon(a_actor);
		Scan_Teammates_NeedArmour(a_actor);
		Scan_Teammates_NeedCloak(a_actor);
		Scan_Teammates_NeedInvisibility(a_actor);
		Scan_Teammates_NeedSurvival(a_actor);
		
		Scan_Soul_Gems(a_actor);

		const auto limbospell = a_actor->GetActorRuntimeData().currentProcess;

		if (limbospell)
		{
			auto H = RE::TESDataHandler::GetSingleton();
			if (const auto key_list = H->LookupForm<RE::BGSListForm>(0x832, "NPC Spell Variance.esp"))
			{
				const auto NSV_Tag_Ally = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Ally");
				const auto NSV_Tag_Enemy = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Enemy");
				const auto NSV_Tag_Weapon = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Weapon");
				const auto NSV_Magic_Summon = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Summon");
				if (lefthand)
				{
					auto eSpell = limbospell->GetEquippedLeftHand();
					if (eSpell && eSpell->Is(RE::FormType::Spell))
					{
						if (auto a_spell = eSpell->As<RE::SpellItem>(); a_spell)
						{
							// logger::info("Lefthand Branch. {} began casting {}. FormID: {} EDID: {}"sv, a_actor->GetName(), a_spell->GetName(), a_spell->formID, clib_util::editorID::get_editorID(a_spell));
							
							switch (a_spell->GetCastingType())
							{
							case RE::MagicSystem::CastingType::kConcentration:
								if (NSV_Tag_Ally && NSV_Tag_Enemy && NSV_Tag_Weapon && NSV_Magic_Summon)
								{
									if (a_spell->HasKeyword(NSV_Magic_Summon) && !a_spell->HasKeyword(NSV_Tag_Weapon))
									{
										return;

									}else{
										switch (a_spell->GetDelivery())
										{
										case RE::MagicSystem::Delivery::kAimed:
											if (a_spell->HasKeywordInList(key_list, false) && a_spell->HasKeyword(NSV_Tag_Ally) && !a_spell->HasKeyword(NSV_Tag_Enemy))
											{
												Execute_Cast(a_actor, a_spell, lefthand, false, false, false, true);
											}

											break;

										case RE::MagicSystem::Delivery::kTargetActor:
											if (a_spell->HasKeywordInList(key_list, false) && a_spell->HasKeyword(NSV_Tag_Ally) && !a_spell->HasKeyword(NSV_Tag_Enemy))
											{
												Execute_Cast(a_actor, a_spell, lefthand, false, false, false, false, true);
											}

											break;

										case RE::MagicSystem::Delivery::kTargetLocation:
											if (const auto NSV_Magic_Trap = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Trap"); NSV_Magic_Trap && a_spell->HasKeyword(NSV_Magic_Trap))
											{
												PrepareRuneCast(a_actor, a_spell, lefthand);
											}
											else if (a_spell->HasKeywordInList(key_list, false) && a_spell->HasKeyword(NSV_Tag_Ally) && !a_spell->HasKeyword(NSV_Tag_Enemy))
											{
												Execute_Cast(a_actor, a_spell, lefthand, false, false, false, false, false, true);
											}

											break;

										case RE::MagicSystem::Delivery::kSelf:
											// Execute_Cast(a_actor, a_spell, lefthand, false, false, false, false, false, false, true);
											break;

										default:
											break;
										}
									}
								}

								break;

							case RE::MagicSystem::CastingType::kFireAndForget:
								if (NSV_Tag_Ally && NSV_Tag_Enemy && NSV_Tag_Weapon && NSV_Magic_Summon)
								{
									if (a_spell->HasKeyword(NSV_Magic_Summon) && !a_spell->HasKeyword(NSV_Tag_Weapon))
									{
										return;
									}
									else
									{
										switch (a_spell->GetDelivery())
										{
										case RE::MagicSystem::Delivery::kAimed:
											if (a_spell->HasKeywordInList(key_list, false) && a_spell->HasKeyword(NSV_Tag_Ally) && !a_spell->HasKeyword(NSV_Tag_Enemy))
											{
												Execute_Cast(a_actor, a_spell, lefthand, true);
											}

											break;

										case RE::MagicSystem::Delivery::kTargetActor:
											if (a_spell->HasKeywordInList(key_list, false) && a_spell->HasKeyword(NSV_Tag_Ally) && !a_spell->HasKeyword(NSV_Tag_Enemy))
											{
												Execute_Cast(a_actor, a_spell, lefthand, false, true);
											}

											break;

										case RE::MagicSystem::Delivery::kTargetLocation:
											if (const auto NSV_Magic_Trap = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Trap"); NSV_Magic_Trap && a_spell->HasKeyword(NSV_Magic_Trap))
											{
												PrepareRuneCast(a_actor, a_spell, lefthand);
											}
											else if (a_spell->HasKeywordInList(key_list, false) && a_spell->HasKeyword(NSV_Tag_Ally) && !a_spell->HasKeyword(NSV_Tag_Enemy))
											{
												Execute_Cast(a_actor, a_spell, lefthand, false, false, true);
											}

											break;

										default:
											break;
										}
									}
								}

								break;

							default:
								break;
							}
						}
					}
				}
				else
				{
					auto eSpell = limbospell->GetEquippedRightHand();
					if (eSpell && eSpell->Is(RE::FormType::Spell))
					{
						if (auto a_spell = eSpell->As<RE::SpellItem>(); a_spell)
						{
							// logger::info("Righthand Branch. {} began casting {}. FormID: {} EDID: {}"sv, a_actor->GetName(), a_spell->GetName(), a_spell->formID, clib_util::editorID::get_editorID(a_spell));

							switch (a_spell->GetCastingType())
							{
							case RE::MagicSystem::CastingType::kConcentration:
								if (NSV_Tag_Ally && NSV_Tag_Enemy && NSV_Tag_Weapon && NSV_Magic_Summon)
								{
									if (a_spell->HasKeyword(NSV_Magic_Summon) && !a_spell->HasKeyword(NSV_Tag_Weapon))
									{
										return;
									}
									else
									{
										switch (a_spell->GetDelivery())
										{
										case RE::MagicSystem::Delivery::kAimed:
											if (a_spell->HasKeywordInList(key_list, false) && a_spell->HasKeyword(NSV_Tag_Ally) && !a_spell->HasKeyword(NSV_Tag_Enemy))
											{
												Execute_Cast(a_actor, a_spell, lefthand, false, false, false, true);
											}

											break;

										case RE::MagicSystem::Delivery::kTargetActor:
											if (a_spell->HasKeywordInList(key_list, false) && a_spell->HasKeyword(NSV_Tag_Ally) && !a_spell->HasKeyword(NSV_Tag_Enemy))
											{
												Execute_Cast(a_actor, a_spell, lefthand, false, false, false, false, true);
											}

											break;

										case RE::MagicSystem::Delivery::kTargetLocation:
											if (const auto NSV_Magic_Trap = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Trap"); NSV_Magic_Trap && a_spell->HasKeyword(NSV_Magic_Trap))
											{
												PrepareRuneCast(a_actor, a_spell, lefthand);
											}
											else if (a_spell->HasKeywordInList(key_list, false) && a_spell->HasKeyword(NSV_Tag_Ally) && !a_spell->HasKeyword(NSV_Tag_Enemy))
											{
												Execute_Cast(a_actor, a_spell, lefthand, false, false, false, false, false, true);
											}

											break;

										case RE::MagicSystem::Delivery::kSelf:
											// Execute_Cast(a_actor, a_spell, lefthand, false, false, false, false, false, false, true);
											break;

										default:
											break;
										}
									}
								}
								
								break;

							case RE::MagicSystem::CastingType::kFireAndForget:
								if (NSV_Tag_Ally && NSV_Tag_Enemy && NSV_Tag_Weapon && NSV_Magic_Summon)
								{
									if (a_spell->HasKeyword(NSV_Magic_Summon) && !a_spell->HasKeyword(NSV_Tag_Weapon))
									{
										return;
									}
									else
									{
										switch (a_spell->GetDelivery())
										{
										case RE::MagicSystem::Delivery::kAimed:
											if (a_spell->HasKeywordInList(key_list, false) && a_spell->HasKeyword(NSV_Tag_Ally) && !a_spell->HasKeyword(NSV_Tag_Enemy))
											{
												Execute_Cast(a_actor, a_spell, lefthand, true);
											}

											break;

										case RE::MagicSystem::Delivery::kTargetActor:
											if (a_spell->HasKeywordInList(key_list, false) && a_spell->HasKeyword(NSV_Tag_Ally) && !a_spell->HasKeyword(NSV_Tag_Enemy))
											{
												Execute_Cast(a_actor, a_spell, lefthand, false, true);
											}

											break;

										case RE::MagicSystem::Delivery::kTargetLocation:
											if (const auto NSV_Magic_Trap = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Trap"); NSV_Magic_Trap && a_spell->HasKeyword(NSV_Magic_Trap))
											{
												PrepareRuneCast(a_actor, a_spell, lefthand);
											}
											else if (a_spell->HasKeywordInList(key_list, false) && a_spell->HasKeyword(NSV_Tag_Ally) && !a_spell->HasKeyword(NSV_Tag_Enemy))
											{
												Execute_Cast(a_actor, a_spell, lefthand, false, false, true);
											}

											break;

										default:
											break;
										}
									}
								}
								
								break;

							default:
								break;
							}
						}
					}
				}
			}
		}
	}

	void OnMeleeHitHook::Toggle_Casting(RE::Actor *actor, bool a_enable, bool a_lefthand)
	{
		if (a_enable)
		{
			if (!GetBoolVariable(actor, "CastOK"))
			{
				actor->SetGraphVariableBool("CastOK", true);
			}
			if (!GetBoolVariable(actor, "bCastReady"))
			{
				actor->SetGraphVariableBool("bCastReady", true);
			}
			if(a_lefthand){
				if (!GetBoolVariable(actor, "bMLh_Ready"))
				{
					actor->SetGraphVariableBool("bMLh_Ready", true);
				}
			}else{
				if (!GetBoolVariable(actor, "bMRh_Ready"))
				{
					actor->SetGraphVariableBool("bMRh_Ready", true);
				}
			}
		}
		else
		{
			if (GetBoolVariable(actor, "CastOK"))
			{
				actor->SetGraphVariableBool("CastOK", false);
			}
			if (GetBoolVariable(actor, "bCastReady"))
			{
				actor->SetGraphVariableBool("bCastReady", false);
			}
			if (a_lefthand)
			{
				if (GetBoolVariable(actor, "bMLh_Ready"))
				{
					actor->SetGraphVariableBool("bMLh_Ready", false);
				}
			}else{
				if (GetBoolVariable(actor, "bMRh_Ready"))
				{
					actor->SetGraphVariableBool("bMRh_Ready", false);
				}
			}
		}
	}

	void OnMeleeHitHook::Toggle_Casting_LE(RE::Actor *actor, bool a_enable)
	{
		if (a_enable)
		{
			if (!GetBoolVariable(actor, "CastOK"))
			{
				actor->SetGraphVariableBool("CastOK", true);
			}
			if (!GetBoolVariable(actor, "bEquipOK"))
			{
				actor->SetGraphVariableBool("bEquipOK", true);
			}
			if (!GetBoolVariable(actor, "bCastReady"))
			{
				actor->SetGraphVariableBool("bCastReady", true);
			}
			if (!GetBoolVariable(actor, "bMLh_Ready"))
			{
				actor->SetGraphVariableBool("bMLh_Ready", true);
			}
			if (!GetBoolVariable(actor, "bMRh_Ready"))
			{
				actor->SetGraphVariableBool("bMRh_Ready", true);
			}
		}
		else
		{
			if (GetBoolVariable(actor, "CastOK"))
			{
				actor->SetGraphVariableBool("CastOK", false);
			}
			if (GetBoolVariable(actor, "bEquipOK"))
			{
				actor->SetGraphVariableBool("bEquipOK", false);
			}
			if (GetBoolVariable(actor, "bCastReady"))
			{
				actor->SetGraphVariableBool("bCastReady", false);
			}
			if (GetBoolVariable(actor, "bMLh_Ready"))
			{
				actor->SetGraphVariableBool("bMLh_Ready", false);
			}
			if (GetBoolVariable(actor, "bMRh_Ready"))
			{
				actor->SetGraphVariableBool("bMRh_Ready", false);
			}
		}
	}

	void OnMeleeHitHook::CheckCast(RE::Actor *a_actor, RE::MagicCaster * &a_caster, RE::MagicItem *a_spell, RE::BGSKeyword *P_Key, RE::BGSKeyword *S_key)
	{

		if(P_Key && S_key)
		{
			switch (a_spell->GetDelivery())
			{
			case RE::MagicSystem::Delivery::kSelf:
				if (a_spell->HasKeyword(P_Key))
				{
					auto Effect_List = a_spell->effects;
					for (auto Effect : Effect_List)
					{
						if (Effect && Effect->baseEffect)
						{
							if (Effect->baseEffect->HasKeyword(S_key))
							{
								// logger::info("Heal Branch. Interrupting cast of {} for {}"sv, a_spell->GetName(), a_actor->GetName());
								Vary_AI_Scores(a_actor);
								InterruptCast(a_caster, false);
								// RE::ActorEquipManager::GetSingleton()->UnequipObject(a_actor, a_spell);
								break;
							}
						}
						continue;
					}
				}

				break;

			case RE::MagicSystem::Delivery::kAimed:
			case RE::MagicSystem::Delivery::kTargetActor:

				if (a_spell->HasKeyword(P_Key))
				{
					auto Effect_List = a_spell->effects;
					for (auto Effect : Effect_List)
					{
						if (Effect && Effect->baseEffect)
						{
							if (Effect->baseEffect->HasKeyword(S_key))
							{
								// logger::info("Heal Branch. Interrupting cast of {} for {}"sv, a_spell->GetName(), a_actor->GetName());
								Vary_AI_Scores(a_actor);
								InterruptCast(a_caster, false);
								// RE::ActorEquipManager::GetSingleton()->UnequipObject(a_actor, a_spell);
								break;
							}
						}
						continue;
					}
				}
				break;

			default:
				break;
			}
		}else
		{
			switch (a_spell->GetDelivery())
			{
			case RE::MagicSystem::Delivery::kSelf:

				if (a_spell->hostileCount <= 0)
				{
					// logger::info("Non Hostile Branch. Interrupting cast of {} for {}"sv, rSpell->GetName(), a_actor->GetName());
					Vary_AI_Scores(a_actor);
					InterruptCast(a_caster, false);
					// RE::ActorEquipManager::GetSingleton()->UnequipObject(a_actor, rSpell);
				}
				break;

			case RE::MagicSystem::Delivery::kAimed:
			case RE::MagicSystem::Delivery::kTargetActor:

				if (a_spell->hostileCount <= 1)
				{
					// logger::info("Non Hostile Branch. Interrupting cast of {} for {}"sv, rSpell->GetName(), a_actor->GetName());
					Vary_AI_Scores(a_actor);
					InterruptCast(a_caster, false);
					// RE::ActorEquipManager::GetSingleton()->UnequipObject(a_actor, rSpell);
				}
				break;

			default:
				break;
			}
		}
	}

	void OnMeleeHitHook::GetEquippedSpell(RE::Actor *a_actor, RE::MagicCaster * &a_caster, RE::MagicItem *a_spell, std::string function)
	{
		// auto limbospell = a_actor->GetActorRuntimeData().currentProcess;
		const auto MagicRestoreHealth = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("MagicRestoreHealth");
		const auto NSV_Magic_Heal = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Heal");
		const auto MagicWard = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("MagicWard");
		const auto NSV_Magic_Ward = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Ward");
		// auto H = RE::TESDataHandler::GetSingleton();

		switch (hash(function.c_str(), function.size()))
		{
		case "HealConc_Update"_h:
			CheckCast(a_actor, a_caster, a_spell, NSV_Magic_Heal, MagicRestoreHealth);
			break;

		case "WardConc_Update"_h:
			CheckCast(a_actor, a_caster, a_spell, NSV_Magic_Ward, MagicWard);
			break;

		case "GenericConc_Update"_h:
			CheckCast(a_actor, a_caster, a_spell);
			break;

		default:
			break;
		}

	}

	bool GetshouldHelp(const RE::Actor *p_ally, const RE::Actor *a_actor)
	{
		static RE::TESConditionItem cond;
		static std::once_flag flag;
		std::call_once(flag, [&]()
					   {
        cond.data.functionData.function = RE::FUNCTION_DATA::FunctionID::kGetShouldHelp;
        cond.data.flags.opCode          = RE::CONDITION_ITEM_DATA::OpCode::kEqualTo;
        cond.data.comparisonValue.f     = 1.0f; });

		ConditionParam cond_param;
		cond_param.form = const_cast<RE::TESObjectREFR *>(a_actor->As<RE::TESObjectREFR>());
		cond.data.functionData.params[0] = std::bit_cast<void *>(cond_param);

		RE::ConditionCheckParams params(const_cast<RE::TESObjectREFR *>(p_ally->As<RE::TESObjectREFR>()),
										const_cast<RE::TESObjectREFR *>(a_actor->As<RE::TESObjectREFR>()));
		return cond(params);
	}

	void StartCombat(const RE::Actor *subject, const RE::Actor *target)
	{
		static RE::TESConditionItem cond;
		static std::once_flag flag;
		std::call_once(flag, [&]()
					   {
        cond.data.functionData.function = RE::FUNCTION_DATA::FunctionID::kStartCombat;});

		RE::ConditionCheckParams params(const_cast<RE::TESObjectREFR *>(subject->As<RE::TESObjectREFR>()),
										const_cast<RE::TESObjectREFR *>(target->As<RE::TESObjectREFR>()));
		//return cond(params);
		params.unk20;
	}

	RE::BSEventNotifyControl animEventHandler::HookedProcessEvent(RE::BSAnimationGraphEvent& a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent>* src)
	{
		FnProcessEvent fn = fnHash.at(*(uint64_t*)this);

		if (!a_event.holder) {
			return fn ? (this->*fn)(a_event, src) : RE::BSEventNotifyControl::kContinue;
		}

		RE::Actor* actor = const_cast<RE::TESObjectREFR*>(a_event.holder)->As<RE::Actor>();
		switch (hash(a_event.tag.c_str(), a_event.tag.size())) {
		case "BeginCastLeft"_h:
		    if (!actor->IsPlayerRef()){
				if (OnMeleeHitHook::GetBoolVariable(actor, "bNSV_IsinCombat"))
				{
					OnMeleeHitHook::GetAttackSpell(actor, true);
				}
			}
			break;

		case "BeginCastRight"_h:
			if (!actor->IsPlayerRef())
			{
				if (OnMeleeHitHook::GetBoolVariable(actor, "bNSV_IsinCombat"))
				{
					OnMeleeHitHook::GetAttackSpell(actor);

					// if (!(actor->HasKeywordString("TLH_Harkon_Key") || OnMeleeHitHook::getrace_VLserana(actor)))
					// {
					// 	OnMeleeHitHook::Toggle_Casting(actor, true);
					// }
				}
			}
			break;

		case "MLh_SpellFire_Event"_h:
			if (!actor->IsPlayerRef())
			{
				if (OnMeleeHitHook::GetBoolVariable(actor, "bNSV_IsinCombat"))
				{
					OnMeleeHitHook::Cast_PotentialRune(actor, true);

					if (actor->HasKeywordString("TLH_Harkon_Key"))
					{
						if (OnMeleeHitHook::GetFloatVariable(actor, "fTLH_HarkonBossBattleState") != 4)
						{
							OnMeleeHitHook::Track_ConcentrationSpells(actor, true);
						}
					}else{
						OnMeleeHitHook::Track_ConcentrationSpells(actor, true);
					}
					
					OnMeleeHitHook::Vary_AI_Scores(actor);

					// if (auto combatcontrol = actor->GetActorRuntimeData().combatController; combatcontrol)
					// {
					// 	OnMeleeHitHook::Check_CombatInventory(combatcontrol);
					// }
				}
			}
			break;

		case "MRh_SpellFire_Event"_h:
			if (!actor->IsPlayerRef())
			{
				if (OnMeleeHitHook::GetBoolVariable(actor, "bNSV_IsinCombat"))
				{

					OnMeleeHitHook::Cast_PotentialRune(actor);

					if (actor->HasKeywordString("TLH_Harkon_Key"))
					{
						if (OnMeleeHitHook::GetFloatVariable(actor, "fTLH_HarkonBossBattleState") != 4)
						{
							OnMeleeHitHook::Track_ConcentrationSpells(actor);
						}
					}
					else
					{
						OnMeleeHitHook::Track_ConcentrationSpells(actor);
					}

					OnMeleeHitHook::Vary_AI_Scores(actor);

					// if (OnMeleeHitHook::GetBoolVariable(actor, "bNSV_IsMage"))
					// {
					// 	if (auto combatcontrol = actor->GetActorRuntimeData().combatController; combatcontrol)
					// 	{
					// 		OnMeleeHitHook::Check_CombatInventory(combatcontrol);
					// 	}
					// }
				}
			}
			break;

		case "InterruptCast"_h:
			if (!actor->IsPlayerRef())
			{
				// if (OnMeleeHitHook::GetBoolVariable(actor, "bNSV_IsinCombat") && OnMeleeHitHook::GetBoolVariable(actor, "bNSV_IsMage"))
				// {
				// 	actor->SetGraphVariableBool("bCastReady", false);
				// 	actor->AsActorValueOwner()->RestoreActorValue(RE::ACTOR_VALUE_MODIFIER::kDamage, RE::ActorValue::kMagicka, 5.0f);
				// 	actor->SetGraphVariableBool("bCastReady", true);
				// }
			}
			break;
			
		case "CastStop"_h:
			if (!actor->IsPlayerRef())
			{
				if (OnMeleeHitHook::GetBoolVariable(actor, "bNSV_IsinCombat"))
				{
					
				}
			}
			break;

		default:
			break;
		}

		return fn ? (this->*fn)(a_event, src) : RE::BSEventNotifyControl::kContinue;
	}

	std::unordered_map<uint64_t, animEventHandler::FnProcessEvent> animEventHandler::fnHash;

	void OnMeleeHitHook::install(){

		auto eventSink = OurEventSink::GetSingleton();

		// ScriptSource
		auto* eventSourceHolder = RE::ScriptEventSourceHolder::GetSingleton();
		// eventSourceHolder->AddEventSink<RE::TESSwitchRaceCompleteEvent>(eventSink);
		// eventSourceHolder->AddEventSink<RE::TESEquipEvent>(eventSink);
		eventSourceHolder->AddEventSink<RE::TESCombatEvent>(eventSink);
		//eventSourceHolder->AddEventSink<RE::TESActorLocationChangeEvent>(eventSink);
		//eventSourceHolder->AddEventSink<RE::TESSpellCastEvent>(eventSink);
		eventSourceHolder->AddEventSink<RE::TESDeathEvent>(eventSink);

		// eventSourceHolder->AddEventSink<RE::TESCellFullyLoadedEvent>(eventSink);

		eventSourceHolder->AddEventSink<RE::TESTopicInfoEvent>(eventSink);

		auto ui = RE::UI::GetSingleton();
		ui->AddEventSink<RE::MenuOpenCloseEvent>(eventSink);
	}


	void OnMeleeHitHook::install_pluginListener(){
		auto eventSink = OurEventSink::GetSingleton();
		SKSE::GetModCallbackEventSource()->AddEventSink(eventSink);
	}

	InputEventHandler* InputEventHandler::GetSingleton()
	{
		static InputEventHandler singleton;
		return std::addressof(singleton);
	}

	RE::BSEventNotifyControl InputEventHandler::ProcessEvent(RE::InputEvent* const* a_event, [[maybe_unused]] RE::BSTEventSource<RE::InputEvent*>* a_eventSource)
	{
		// using EventType = RE::INPUT_EVENT_TYPE;
		// using DeviceType = RE::INPUT_DEVICE;

		if (!a_event) {
			return RE::BSEventNotifyControl::kContinue;
		}

		// for (auto event = *a_event; event; event = event->next) {
		// 	if (event->eventType != EventType::kButton) {
		// 		continue;
		// 	}

		// 	auto button = static_cast<RE::ButtonEvent*>(event);
		// 	if (!button) {
		// 		continue;
		// 	}
		// 	auto key = button->idCode;
		// 	if (!key) {
		// 		continue;
		// 	}

		// 	switch (button->device.get()) {
		// 	case DeviceType::kMouse:
		// 		key += kMouseOffset;
		// 		break;
		// 	case DeviceType::kKeyboard:
		// 		key += kKeyboardOffset;
		// 		break;
		// 	case DeviceType::kGamepad:
		// 		key = GetGamepadIndex((RE::BSWin32GamepadDevice::Key)key);
		// 		break;
		// 	default:
		// 		continue;
		// 	}

		// 	if (key == 274)
		// 	{
		// 		auto Playerhandle = RE::PlayerCharacter::GetSingleton();
		// 		if (button->IsDown() || button->IsHeld() || button->IsPressed()){
		// 			Playerhandle->SetGraphVariableBool("bPSV_Toggle_PowerSprintAttack", true);
		// 		}
		// 		if(button->IsUp()){
		// 			Playerhandle->SetGraphVariableBool("bPSV_Toggle_PowerSprintAttack", false);
		// 		}
		// 		break;
		// 	}
		// }

		return RE::BSEventNotifyControl::kContinue;
	}

	std::uint32_t InputEventHandler::GetGamepadIndex(RE::BSWin32GamepadDevice::Key a_key)
	{
		using Key = RE::BSWin32GamepadDevice::Key;

		std::uint32_t index;
		switch (a_key) {
		case Key::kUp:
			index = 0;
			break;
		case Key::kDown:
			index = 1;
			break;
		case Key::kLeft:
			index = 2;
			break;
		case Key::kRight:
			index = 3;
			break;
		case Key::kStart:
			index = 4;
			break;
		case Key::kBack:
			index = 5;
			break;
		case Key::kLeftThumb:
			index = 6;
			break;
		case Key::kRightThumb:
			index = 7;
			break;
		case Key::kLeftShoulder:
			index = 8;
			break;
		case Key::kRightShoulder:
			index = 9;
			break;
		case Key::kA:
			index = 10;
			break;
		case Key::kB:
			index = 11;
			break;
		case Key::kX:
			index = 12;
			break;
		case Key::kY:
			index = 13;
			break;
		case Key::kLeftTrigger:
			index = 14;
			break;
		case Key::kRightTrigger:
			index = 15;
			break;
		default:
			index = kInvalid;
			break;
		}

		return index != kInvalid ? index + kGamepadOffset : kInvalid;
	}

	void InputEventHandler::SinkEventHandlers()
	{
		auto deviceManager = RE::BSInputDeviceManager::GetSingleton();
		deviceManager->AddEventSink(InputEventHandler::GetSingleton());
		logger::info("Added input event sink");
	}

	bool OnMeleeHitHook::BindPapyrusFunctions(VM* vm)
	{
		//vm->RegisterFunction("XXXX", "XXXXX", XXXX);
		return true;
	}

	int OnMeleeHitHook::GenerateRandomInt(int value_a, int value_b)
	{
		std::mt19937 generator(rd());
		std::uniform_int_distribution<int> dist(value_a, value_b);
		return dist(generator);
	}

	float OnMeleeHitHook::GenerateRandomFloat(float value_a, float value_b)
	{
		std::mt19937 generator(rd());
		std::uniform_real_distribution<float> dist(value_a, value_b);
		return dist(generator);
	}
	double OnMeleeHitHook::GenerateRandomDouble(double value_a, double value_b)
	{
		std::mt19937 generator(rd());
		std::uniform_real_distribution<double> dist(value_a, value_b);
		return dist(generator);
	}

	void OnMeleeHitHook::RegisterforUpdate(RE::Actor *a_actor, std::tuple<RE::MagicCaster *, std::chrono::steady_clock::time_point, std::chrono::milliseconds, std::string> data)
	{
		auto itt = _Timer.find(a_actor);
		if (itt == _Timer.end())
		{
			std::vector<std::tuple<RE::MagicCaster *, std::chrono::steady_clock::time_point, std::chrono::milliseconds, std::string>> Hen;
			Hen.emplace_back(data);
			_Timer.emplace(a_actor, Hen);
		}
		else
		{
			itt->second.emplace_back(data);
		}
	}

	void OnMeleeHitHook::Mod_CombatInventory_Weapons_Reset(RE::Actor *a_actor, RE::CombatController *a_controller)
	{
		auto H = RE::TESDataHandler::GetSingleton();
		const auto righthand = H->LookupForm<RE::BGSEquipSlot>(0x13F42, "Skyrim.esm");

		if (auto a_invent = a_controller->inventory; a_invent && righthand)
		{
			for (auto invent_item : *a_invent->inventoryItems)
			{
				if (invent_item && invent_item.get() && invent_item.get()->item && invent_item.get()->itemScore && invent_item.get()->item->IsWeapon() && invent_item.get()->GetType() 
				== RE::CombatInventoryItem::TYPE::kMelee && invent_item.get()->itemSlot.equipSlot && invent_item.get()->itemSlot.equipSlot 
				== righthand && invent_item.get()->itemSlot.slot && invent_item.get()->itemSlot.slot == 1)
				{
					if (invent_item.get()->itemScore >= 500000)
					{
						invent_item.get()->itemScore -= 500000;
					}
				}
			}
		}
	}

	void OnMeleeHitHook::Mod_CombatInventory_Weapons(RE::Actor *a_actor, RE::CombatController *a_controller)
	{
		auto H = RE::TESDataHandler::GetSingleton();
		const auto righthand = H->LookupForm<RE::BGSEquipSlot>(0x13F42, "Skyrim.esm");

		if (auto a_invent = a_controller->inventory; a_invent && righthand)
		{
			float score = 0;
			bool modded = false;
			int counter = 0;
			float highest_score = 0;
			RE::NiPointer<RE::CombatInventoryItem> highest_position = nullptr;
			RE::NiPointer<RE::CombatInventoryItem> position = nullptr;
			std::vector<RE::NiPointer<RE::CombatInventoryItem>> modded_store;

			for (auto invent_item : *a_invent->inventoryItems)
			{
				if (invent_item && invent_item.get() && invent_item.get()->item && invent_item.get()->itemScore && invent_item.get()->item->IsWeapon() && invent_item.get()->GetType() 
				== RE::CombatInventoryItem::TYPE::kMelee && invent_item.get()->itemSlot.equipSlot && invent_item.get()->itemSlot.equipSlot 
				== righthand && invent_item.get()->itemSlot.slot && invent_item.get()->itemSlot.slot == 1)
				{
					if (invent_item.get()->itemScore >= 500000)
					{
						if (!modded)
						{
							modded = true;
						}
						if (invent_item.get()->itemScore > highest_score)
						{
							highest_score = invent_item.get()->itemScore;
							highest_position = invent_item;
						}
						counter++;
						modded_store.emplace_back(invent_item);
						
					}else
					{

						if (invent_item.get()->itemScore > score)
						{
							score = invent_item.get()->itemScore;
							position = invent_item;
						}
					}
				}
			}

			if(modded)
			{
				if (counter > 1)
				{
					for (auto &a_modded_store : modded_store)
					{
						if (a_modded_store && a_modded_store.get() && highest_position && highest_position.get())
						{
							if (a_modded_store == highest_position)
							{
								continue;

							}else
							{
								a_modded_store.get()->itemScore -= 500000;
							}
						}
					}
				}else
				{
					return;
				}
			}
			else if (position && position.get() && score > 0)
			{
				position.get()->itemScore += 500000;
			}
		}
	}

	void OnMeleeHitHook::Mod_CombatInventory(RE::Actor *a_actor, RE::CombatController *a_controller)
	{
		auto H = RE::TESDataHandler::GetSingleton();
		const auto NSV_Keywords = H->LookupForm<RE::BGSListForm>(0x996, "NPC Spell Variance.esp");
		const auto eitherhand = H->LookupForm<RE::BGSEquipSlot>(0x13F44, "Skyrim.esm");
		const auto lefthand = H->LookupForm<RE::BGSEquipSlot>(0x13F43, "Skyrim.esm");
		const auto righthand = H->LookupForm<RE::BGSEquipSlot>(0x13F42, "Skyrim.esm");
		const auto Mod_Invent_Left = H->LookupForm<RE::BGSListForm>(0xB02, "NPC Spell Variance.esp");
		const auto Mod_Invent_Right = H->LookupForm<RE::BGSListForm>(0xB03, "NPC Spell Variance.esp");
		const auto Mod_Invent_SS_VL = H->LookupForm<RE::BGSListForm>(0xB04, "NPC Spell Variance.esp");
		const auto NSV_Support_Keywords = H->LookupForm<RE::BGSListForm>(0xB07, "NPC Spell Variance.esp");
		const auto NSV_Healer_Keywords = H->LookupForm<RE::BGSListForm>(0xB08, "NPC Spell Variance.esp");

		if (auto a_invent = a_controller->inventory; a_invent && NSV_Keywords && eitherhand && lefthand && righthand && Mod_Invent_Left && Mod_Invent_Right && Mod_Invent_SS_VL && NSV_Support_Keywords && NSV_Healer_Keywords)
		{
			for (auto invent_item : *a_invent->inventoryItems)
			{
				if (invent_item && invent_item.get() && invent_item.get()->item && invent_item.get()->item->IsMagicItem() && invent_item.get()->item->Is(RE::FormType::Spell))
				{
					int counter = 0;
					RE::NiPointer<RE::CombatInventoryItem> position1 = nullptr;
					RE::NiPointer<RE::CombatInventoryItem> position2 = nullptr;

					for (auto alt_item : *a_invent->inventoryItems)
					{
						if (alt_item && alt_item.get() && alt_item.get()->item && alt_item.get()->item == invent_item.get()->item)
						{
							counter++;
							switch (counter)
							{
							case 1:
								position1 = alt_item;
								break;

							case 2:
								position2 = alt_item;
								break;
							}
						}
						if(counter > 2){
							break;
						}
					}

					if (counter <= 1)
					{
						if (!position1 || !position1.get())
						{
							continue;
						}
						else if (!position2 && position1 && position1.get())
						{
							if (position1.get()->item->As<RE::SpellItem>() && position1.get()->item->As<RE::SpellItem>()->equipSlot && position1.get()->item->As<RE::SpellItem>()->equipSlot == eitherhand)
							{

								if (position1.get()->item->HasKeywordInList(Mod_Invent_SS_VL, false))
								{
									// do nothing to ascendant spells
								}
								else
								{
									//(position1.get()->itemSlot.equipSlot == position2.get()->itemSlot.equipSlot)

									if (position1.get()->item->HasKeywordInList(Mod_Invent_Left, false))
									{
										bool skip = false;
										if (a_actor->HasKeywordString("NSV_Support"))
										{
											if (position1.get()->item->HasKeywordInList(NSV_Support_Keywords, false))
											{
												skip = true;
											}
										}
										else if (a_actor->HasKeywordString("NSV_Healer"))
										{
											if (position1.get()->item->HasKeywordInList(NSV_Healer_Keywords, true))
											{
												skip = true;
											}
										}

										if(!skip)
										{
											if (position1.get()->itemSlot.slot != 2)
											{
												position1.get()->itemSlot.slot = 2;
											}

											if (position1.get()->itemSlot.equipSlot != lefthand)
											{
												position1.get()->itemSlot.equipSlot = lefthand;
											}
										}
									}
									else if (position1.get()->item->HasKeywordInList(Mod_Invent_Right, false))
									{
										bool skip = false;
										if (a_actor->HasKeywordString("NSV_Support"))
										{
											if (position1.get()->item->HasKeywordInList(NSV_Support_Keywords, false))
											{
												skip = true;
											}
										}
										else if (a_actor->HasKeywordString("NSV_Healer"))
										{
											if (position1.get()->item->HasKeywordInList(NSV_Healer_Keywords, true))
											{
												skip = true;
											}
										}

										if (!skip)
										{
											if (position1.get()->itemSlot.slot != 1)
											{
												position1.get()->itemSlot.slot = 1;
											}

											if (position1.get()->itemSlot.equipSlot != righthand)
											{
												position1.get()->itemSlot.equipSlot = righthand;
											}
										}
									}
									else
									{

										if (position1.get()->item->HasKeywordInList(NSV_Keywords, false))
										{
											if (a_actor->HasKeywordString("NSV_SpellSword"))
											{
												position1.get()->itemSlot.slot = 2;

												position1.get()->itemSlot.equipSlot = lefthand;
											}
										}
									}
								}
							}
						}

					}else
					{
						if (!position1 || !position2 || !position1.get() || !position2.get())
						{
							continue;
						}
						else if (position1 && position2 && position1.get() && position2.get())
						{

							if (position1.get()->item->HasKeywordInList(Mod_Invent_SS_VL, false) || position2.get()->item->HasKeywordInList(Mod_Invent_SS_VL, false))
							{
								// do nothing to ascendant spells
							}
							else
							{
								//(position1.get()->itemSlot.equipSlot == position2.get()->itemSlot.equipSlot)

								if (position1.get()->item->HasKeywordInList(Mod_Invent_Left, false) || position2.get()->item->HasKeywordInList(Mod_Invent_Left, false))
								{
									bool skip = false;
									if (a_actor->HasKeywordString("NSV_Support"))
									{
										if (position1.get()->item->HasKeywordInList(NSV_Support_Keywords, false) || position2.get()->item->HasKeywordInList(NSV_Support_Keywords, false))
										{
											skip = true;
										}
									}
									else if (a_actor->HasKeywordString("NSV_Healer"))
									{
										if (position1.get()->item->HasKeywordInList(NSV_Healer_Keywords, true) || position2.get()->item->HasKeywordInList(NSV_Healer_Keywords, true))
										{
											skip = true;
										}
									}
							

									if(!skip)
									{
										position1.get()->itemSlot.slot = 2;
										position2.get()->itemSlot.slot = 2;

										position1.get()->itemSlot.equipSlot = lefthand;
										position2.get()->itemSlot.equipSlot = lefthand;

										a_invent->inventoryItems->erase(std::find(a_invent->inventoryItems->begin(), a_invent->inventoryItems->end(), position2));
									}
				
								}
								else if (position1.get()->item->HasKeywordInList(Mod_Invent_Right, false) || position2.get()->item->HasKeywordInList(Mod_Invent_Right, false))
								{
									bool skip = false;
									if (a_actor->HasKeywordString("NSV_Support"))
									{
										if (position1.get()->item->HasKeywordInList(NSV_Support_Keywords, false) || position2.get()->item->HasKeywordInList(NSV_Support_Keywords, false))
										{
											skip = true;
										}
									}
									else if (a_actor->HasKeywordString("NSV_Healer"))
									{
										if (position1.get()->item->HasKeywordInList(NSV_Healer_Keywords, true) || position2.get()->item->HasKeywordInList(NSV_Healer_Keywords, true))
										{
											skip = true;
										}
									}

									if (!skip)
									{
										position1.get()->itemSlot.slot = 1;
										position2.get()->itemSlot.slot = 1;

										position1.get()->itemSlot.equipSlot = righthand;
										position2.get()->itemSlot.equipSlot = righthand;

										a_invent->inventoryItems->erase(std::find(a_invent->inventoryItems->begin(), a_invent->inventoryItems->end(), position2));
									}

								}
								else if (position1.get()->item->HasKeywordInList(NSV_Keywords, false) || position2.get()->item->HasKeywordInList(NSV_Keywords, false))
								{
									if (a_actor->HasKeywordString("NSV_SpellSword"))
									{
										position1.get()->itemSlot.slot = 2;
										position2.get()->itemSlot.slot = 2;

										position1.get()->itemSlot.equipSlot = lefthand;
										position2.get()->itemSlot.equipSlot = lefthand;

										a_invent->inventoryItems->erase(std::find(a_invent->inventoryItems->begin(), a_invent->inventoryItems->end(), position2));
									}
									else
									{
										bool skip = false;
										if (a_actor->HasKeywordString("NSV_Support"))
										{
											if (position1.get()->item->HasKeywordInList(NSV_Support_Keywords, false) || position2.get()->item->HasKeywordInList(NSV_Support_Keywords, false))
											{
												skip = true;
											}
										}
										else if (a_actor->HasKeywordString("NSV_Healer"))
										{
											if (position1.get()->item->HasKeywordInList(NSV_Healer_Keywords, true) || position2.get()->item->HasKeywordInList(NSV_Healer_Keywords, true))
											{
												skip = true;
											}
										}

										if (!skip)
										{
											switch (GetSingleton()->GenerateRandomInt(1, 2))
											{
											case 1:
												position1.get()->itemSlot.slot = 1;
												position2.get()->itemSlot.slot = 1;

												position1.get()->itemSlot.equipSlot = righthand;
												position2.get()->itemSlot.equipSlot = righthand;

												a_invent->inventoryItems->erase(std::find(a_invent->inventoryItems->begin(), a_invent->inventoryItems->end(), position2));
												break;

											case 2:
												position1.get()->itemSlot.slot = 2;
												position2.get()->itemSlot.slot = 2;

												position1.get()->itemSlot.equipSlot = lefthand;
												position2.get()->itemSlot.equipSlot = lefthand;

												a_invent->inventoryItems->erase(std::find(a_invent->inventoryItems->begin(), a_invent->inventoryItems->end(), position2));
												break;
											}
										}

									}
								}

								// a_invent->inventoryItems->emplace_back(position2);
							}
						}
					}
				}
			}
		}
	}

	void OnMeleeHitHook::Mod_CombatInventory_VaryScores(RE::Actor *a_actor, RE::CombatController *a_controller){
		
		auto H = RE::TESDataHandler::GetSingleton();
		const auto NSV_Keywords = H->LookupForm<RE::BGSListForm>(0x996, "NPC Spell Variance.esp");

		if (auto a_invent = a_controller->inventory; a_invent && NSV_Keywords)
		{
			for (auto invent_item : *a_invent->inventoryItems)
			{
				if (invent_item && invent_item.get() && invent_item.get()->item && invent_item.get()->item->IsMagicItem() && invent_item.get()->item->Is(RE::FormType::Spell))
				{
					if (invent_item.get()->item->HasKeywordInList(NSV_Keywords, false) && invent_item.get()->itemScore)
					{
						if (invent_item.get()->itemScore > 250)
						{
							switch (GetSingleton()->GenerateRandomInt(1, 2))
							{
							case 1:
								invent_item.get()->itemScore += 150;
								break;

							case 2:
								invent_item.get()->itemScore -= 150;
								break;
							}
						}
						else
						{
							switch (GetSingleton()->GenerateRandomInt(1, 2))
							{
							case 1:
								invent_item.get()->itemScore += 150;
								break;

							case 2:

								break;
							}
						}
					}
				}
			}
		}
	}

    void OnMeleeHitHook::Check_CombatInventory(RE::Actor* a_actor, RE::CombatController* a_controller) {
        if (auto a_invent = a_controller->inventory; a_invent)
		{
			for (auto invent_item : *a_invent->inventoryItems)
			{
				if (invent_item && invent_item.get() && invent_item.get()->item)
				{
					logger::info("actor: {} item: {} formID: {} score: {:.2f} equipSlot: {} equipUnsigned: {}"sv, a_actor->GetName(), invent_item.get()->item->GetName(), invent_item.get()->item->formID, invent_item.get()->itemScore, clib_util::editorID::get_editorID(invent_item.get()->itemSlot.equipSlot), invent_item.get()->itemSlot.slot);
				}
			}

			// for (const auto equip_item : a_invent->unk118.items)
			// {
			// 	if (equip_item && equip_item.get())
			// 	{
			// 		logger::info("actor: {} itemSet1: {} scoreSet1: {:.2f} slotSet1: {} equipSlot1: {} equipUns1: {}"sv, a_actor->GetName(), equip_item.get()->item->GetName(), a_invent->unk118.score, a_invent->unk118.slot, clib_util::editorID::get_editorID(equip_item.get()->itemSlot.equipSlot), equip_item.get()->itemSlot.slot);
			// 	}
			// }

			// for (const auto equip_item : a_invent->unk148.items)
			// {
			// 	if (equip_item && equip_item.get())
			// 	{
			// 		logger::info("actor: {} itemSet2: {} scoreSet2: {:.2f} slotSet2: {} equipSlot2: {} equipUns2: {}"sv, a_actor->GetName(), equip_item.get()->item->GetName(), a_invent->unk148.score, a_invent->unk148.slot, clib_util::editorID::get_editorID(equip_item.get()->itemSlot.equipSlot), equip_item.get()->itemSlot.slot);
			// 	}
			// }
		}
    }

    void OnMeleeHitHook::ClearUpdates(RE::Actor *a_actor, bool clearAll)
	{
		if (clearAll)
		{
			_Timer.clear();
		}
		else
		{
			auto itt = _Timer.find(a_actor);
			if (itt != _Timer.end())
			{
				itt->second.clear();
				_Timer.erase(itt);
			}
		}
	}


	void OnMeleeHitHook::Update(RE::Actor* a_actor, [[maybe_unused]] float a_delta)
	{
		if (a_actor->GetActorRuntimeData().currentProcess && a_actor->GetActorRuntimeData().currentProcess->InHighProcess() && a_actor->Is3DLoaded())
		{
			if (!a_actor || !IsValidLifeState(a_actor))
			{
				return;
			}

			if (!GetBoolVariable(a_actor, "bNSV_IsinCombat"))
			{
				return;
			}

			GetSingleton()->Process_Updates(a_actor, std::chrono::steady_clock::now());

			if (GetBoolVariable(a_actor, "bNSV_IsMage") && !a_actor->HasKeywordString("NSV_ModCombatInvent_Exclude"))
			{
				if (auto combatcontrol = a_actor->GetActorRuntimeData().combatController; combatcontrol)
				{
					Mod_CombatInventory(a_actor, combatcontrol);

					if (a_actor->HasKeywordString("NSV_SpellSword") && !getrace_VLserana(a_actor, false))
					{
						Mod_CombatInventory_Weapons(a_actor, combatcontrol);
					}
					else
					{
						Mod_CombatInventory_Weapons_Reset(a_actor, combatcontrol);
					}

					// Check_CombatInventory(combatcontrol);
				}
			}
		}
	}

	void OnMeleeHitHook::Process_Updates(RE::Actor *a_actor, std::chrono::steady_clock::time_point time_now)
	{
		if (a_actor == nullptr || !IsValidLifeState(a_actor)){
			return;
		}

		const auto &it = _Timer.find(a_actor);

		if (it != _Timer.end())
		{
			if (!it->second.empty())
			{
				for (auto data : it->second)
				{
					RE::MagicCaster *caster = nullptr;
					std::chrono::steady_clock::time_point time_initial;
					std::chrono::milliseconds time_required;
					std::string function;
					std::tie(caster, time_initial, time_required, function) = data;

					if (duration_cast<std::chrono::milliseconds>(time_now - time_initial).count() >= time_required.count())
					{
						auto H = RE::TESDataHandler::GetSingleton();
						switch (hash(function.c_str(), function.size()))
						{
						case "HealConc_Update"_h:
						case "WardConc_Update"_h:
						case "GenericConc_Update"_h:

							if (caster != nullptr)
							{
								if (caster->GetCasterAsActor() && caster->GetCasterAsActor() == a_actor && caster->currentSpell)
								{
									GetEquippedSpell(a_actor, caster, caster->currentSpell, function);
								}
							}

							break;

						case "EquipUnequip_Update"_h:

							// if (!GetBoolVariable(a_actor, "bNSV_Equip_OK"))
							// {
							// 	a_actor->SetGraphVariableBool("bNSV_Equip_OK", true);
							// }

							break;

						case "TrapCooldown_Update"_h:

							if (GetBoolVariable(a_actor, "bNSV_Trap_Cool"))
							{
								a_actor->SetGraphVariableBool("bNSV_Trap_Cool", false);
							}

							break;

						case "BuffCooldown_Update"_h:

							if (GetBoolVariable(a_actor, "bNSV_Buff_Cool"))
							{
								a_actor->SetGraphVariableBool("bNSV_Buff_Cool", false);
							}

							break;

						case "DeBuffCooldown_Update"_h:

							if (GetBoolVariable(a_actor, "bNSV_DeBuff_Cool"))
							{
								a_actor->SetGraphVariableBool("bNSV_DeBuff_Cool", false);
							}

							break;

						case "InterruptCoolAI_Update"_h:

							if (GetBoolVariable(a_actor, "bNSV_Interruptcast_Cool"))
							{
								a_actor->SetGraphVariableBool("bNSV_Interruptcast_Cool", false);
							}

							break;

						case "AIScores_Update"_h:

							if(GetBoolVariable(a_actor, "bNSV_IsInCombat")){
								Vary_AI_Scores(a_actor);

								GetSingleton()->RegisterforUpdate(a_actor, std::forward_as_tuple(nullptr, std::chrono::steady_clock::now(), 8000ms, "AIScores_Update"));
							}

							break;

						case "ModCombat_Inventory_Update"_h:

							if (GetBoolVariable(a_actor, "bNSV_IsInCombat"))
							{
								if (auto combatcontrol = a_actor->GetActorRuntimeData().combatController; combatcontrol)
								{
									Mod_CombatInventory(a_actor, combatcontrol);

								}else
								{
									GetSingleton()->RegisterforUpdate(a_actor, std::forward_as_tuple(nullptr, std::chrono::steady_clock::now(), 2000ms, "ModCombat_Inventory_Update"));
								}
							}

							break;

						case "Cloak_Active_Update"_h:

							if (GetBoolVariable(a_actor, "bNSV_InterruptCast_Cloak"))
							{
								a_actor->SetGraphVariableBool("bNSV_InterruptCast_Cloak", false);
							}

							break;

						case "Armour_Active_Update"_h:

							if (GetBoolVariable(a_actor, "bNSV_InterruptCast_Armour"))
							{
								a_actor->SetGraphVariableBool("bNSV_InterruptCast_Armour", false);
							}

							break;

						case "Heal_Active_Update"_h:

							if (GetBoolVariable(a_actor, "bNSV_InterruptCast_Heal"))
							{
								a_actor->SetGraphVariableBool("bNSV_InterruptCast_Heal", false);
							}

							break;

						case "Invisibility_Active_Update"_h:

							if (GetBoolVariable(a_actor, "bNSV_InterruptCast_Invisibility"))
							{
								a_actor->SetGraphVariableBool("bNSV_InterruptCast_Invisibility", false);
							}

							break;

						case "Summon_Active_Update"_h:

							if (GetBoolVariable(a_actor, "bNSV_InterruptCast_Summon"))
							{
								a_actor->SetGraphVariableBool("bNSV_InterruptCast_Summon", false);
							}

							break;

						case "Ward_Active_Update"_h:

							if (GetBoolVariable(a_actor, "bNSV_InterruptCast_Ward"))
							{
								a_actor->SetGraphVariableBool("bNSV_InterruptCast_Ward", false);
							}

							break;

						case "Survive_Active_Update"_h:

							if (GetBoolVariable(a_actor, "bNSV_InterruptCast_Survive"))
							{
								a_actor->SetGraphVariableBool("bNSV_InterruptCast_Survive", false);
							}

							break;

						default:
							break;
						}
						std::vector<std::tuple<RE::MagicCaster *, std::chrono::steady_clock::time_point, std::chrono::milliseconds, std::string>>::iterator position = std::find(it->second.begin(), it->second.end(), data);
						if (position != it->second.end())
						{
							it->second.erase(position);
						}
					}
				}
			}
			else
			{
				_Timer.erase(it);
			}
		}
	}

	// void OnMeleeHitHook::init()
	// {
	// 	_precision_API = reinterpret_cast<PRECISION_API::IVPrecision1*>(PRECISION_API::RequestPluginAPI());
	// 	if (_precision_API) {
	// 		_precision_API->AddPostHitCallback(SKSE::GetPluginHandle(), PrecisionWeaponsCallback_Post);
	// 		logger::info("Enabled compatibility with Precision");
	// 	}
	// }

	// void OnMeleeHitHook::PrecisionWeaponsCallback_Post(const PRECISION_API::PrecisionHitData& a_precisionHitData, const RE::HitData& a_hitdata)
	// {
	// 	if (!a_precisionHitData.target || !a_precisionHitData.target->Is(RE::FormType::ActorCharacter)) {
	// 		return;
	// 	}
	// 	return;
	// }

	void Settings::Load(){
		constexpr auto path = "Data\\SKSE\\Plugins\\NPCSpellVariance.ini";

		CSimpleIniA ini;
		ini.SetUnicode();

		ini.LoadFile(path);

		general.Load(ini);

		include_spells_mods.Load(ini);
		include_spells_keywords.Load(ini);
		exclude_spells_mods.Load(ini);
		exclude_spells_keywords.Load(ini);

		ini.SaveFile(path);
	}

	

	void Settings::General_Settings::Load(CSimpleIniA &a_ini)
	{
		static const char *section = "General_Settings";

		auto DS = GetSingleton();

		DS->general.bWhiteListApproach = a_ini.GetBoolValue(section, "bWhiteListApproach", DS->general.bWhiteListApproach);
		DS->general.bDebug = a_ini.GetBoolValue(section, "bDebug", DS->general.bDebug);

		a_ini.SetBoolValue(section, "bWhiteListApproach", DS->general.bWhiteListApproach, ";If set to true, only the include mods and keywords are considered. Else only the exclude approach is used");
		a_ini.SetBoolValue(section, "bDebug", DS->general.bDebug, ";Enables logging for some processes");
	}

	void Settings::Include_AllSpells_withKeywords::Load(CSimpleIniA &a_ini)
	{
		static const char *section = "Include_AllSpells_withKeywords";

		auto DS = GetSingleton();

		DS->include_spells_keywords.inc_keywords_joined = a_ini.GetValue(section, "inc_keywords", DS->include_spells_keywords.inc_keywords_joined.c_str());

		std::istringstream f(DS->include_spells_keywords.inc_keywords_joined);
		std::string s;
		while (getline(f, s, '|'))
		{
			DS->include_spells_keywords.inc_keywords.emplace_back(s);
		}

		a_ini.SetValue(section, "inc_keywords", DS->include_spells_keywords.inc_keywords_joined.c_str(), ";Enter keywords for which all associated spells are included. Seperate keywords with | ");
	}

	void Settings::Include_AllSpells_inMods::Load(CSimpleIniA& a_ini){
		static const char* section = "Include_AllSpells_inMods";

		auto DS = GetSingleton();

		DS->include_spells_mods.inc_mods_joined = a_ini.GetValue(section, "inc_mods", DS->include_spells_mods.inc_mods_joined.c_str());

		std::istringstream f(DS->include_spells_mods.inc_mods_joined);
		std::string  s;
		while (getline(f, s, '|')) {
			DS->include_spells_mods.inc_mods.emplace_back(s);
		}

		a_ini.SetValue(section, "inc_mods", DS->include_spells_mods.inc_mods_joined.c_str(), ";Enter Mod Names of which all spells within are included. Seperate names with | ");
		//
	}

	void Settings::Exclude_AllSpells_withKeywords::Load(CSimpleIniA& a_ini)
	{
		static const char* section = "Exclude_AllSpells_withKeywords";

		auto DS = GetSingleton();

		DS->exclude_spells_keywords.exc_keywords_joined = a_ini.GetValue(section, "exc_keywords", DS->exclude_spells_keywords.exc_keywords_joined.c_str());

		std::istringstream f(DS->exclude_spells_keywords.exc_keywords_joined);
		std::string  s;
		while (getline(f, s, '|')) {
			DS->exclude_spells_keywords.exc_keywords.emplace_back(s);
		}

		a_ini.SetValue(section, "exc_keywords", DS->exclude_spells_keywords.exc_keywords_joined.c_str(), ";Enter keywords for which all associated spells are excluded. Seperate keywords with | ");
	}

	void Settings::Exclude_AllSpells_inMods::Load(CSimpleIniA &a_ini)
	{
		static const char *section = "Exclude_AllSpells_inMods";

		auto DS = GetSingleton();

		DS->exclude_spells_mods.exc_mods_joined = a_ini.GetValue(section, "exc_mods", DS->exclude_spells_mods.exc_mods_joined.c_str());

		std::istringstream f(DS->exclude_spells_mods.exc_mods_joined);
		std::string s;
		while (getline(f, s, '|'))
		{
			DS->exclude_spells_mods.exc_mods.emplace_back(s);
		}

		a_ini.SetValue(section, "exc_mods", DS->exclude_spells_mods.exc_mods_joined.c_str(), ";Enter Mod Names of which all spells within are excluded. Seperate names with | ");
		//
	}

	std::pair<bool, RE::EffectSetting *> OnMeleeHitHook::GetEffect_CC_TA(RE::SpellItem *a_spell, uint32_t &lastFormId)
	{
		RE::EffectSetting *a_effect = nullptr;
		bool detected_key = false;

		const auto NSV_Magic_Cloak = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Cloak");
		const auto NSV_Magic_Armour = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Armour");
		const auto NSV_Magic_Control = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Control");
		const auto NSV_Magic_Heal = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Heal");
		const auto NSV_Magic_Invisibility = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Invisibility");
		const auto NSV_Magic_CalmSeduce = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_CalmSeduce");

		const auto NSV_Magic_Fear = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Fear");
		const auto NSV_Magic_Summon = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Summon");
		const auto NSV_Magic_Ward = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Ward");
		const auto NSV_Magic_Absorb = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Absorb");

		const auto NSV_Magic_Damage = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Damage");
		const auto NSV_Magic_Teleport = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Teleport");
		const auto NSV_Survival_Lethal = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Survival_Lethal");
		const auto NSV_Magic_Trap = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Trap");

		const auto NSV_Generic_Buff = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Generic_Buff");
		const auto NSV_Generic_Debuff = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Generic_Debuff");

		const auto NSV_Tag_Undead = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Undead");
		const auto NSV_Tag_Vampiric = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Vampiric");
		const auto NSV_Tag_Summoned = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Summoned");

		const auto NSV_Tag_Ally = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Ally");
		const auto NSV_Tag_Enemy = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Enemy");
		const auto NSV_Tag_Weapon = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Weapon");
		const auto NSV_Tag_Area = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Area");

		const auto NSV_Tag_VampireLord_Ascendant = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_VampireLord_Ascendant");
		

		if (NSV_Magic_Cloak && a_spell->HasKeyword(NSV_Magic_Cloak))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_CC_CloakUndead");
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_CC_CloakSummoned");
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_CC_CloakBase");
			}
		}
		if (NSV_Magic_Armour && a_spell->HasKeyword(NSV_Magic_Armour))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_CC_ArmourUndead");
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_CC_ArmourSummoned");
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_CC_ArmourBase");
			}
		}
		if (NSV_Magic_Control && a_spell->HasKeyword(NSV_Magic_Control))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_CC_ControlUndead");
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_CC_ControlSummoned");
			}
			else if (NSV_Tag_Weapon && a_spell->HasKeyword(NSV_Tag_Weapon))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_CC_ControlWeapon");
			}
			else
			{
				if (NSV_Tag_VampireLord_Ascendant && a_spell->HasKeyword(NSV_Tag_VampireLord_Ascendant))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_CC_ControlBase_Ascendant");
				}
				else
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_CC_ControlBase");
				}
				
			}
		}
		if (NSV_Magic_Heal && a_spell->HasKeyword(NSV_Magic_Heal))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_CC_HealUndead");
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_CC_HealSummoned");
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_CC_HealBase");
			}
		}
		if (NSV_Magic_Invisibility && a_spell->HasKeyword(NSV_Magic_Invisibility))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_CC_InvisibilityUndead");
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_CC_InvisibilitySummoned");
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_CC_InvisibilityBase");
			}
		}
		if (NSV_Magic_CalmSeduce && a_spell->HasKeyword(NSV_Magic_CalmSeduce))
		{
			a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_CC_CalmSeduceBase");
		}
		if (NSV_Magic_Fear && a_spell->HasKeyword(NSV_Magic_Fear))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_CC_FearUndead");
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_CC_FearSummoned");
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_CC_FearBase");
			}
		}
		if (NSV_Magic_Summon && a_spell->HasKeyword(NSV_Magic_Summon))
		{
			if (NSV_Tag_Weapon && a_spell->HasKeyword(NSV_Tag_Weapon))
			{
				if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_CC_SummonWeaponUndead");
				}
				else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_CC_SummonWeaponSummoned");
				}
				else
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_CC_SummonWeapon");
				}
			}
		}
		if (NSV_Magic_Ward && a_spell->HasKeyword(NSV_Magic_Ward))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_CC_WardUndead");
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_CC_WardSummoned");
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_CC_WardBase");
			}
		}
		if (NSV_Magic_Absorb && a_spell->HasKeyword(NSV_Magic_Absorb))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_CC_AbsorbUndead");
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_CC_AbsorbSummoned");
			}
			else
			{


				if (NSV_Tag_VampireLord_Ascendant && a_spell->HasKeyword(NSV_Tag_VampireLord_Ascendant))
				{
				}
				else
				{
                    GetSingleton()->Clone_Spell(a_spell, lastFormId);
                    switch (GetSingleton()->GenerateRandomInt(1, 5))
					{
					case 1:
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_CC_AbsorbBase_1");
						break;

					case 2:
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_CC_AbsorbBase_2");
						break;

					case 3:
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_CC_AbsorbBase_3");
						break;

					case 4:
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_CC_AbsorbBase_4");
						break;

					case 5:
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_CC_AbsorbBase_5");
						break;

					default:
						break;
					}
				}
				
			}
		}
		if (NSV_Magic_Damage && a_spell->HasKeyword(NSV_Magic_Damage))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_CC_DamageUndead");
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_CC_DamageSummoned");
			}
			else
			{

				if (NSV_Tag_VampireLord_Ascendant && a_spell->HasKeyword(NSV_Tag_VampireLord_Ascendant))
				{
				}
				else
				{
                    GetSingleton()->Clone_Spell(a_spell, lastFormId);
                    switch (GetSingleton()->GenerateRandomInt(1, 5))
					{
					case 1:
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_CC_DamageBase_1");
						break;

					case 2:
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_CC_DamageBase_2");
						break;

					case 3:
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_CC_DamageBase_3");
						break;

					case 4:
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_CC_DamageBase_4");
						break;

					case 5:
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_CC_DamageBase_5");
						break;

					default:
						break;
					}
				}
				
			}
		}
		if (NSV_Magic_Teleport && a_spell->HasKeyword(NSV_Magic_Teleport))
		{
			if (NSV_Tag_Enemy && a_spell->HasKeyword(NSV_Tag_Enemy))
			{
				if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_CC_TeleportEnemyUndead");
				}
				else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_CC_TeleportEnemySummoned");
				}
				else
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_CC_TeleportEnemy");
				}
			}
			else
			{
				if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_CC_TeleportAllyUndead");
				}
				else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_CC_TeleportAllySummoned");
				}
				else
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_CC_TeleportAlly");
				}
			}
		}
		if (NSV_Survival_Lethal && a_spell->HasKeyword(NSV_Survival_Lethal))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_CC_SurviveUndead");
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_CC_SurviveSummoned");
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_CC_SurviveBase");
			}
		}
		if (NSV_Generic_Buff && a_spell->HasKeyword(NSV_Generic_Buff))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_CC_BuffUndead");
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_CC_BuffSummoned");
			}
			else if (NSV_Tag_Weapon && a_spell->HasKeyword(NSV_Tag_Weapon))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_CC_BuffWeapon");
			}else
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_CC_BuffBase");
			}
		}
		if (NSV_Generic_Debuff && a_spell->HasKeyword(NSV_Generic_Debuff))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_CC_DebuffUndead");
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_CC_DebuffSummoned");
			}
			else if (NSV_Tag_Weapon && a_spell->HasKeyword(NSV_Tag_Weapon))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_CC_DebuffWeapon");
			}
			else
			{
				if (NSV_Tag_VampireLord_Ascendant && a_spell->HasKeyword(NSV_Tag_VampireLord_Ascendant))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_CC_DebuffBase_VL_Ascendant");
				}
				else
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_CC_DebuffBase");
				}
				
			}
		}
		if (NSV_Magic_Trap && a_spell->HasKeyword(NSV_Magic_Trap))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_CC_TrapUndead");
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_CC_TrapSummoned");
			}
			else if (NSV_Tag_Weapon && a_spell->HasKeyword(NSV_Tag_Weapon))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_CC_TrapWeapon");
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_CC_TrapBase");
			}
		}

		if (a_effect)
		{
			detected_key = true;
		}
		return {detected_key, a_effect};
	}

	std::pair<bool, RE::EffectSetting *> OnMeleeHitHook::GetEffect_FF_Aimed(RE::SpellItem *a_spell, uint32_t &lastFormId)
	{
		RE::EffectSetting *a_effect = nullptr;
		bool detected_key = false;

		const auto NSV_Magic_Cloak = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Cloak");
		const auto NSV_Magic_Armour = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Armour");
		const auto NSV_Magic_Control = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Control");
		const auto NSV_Magic_Heal = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Heal");
		const auto NSV_Magic_Invisibility = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Invisibility");
		const auto NSV_Magic_CalmSeduce = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_CalmSeduce");

		const auto NSV_Magic_Fear = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Fear");
		const auto NSV_Magic_Summon = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Summon");
		const auto NSV_Magic_Ward = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Ward");
		const auto NSV_Magic_Absorb = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Absorb");

		const auto NSV_Magic_Damage = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Damage");
		const auto NSV_Magic_Teleport = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Teleport");
		const auto NSV_Survival_Lethal = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Survival_Lethal");
		const auto NSV_Magic_Trap = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Trap");

		const auto NSV_Generic_Buff = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Generic_Buff");
		const auto NSV_Generic_Debuff = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Generic_Debuff");

		const auto NSV_Tag_Undead = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Undead");
		const auto NSV_Tag_Vampiric = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Vampiric");
		const auto NSV_Tag_Summoned = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Summoned");

		const auto NSV_Tag_Ally = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Ally");
		const auto NSV_Tag_Enemy = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Enemy");
		const auto NSV_Tag_Weapon = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Weapon");
		const auto NSV_Tag_Area = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Area");

		const auto NSV_Tag_VampireLord_Ascendant = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_VampireLord_Ascendant");
		


		if (NSV_Magic_Cloak && a_spell->HasKeyword(NSV_Magic_Cloak))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_FF_CloakUndead");
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_FF_CloakSummoned");
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_FF_CloakBase");
			}
		}
		if (NSV_Magic_Armour && a_spell->HasKeyword(NSV_Magic_Armour))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_FF_ArmourUndead");
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_FF_ArmourSummoned");
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_FF_ArmourBase");
			}
		}
		if (NSV_Magic_Control && a_spell->HasKeyword(NSV_Magic_Control))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_FF_ControlUndead");
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_FF_ControlSummoned");
			}
			else if (NSV_Tag_Weapon && a_spell->HasKeyword(NSV_Tag_Weapon))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_FF_ControlWeapon");
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_FF_ControlBase");
			}
		}
		if (NSV_Magic_Heal && a_spell->HasKeyword(NSV_Magic_Heal))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_FF_HealUndead");
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_FF_HealSummoned");
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_FF_HealBase");
			}
		}
		if (NSV_Magic_Invisibility && a_spell->HasKeyword(NSV_Magic_Invisibility))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_FF_InvisibilityUndead");
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_FF_InvisibilitySummoned");
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_FF_InvisibilityBase");
			}
		}
		if (NSV_Magic_CalmSeduce && a_spell->HasKeyword(NSV_Magic_CalmSeduce))
		{
			a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_FF_CalmSeduceBase");
		}
		if (NSV_Magic_Fear && a_spell->HasKeyword(NSV_Magic_Fear))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_FF_FearUndead");
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_FF_FearSummoned");
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_FF_FearBase");
			}
		}
		if (NSV_Magic_Summon && a_spell->HasKeyword(NSV_Magic_Summon))
		{
			if (NSV_Tag_Weapon && a_spell->HasKeyword(NSV_Tag_Weapon))
			{
				if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_FF_SummonWeaponUndead");
				}
				else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_FF_SummonWeaponSummoned");
				}
				else
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_FF_SummonWeapon");
				}
			}else if(NSV_Tag_Ally && a_spell->HasKeyword(NSV_Tag_Ally)){
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_FF_SummonAlly");
			}
		}
		if (NSV_Magic_Ward && a_spell->HasKeyword(NSV_Magic_Ward))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_FF_WardUndead");
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_FF_WardSummoned");
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_FF_WardBase");
			}
		}
		if (NSV_Magic_Absorb && a_spell->HasKeyword(NSV_Magic_Absorb))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_FF_AbsorbUndead");
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_FF_AbsorbSummoned");
			}
			else
			{

				if (NSV_Tag_VampireLord_Ascendant && a_spell->HasKeyword(NSV_Tag_VampireLord_Ascendant))
				{
				}
				else
				{
                    GetSingleton()->Clone_Spell(a_spell, lastFormId);
                    switch (GetSingleton()->GenerateRandomInt(1, 5))
					{
					case 1:
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_FF_AbsorbBase_1");
						break;

					case 2:
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_FF_AbsorbBase_2");
						break;

					case 3:
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_FF_AbsorbBase_3");
						break;

					case 4:
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_FF_AbsorbBase_4");
						break;

					case 5:
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_FF_AbsorbBase_5");
						break;

					default:
						break;
					}
				}
			}
		}
		if (NSV_Magic_Damage && a_spell->HasKeyword(NSV_Magic_Damage))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_FF_DamageUndead");
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_FF_DamageSummoned");
			}
			else
			{

				if (NSV_Tag_VampireLord_Ascendant && a_spell->HasKeyword(NSV_Tag_VampireLord_Ascendant))
				{
					switch (GetSingleton()->GenerateRandomInt(1, 5))
					{
					case 1:
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_FF_DamageBase_1_VL_Ascendant");
						break;

					case 2:
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_FF_DamageBase_2_VL_Ascendant");
						break;

					case 3:
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_FF_DamageBase_3_VL_Ascendant");
						break;

					case 4:
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_FF_DamageBase_4_VL_Ascendant");
						break;

					case 5:
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_FF_DamageBase_5_VL_Ascendant");
						break;

					default:
						break;
					}
				}
				else
				{
                    GetSingleton()->Clone_Spell(a_spell, lastFormId);
                    switch (GetSingleton()->GenerateRandomInt(1, 5))
					{
					case 1:
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_FF_DamageBase_1");
						break;

					case 2:
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_FF_DamageBase_2");
						break;

					case 3:
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_FF_DamageBase_3");
						break;

					case 4:
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_FF_DamageBase_4");
						break;

					case 5:
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_FF_DamageBase_5");
						break;

					default:
						break;
					}
				}
				
			}
		}
		if (NSV_Magic_Teleport && a_spell->HasKeyword(NSV_Magic_Teleport))
		{
			if (NSV_Tag_Enemy && a_spell->HasKeyword(NSV_Tag_Enemy))
			{
				if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_FF_TeleportEnemyUndead");
				}
				else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_FF_TeleportEnemySummoned");
				}
				else
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_FF_TeleportEnemy");
				}
			}
			else
			{
				if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_FF_TeleportAllyUndead");
				}
				else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_FF_TeleportAllySummoned");
				}
				else
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_FF_TeleportAlly");
				}
			}
		}
		if (NSV_Survival_Lethal && a_spell->HasKeyword(NSV_Survival_Lethal))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_FF_SurviveUndead");
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_FF_SurviveSummoned");
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_FF_SurviveBase");
			}
		}
		if (NSV_Generic_Buff && a_spell->HasKeyword(NSV_Generic_Buff))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_FF_BuffUndead");
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_FF_BuffSummoned");
			}
			else if (NSV_Tag_Weapon && a_spell->HasKeyword(NSV_Tag_Weapon))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_FF_BuffWeapon");
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_FF_BuffBase");
			}
		}
		if (NSV_Generic_Debuff && a_spell->HasKeyword(NSV_Generic_Debuff))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_FF_DebuffUndead");
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_FF_DebuffSummoned");
			}
			else if (NSV_Tag_Weapon && a_spell->HasKeyword(NSV_Tag_Weapon))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_FF_DebuffWeapon");
			}
			else
			{
				if (NSV_Tag_VampireLord_Ascendant && a_spell->HasKeyword(NSV_Tag_VampireLord_Ascendant))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_FF_DebuffBase_VL_Ascendant");
				}
				else
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_FF_DebuffBase");
				}
				
			}
		}

		if (NSV_Magic_Trap && a_spell->HasKeyword(NSV_Magic_Trap))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_FF_TrapUndead");
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_FF_TrapSummoned");
			}
			else if (NSV_Tag_Weapon && a_spell->HasKeyword(NSV_Tag_Weapon))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_FF_TrapWeapon");
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_FF_TrapBase");
			}
		}

		if (a_effect)
		{
			detected_key = true;
		}
		return {detected_key, a_effect};
	}

	std::pair<bool, RE::EffectSetting *> OnMeleeHitHook::GetEffect_CC_Aimed(RE::SpellItem *a_spell, uint32_t &lastFormId)
	{
		RE::EffectSetting *a_effect = nullptr;
		bool detected_key = false;

		const auto NSV_Magic_Cloak = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Cloak");
		const auto NSV_Magic_Armour = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Armour");
		const auto NSV_Magic_Control = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Control");
		const auto NSV_Magic_Heal = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Heal");
		const auto NSV_Magic_Invisibility = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Invisibility");
		const auto NSV_Magic_CalmSeduce = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_CalmSeduce");

		const auto NSV_Magic_Fear = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Fear");
		const auto NSV_Magic_Summon = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Summon");
		const auto NSV_Magic_Ward = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Ward");
		const auto NSV_Magic_Absorb = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Absorb");

		const auto NSV_Magic_Damage = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Damage");
		const auto NSV_Magic_Teleport = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Teleport");
		const auto NSV_Survival_Lethal = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Survival_Lethal");
		const auto NSV_Magic_Trap = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Trap");

		const auto NSV_Generic_Buff = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Generic_Buff");
		const auto NSV_Generic_Debuff = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Generic_Debuff");

		const auto NSV_Tag_Undead = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Undead");
		const auto NSV_Tag_Vampiric = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Vampiric");
		const auto NSV_Tag_Summoned = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Summoned");

		const auto NSV_Tag_Ally = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Ally");
		const auto NSV_Tag_Enemy = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Enemy");
		const auto NSV_Tag_Weapon = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Weapon");
		const auto NSV_Tag_Area = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Area");

		const auto NSV_Tag_VampireLord_Ascendant = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_VampireLord_Ascendant");
		

		if (NSV_Magic_Cloak && a_spell->HasKeyword(NSV_Magic_Cloak))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_CC_CloakUndead");
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_CC_CloakSummoned");
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_CC_CloakBase");
			}
		}
		if (NSV_Magic_Armour && a_spell->HasKeyword(NSV_Magic_Armour))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_CC_ArmourUndead");
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_CC_ArmourSummoned");
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_CC_ArmourBase");
			}
		}
		if (NSV_Magic_Control && a_spell->HasKeyword(NSV_Magic_Control))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_CC_ControlUndead");
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_CC_ControlSummoned");
			}
			else if (NSV_Tag_Weapon && a_spell->HasKeyword(NSV_Tag_Weapon))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_CC_ControlWeapon");
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_CC_ControlBase");
			}
		}
		if (NSV_Magic_Heal && a_spell->HasKeyword(NSV_Magic_Heal))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_CC_HealUndead");
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_CC_HealSummoned");
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_CC_HealBase");
			}
		}
		if (NSV_Magic_Invisibility && a_spell->HasKeyword(NSV_Magic_Invisibility))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_CC_InvisibilityUndead");
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_CC_InvisibilitySummoned");
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_CC_InvisibilityBase");
			}
		}
		if (NSV_Magic_CalmSeduce && a_spell->HasKeyword(NSV_Magic_CalmSeduce))
		{
			a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_CC_CalmSeduceBase");
		}
		if (NSV_Magic_Fear && a_spell->HasKeyword(NSV_Magic_Fear))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_CC_FearUndead");
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_CC_FearSummoned");
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_CC_FearBase");
			}
		}
		if (NSV_Magic_Summon && a_spell->HasKeyword(NSV_Magic_Summon))
		{
			if (NSV_Tag_Weapon && a_spell->HasKeyword(NSV_Tag_Weapon))
			{
				if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_CC_SummonWeaponUndead");
				}
				else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_CC_SummonWeaponSummoned");
				}
				else
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_CC_SummonWeapon");
				}
			}
		}
		if (NSV_Magic_Ward && a_spell->HasKeyword(NSV_Magic_Ward))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_CC_WardUndead");
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_CC_WardSummoned");
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_CC_WardBase");
			}
		}
		if (NSV_Magic_Absorb && a_spell->HasKeyword(NSV_Magic_Absorb))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_CC_AbsorbUndead");
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_CC_AbsorbSummoned");
			}
			else
			{

				if (NSV_Tag_VampireLord_Ascendant && a_spell->HasKeyword(NSV_Tag_VampireLord_Ascendant))
				{
				}
				else
				{
                    GetSingleton()->Clone_Spell(a_spell, lastFormId);
                    switch (GetSingleton()->GenerateRandomInt(1, 5))
					{
					case 1:
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_CC_AbsorbBase_1");
						break;

					case 2:
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_CC_AbsorbBase_2");
						break;

					case 3:
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_CC_AbsorbBase_3");
						break;

					case 4:
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_CC_AbsorbBase_4");
						break;

					case 5:
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_CC_AbsorbBase_5");
						break;

					default:
						break;
					}
				}
				
			}
		}
		if (NSV_Magic_Damage && a_spell->HasKeyword(NSV_Magic_Damage))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_CC_DamageUndead");
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_CC_DamageSummoned");
			}
			else
			{

				if (NSV_Tag_VampireLord_Ascendant && a_spell->HasKeyword(NSV_Tag_VampireLord_Ascendant))
				{
					switch (GetSingleton()->GenerateRandomInt(1, 5))
					{
					case 1:
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_CC_DamageBase_1_VL_Ascendant");
						break;

					case 2:
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_CC_DamageBase_2_VL_Ascendant");
						break;

					case 3:
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_CC_DamageBase_3_VL_Ascendant");
						break;

					case 4:
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_CC_DamageBase_4_VL_Ascendant");
						break;

					case 5:
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_CC_DamageBase_5_VL_Ascendant");
						break;

					default:
						break;
					}
				}
				else
				{
                    GetSingleton()->Clone_Spell(a_spell, lastFormId);
                    switch (GetSingleton()->GenerateRandomInt(1, 5))
					{
					case 1:
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_CC_DamageBase_1");
						break;

					case 2:
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_CC_DamageBase_2");
						break;

					case 3:
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_CC_DamageBase_3");
						break;

					case 4:
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_CC_DamageBase_4");
						break;

					case 5:
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_CC_DamageBase_5");
						break;

					default:
						break;
					}
				}
			}
		}
		if (NSV_Magic_Teleport && a_spell->HasKeyword(NSV_Magic_Teleport))
		{
			if (NSV_Tag_Enemy && a_spell->HasKeyword(NSV_Tag_Enemy))
			{
				if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_CC_TeleportEnemyUndead");
				}
				else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_CC_TeleportEnemySummoned");
				}
				else
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_CC_TeleportEnemy");
				}
			}
			else
			{
				if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_CC_TeleportAllyUndead");
				}
				else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_CC_TeleportAllySummoned");
				}
				else
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_CC_TeleportAlly");
				}
			}
		}
		if (NSV_Survival_Lethal && a_spell->HasKeyword(NSV_Survival_Lethal))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_CC_SurviveUndead");
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_CC_SurviveSummoned");
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_CC_SurviveBase");
			}
		}
		if (NSV_Generic_Buff && a_spell->HasKeyword(NSV_Generic_Buff))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_CC_BuffUndead");
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_CC_BuffSummoned");
			}
			else if (NSV_Tag_Weapon && a_spell->HasKeyword(NSV_Tag_Weapon))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_CC_BuffWeapon");
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_CC_BuffBase");
			}
		}
		if (NSV_Generic_Debuff && a_spell->HasKeyword(NSV_Generic_Debuff))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_CC_DebuffUndead");
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_CC_DebuffSummoned");
			}
			else if (NSV_Tag_Weapon && a_spell->HasKeyword(NSV_Tag_Weapon))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_CC_DebuffWeapon");
			}
			else
			{
				if (NSV_Tag_VampireLord_Ascendant && a_spell->HasKeyword(NSV_Tag_VampireLord_Ascendant))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_CC_DebuffBase_VL_Ascendant");
				}
				else
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_CC_DebuffBase");
				}
				
			}
		}

		if (NSV_Magic_Trap && a_spell->HasKeyword(NSV_Magic_Trap))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_CC_TrapUndead");
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_CC_TrapSummoned");
			}
			else if (NSV_Tag_Weapon && a_spell->HasKeyword(NSV_Tag_Weapon))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_CC_TrapWeapon");
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Aimed_CC_TrapBase");
			}
		}

		if (a_effect)
		{
			detected_key = true;
		}
		return {detected_key, a_effect};
	}
	std::pair<bool, RE::EffectSetting *> OnMeleeHitHook::GetEffect_FF_TA(RE::SpellItem *a_spell, uint32_t &lastFormId)
	{
		RE::EffectSetting *a_effect = nullptr;
		bool detected_key = false;

		const auto NSV_Magic_Cloak = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Cloak");
		const auto NSV_Magic_Armour = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Armour");
		const auto NSV_Magic_Control = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Control");
		const auto NSV_Magic_Heal = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Heal");
		const auto NSV_Magic_Invisibility = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Invisibility");
		const auto NSV_Magic_CalmSeduce = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_CalmSeduce");

		const auto NSV_Magic_Fear = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Fear");
		const auto NSV_Magic_Summon = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Summon");
		const auto NSV_Magic_Ward = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Ward");
		const auto NSV_Magic_Absorb = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Absorb");

		const auto NSV_Magic_Damage = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Damage");
		const auto NSV_Magic_Teleport = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Teleport");
		const auto NSV_Survival_Lethal = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Survival_Lethal");
		const auto NSV_Magic_Trap = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Trap");

		const auto NSV_Generic_Buff = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Generic_Buff");
		const auto NSV_Generic_Debuff = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Generic_Debuff");

		const auto NSV_Tag_Undead = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Undead");
		const auto NSV_Tag_Vampiric = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Vampiric");
		const auto NSV_Tag_Summoned = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Summoned");

		const auto NSV_Tag_Ally = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Ally");
		const auto NSV_Tag_Enemy = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Enemy");
		const auto NSV_Tag_Weapon = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Weapon");
		const auto NSV_Tag_Area = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Area");

		const auto NSV_Tag_VampireLord_Ascendant = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_VampireLord_Ascendant");
		

		if (NSV_Magic_Cloak && a_spell->HasKeyword(NSV_Magic_Cloak))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_FF_CloakUndead");
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_FF_CloakSummoned");
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_FF_CloakBase");
			}
		}
		if (NSV_Magic_Armour && a_spell->HasKeyword(NSV_Magic_Armour))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_FF_ArmourUndead");
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_FF_ArmourSummoned");
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_FF_ArmourBase");
			}
		}
		if (NSV_Magic_Control && a_spell->HasKeyword(NSV_Magic_Control))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_FF_ControlUndead");
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_FF_ControlSummoned");
			}
			else if (NSV_Tag_Weapon && a_spell->HasKeyword(NSV_Tag_Weapon))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_FF_ControlWeapon");
			}
			else
			{
				if (NSV_Tag_VampireLord_Ascendant && a_spell->HasKeyword(NSV_Tag_VampireLord_Ascendant))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_FF_ControlBase_VL_Ascendant");
				}
				else
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_FF_ControlBase");
				}
				
			}
		}
		if (NSV_Magic_Heal && a_spell->HasKeyword(NSV_Magic_Heal))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_FF_HealUndead");
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_FF_HealSummoned");
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_FF_HealBase");
			}
		}
		if (NSV_Magic_Invisibility && a_spell->HasKeyword(NSV_Magic_Invisibility))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_FF_InvisibilityUndead");
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_FF_InvisibilitySummoned");
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_FF_InvisibilityBase");
			}
		}
		if (NSV_Magic_CalmSeduce && a_spell->HasKeyword(NSV_Magic_CalmSeduce))
		{
			a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_FF_CalmSeduceBase");
		}
		if (NSV_Magic_Fear && a_spell->HasKeyword(NSV_Magic_Fear))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_FF_FearUndead");
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_FF_FearSummoned");
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_FF_FearBase");
			}
		}
		if (NSV_Magic_Summon && a_spell->HasKeyword(NSV_Magic_Summon))
		{
			if (NSV_Tag_Weapon && a_spell->HasKeyword(NSV_Tag_Weapon))
			{
				if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_FF_SummonWeaponUndead");
				}
				else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_FF_SummonWeaponSummoned");
				}
				else
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_FF_SummonWeapon");
				}
			}else if(NSV_Tag_Ally && a_spell->HasKeyword(NSV_Tag_Ally)){

				if (NSV_Tag_VampireLord_Ascendant && a_spell->HasKeyword(NSV_Tag_VampireLord_Ascendant))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_FF_SummonAlly_VL_Ascendant");
				}
				else
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_FF_SummonAlly");
				}
				
			}
		}
		if (NSV_Magic_Ward && a_spell->HasKeyword(NSV_Magic_Ward))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_FF_WardUndead");
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_FF_WardSummoned");
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_FF_WardBase");
			}
		}
		if (NSV_Magic_Absorb && a_spell->HasKeyword(NSV_Magic_Absorb))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_FF_AbsorbUndead");
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_FF_AbsorbSummoned");
			}
			else
			{

				if (NSV_Tag_VampireLord_Ascendant && a_spell->HasKeyword(NSV_Tag_VampireLord_Ascendant))
				{
				}
				else
				{
                    GetSingleton()->Clone_Spell(a_spell, lastFormId);
                    switch (GetSingleton()->GenerateRandomInt(1, 5))
					{
					case 1:
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_FF_AbsorbBase_1");
						break;

					case 2:
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_FF_AbsorbBase_2");
						break;

					case 3:
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_FF_AbsorbBase_3");
						break;

					case 4:
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_FF_AbsorbBase_4");
						break;

					case 5:
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_FF_AbsorbBase_5");
						break;

					default:
						break;
					}
				}
				
			}
		}
		if (NSV_Magic_Damage && a_spell->HasKeyword(NSV_Magic_Damage))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_FF_DamageUndead");
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_FF_DamageSummoned");
			}
			else
			{

				if (NSV_Tag_VampireLord_Ascendant && a_spell->HasKeyword(NSV_Tag_VampireLord_Ascendant))
				{
				}
				else
				{
                    GetSingleton()->Clone_Spell(a_spell, lastFormId);
                    switch (GetSingleton()->GenerateRandomInt(1, 5))
					{
					case 1:
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_FF_DamageBase_1");
						break;

					case 2:
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_FF_DamageBase_2");
						break;

					case 3:
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_FF_DamageBase_3");
						break;

					case 4:
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_FF_DamageBase_4");
						break;

					case 5:
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_FF_DamageBase_5");
						break;

					default:
						break;
					}
				}
				
			}
		}
		if (NSV_Magic_Teleport && a_spell->HasKeyword(NSV_Magic_Teleport))
		{
			if (NSV_Tag_Enemy && a_spell->HasKeyword(NSV_Tag_Enemy))
			{
				if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_FF_TeleportEnemyUndead");
				}
				else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_FF_TeleportEnemySummoned");
				}
				else
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_FF_TeleportEnemy");
				}
			}
			else
			{
				if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_FF_TeleportAllyUndead");
				}
				else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_FF_TeleportAllySummoned");
				}
				else
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_FF_TeleportAlly");
				}
			}
		}
		if (NSV_Survival_Lethal && a_spell->HasKeyword(NSV_Survival_Lethal))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_FF_SurviveUndead");
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_FF_SurviveSummoned");
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_FF_SurviveBase");
			}
		}
		if (NSV_Generic_Buff && a_spell->HasKeyword(NSV_Generic_Buff))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_FF_BuffUndead");
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_FF_BuffSummoned");
			}
			else if (NSV_Tag_Weapon && a_spell->HasKeyword(NSV_Tag_Weapon))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_FF_BuffWeapon");
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_FF_BuffBase");
			}
		}
		if (NSV_Generic_Debuff && a_spell->HasKeyword(NSV_Generic_Debuff))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_FF_DebuffUndead");
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_FF_DebuffSummoned");
			}
			else if (NSV_Tag_Weapon && a_spell->HasKeyword(NSV_Tag_Weapon))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_FF_DebuffWeapon");
			}
			else
			{
				if (NSV_Tag_VampireLord_Ascendant && a_spell->HasKeyword(NSV_Tag_VampireLord_Ascendant))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_FF_DebuffBase_VL_Ascendant");
				}
				else
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_FF_DebuffBase");
				}
				
			}
		}

		if (NSV_Magic_Trap && a_spell->HasKeyword(NSV_Magic_Trap))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_FF_TrapUndead");
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_FF_TrapSummoned");
			}
			else if (NSV_Tag_Weapon && a_spell->HasKeyword(NSV_Tag_Weapon))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_FF_TrapWeapon");
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TA_FF_TrapBase");
			}
		}

		if (a_effect)
		{
			detected_key = true;
		}
		return {detected_key, a_effect};
	}

	std::pair<bool, RE::EffectSetting *> hooks::OnMeleeHitHook::GetEffect_FF_Self(RE::SpellItem *a_spell, uint32_t &lastFormId){

		RE::EffectSetting *a_effect = nullptr;
		bool detected_key = false;

		const auto NSV_Magic_Cloak = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Cloak");
		const auto NSV_Magic_Armour = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Armour");
		const auto NSV_Magic_Control = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Control");
		const auto NSV_Magic_Heal = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Heal");
		const auto NSV_Magic_Invisibility = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Invisibility");
		const auto NSV_Magic_CalmSeduce = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_CalmSeduce");

		const auto NSV_Magic_Fear = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Fear");
		const auto NSV_Magic_Summon = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Summon");
		const auto NSV_Magic_Ward = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Ward");
		const auto NSV_Magic_Absorb = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Absorb");

		const auto NSV_Magic_Damage = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Damage");
		const auto NSV_Magic_Teleport = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Teleport");
		const auto NSV_Survival_Lethal = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Survival_Lethal");
		const auto NSV_Magic_Trap = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Trap");

		const auto NSV_Generic_Buff = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Generic_Buff");
		const auto NSV_Generic_Debuff = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Generic_Debuff");

		const auto NSV_Resource_Convert = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Resource_Convert");
		const auto NSV_Tag_Health_to = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Health_to");
		const auto NSV_Tag_Magicka_to = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Magicka_to");
		const auto NSV_Tag_Stamina_to = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Stamina_to");
		const auto NSV_Tag_pMagicka = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_pMagicka");
		const auto NSV_Tag_pStamina = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_pStamina");
		const auto NSV_Tag_pHealth = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_pHealth");

		const auto NSV_Tag_Undead = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Undead");
		const auto NSV_Tag_Vampiric = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Vampiric");
		const auto NSV_Tag_Summoned = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Summoned");

		const auto NSV_Tag_Ally = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Ally");
		const auto NSV_Tag_Enemy = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Enemy");
		const auto NSV_Tag_Weapon = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Weapon");
		const auto NSV_Tag_Area = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Area");

		const auto NSV_Tag_VampireLord_Ascendant = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_VampireLord_Ascendant");
		

		if (NSV_Resource_Convert && a_spell->HasKeyword(NSV_Resource_Convert))
		{
			if (NSV_Tag_Health_to && a_spell->HasKeyword(NSV_Tag_Health_to))
			{
				if (NSV_Tag_pMagicka && NSV_Tag_pStamina && a_spell->HasKeyword(NSV_Tag_pMagicka) && a_spell->HasKeyword(NSV_Tag_pStamina))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_Convert_Health_MagickaStamina");
				}
				else if (NSV_Tag_pMagicka && a_spell->HasKeyword(NSV_Tag_pMagicka))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_Convert_Health_Magicka");
				}
				else if (NSV_Tag_pStamina  && a_spell->HasKeyword(NSV_Tag_pStamina))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_Convert_Health_Stamina");
				}
			}
			else if (NSV_Tag_Stamina_to && a_spell->HasKeyword(NSV_Tag_Stamina_to))
			{
				if (NSV_Tag_pMagicka && NSV_Tag_pHealth && a_spell->HasKeyword(NSV_Tag_pMagicka) && a_spell->HasKeyword(NSV_Tag_pHealth))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_Convert_Stamina_HealthMagicka");
				}
				else if (NSV_Tag_pMagicka && a_spell->HasKeyword(NSV_Tag_pMagicka))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_Convert_Stamina_Magicka");
				}
				else if (NSV_Tag_pHealth && a_spell->HasKeyword(NSV_Tag_pHealth))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_Convert_Stamina_Health");
				}
			}
			else if (NSV_Tag_Magicka_to && a_spell->HasKeyword(NSV_Tag_Magicka_to))
			{
				if (NSV_Tag_pStamina && a_spell->HasKeyword(NSV_Tag_pStamina))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_Convert_Magicka_Stamina");
				}
			}
		}

		if (NSV_Magic_Cloak && a_spell->HasKeyword(NSV_Magic_Cloak))
		{
			if (NSV_Tag_Ally && a_spell->HasKeyword(NSV_Tag_Ally)){
				if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_CloakAllyUndead");
				}
				else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_CloakAllySummoned");
				}
				else
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_CloakAlly");
				}
			}else{
				if (NSV_Tag_VampireLord_Ascendant && a_spell->HasKeyword(NSV_Tag_VampireLord_Ascendant))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_CloakBase_VL_Ascendant");
				}
				else
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_CloakBase");
				}
				
			}
		}
		if (NSV_Magic_Armour && a_spell->HasKeyword(NSV_Magic_Armour))
		{
			if (NSV_Tag_Ally && a_spell->HasKeyword(NSV_Tag_Ally))
			{
				if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_ArmourAllyUndead");
				}
				else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_ArmourAllySummoned");
				}
				else
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_ArmourAlly");
				}
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_ArmourBase");
			}
		}
		if (NSV_Magic_Control && a_spell->HasKeyword(NSV_Magic_Control))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_ControlUndead");
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_ControlSummoned");
			}
			else if (NSV_Tag_Weapon && a_spell->HasKeyword(NSV_Tag_Weapon))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_ControlWeapon");
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_ControlBase");
			}
		}
		if (NSV_Magic_Heal && a_spell->HasKeyword(NSV_Magic_Heal))
		{
			if (NSV_Tag_Ally && a_spell->HasKeyword(NSV_Tag_Ally))
			{
				if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_HealAllyUndead");
				}
				else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_HealAllySummoned");
				}
				else
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_HealAlly");
				}
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_HealBase");
			}
		}
		if (NSV_Magic_Invisibility && a_spell->HasKeyword(NSV_Magic_Invisibility))
		{
			if (NSV_Tag_Ally && a_spell->HasKeyword(NSV_Tag_Ally))
			{
				if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_InvisibilityAllyUndead");
				}
				else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_InvisibilityAllySummoned");
				}
				else
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_InvisibilityAlly");
				}
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_InvisibilityBase");
			}
		}
		if (NSV_Magic_CalmSeduce && a_spell->HasKeyword(NSV_Magic_CalmSeduce))
		{
			a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_CalmSeduceBase");
		}
		if (NSV_Magic_Fear && a_spell->HasKeyword(NSV_Magic_Fear))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_FearUndead");
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_FearSummoned");
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_FearBase");
			}
		}
		if (NSV_Magic_Summon && a_spell->HasKeyword(NSV_Magic_Summon))
		{
			if (NSV_Tag_Ally && a_spell->HasKeyword(NSV_Tag_Ally))
			{
				if (NSV_Tag_Weapon && a_spell->HasKeyword(NSV_Tag_Weapon)){

					if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
					{
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_SummonAllyWeaponUndead");
					}
					else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
					{
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_SummonAllyWeaponSummoned");
					}
					else
					{
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_SummonAllyWeapon");
					}
				}else{
					if (NSV_Tag_VampireLord_Ascendant && a_spell->HasKeyword(NSV_Tag_VampireLord_Ascendant))
					{
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_SummonAlly_VL_Ascendant");
					}
					else
					{
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_SummonAlly");
					}
					
				}
				
			}
			else if (NSV_Tag_Weapon && a_spell->HasKeyword(NSV_Tag_Weapon))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_SummonWeapon");
			}
		}
		if (NSV_Magic_Ward && a_spell->HasKeyword(NSV_Magic_Ward))
		{
			if (NSV_Tag_Ally && a_spell->HasKeyword(NSV_Tag_Ally))
			{
				if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_WardAllyUndead");
				}
				else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_WardAllySummoned");
				}
				else
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_WardAlly");
				}
			}
			else
			{
				//GetSingleton()->Clone_Spell(a_spell);
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_WardBase");
			}
		}
		if (NSV_Magic_Absorb && a_spell->HasKeyword(NSV_Magic_Absorb))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_AbsorbUndead");
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_AbsorbSummoned");
			}
			else
			{

				if (NSV_Tag_VampireLord_Ascendant && a_spell->HasKeyword(NSV_Tag_VampireLord_Ascendant))
				{
				}
				else
				{
                    GetSingleton()->Clone_Spell(a_spell, lastFormId);
                    switch (GetSingleton()->GenerateRandomInt(1, 5))
					{
					case 1:
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_AbsorbBase_1");
						break;

					case 2:
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_AbsorbBase_2");
						break;

					case 3:
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_AbsorbBase_3");
						break;

					case 4:
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_AbsorbBase_4");
						break;

					case 5:
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_AbsorbBase_5");
						break;

					default:
						break;
					}
				}
				
			}
		}
		if (NSV_Magic_Damage && a_spell->HasKeyword(NSV_Magic_Damage))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_DamageUndead");
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_DamageSummoned");
			}
			else
			{

				if (NSV_Tag_VampireLord_Ascendant && a_spell->HasKeyword(NSV_Tag_VampireLord_Ascendant))
				{
				}
				else
				{
                    GetSingleton()->Clone_Spell(a_spell, lastFormId);
                    switch (GetSingleton()->GenerateRandomInt(1, 5))
					{
					case 1:
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_DamageBase_1");
						break;

					case 2:
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_DamageBase_2");
						break;

					case 3:
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_DamageBase_3");
						break;

					case 4:
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_DamageBase_4");
						break;

					case 5:
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_DamageBase_5");
						break;

					default:
						break;
					}
				}
				
			}
		}
		if (NSV_Magic_Teleport && a_spell->HasKeyword(NSV_Magic_Teleport))
		{
			if (NSV_Tag_Enemy && a_spell->HasKeyword(NSV_Tag_Enemy))
			{
				if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_TeleportEnemyUndead");
				}
				else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_TeleportEnemySummoned");
				}
				else
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_TeleportEnemy");
				}
			}
			else if (NSV_Tag_Ally && a_spell->HasKeyword(NSV_Tag_Ally))
			{
				if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_TeleportAllyUndead");
				}
				else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_TeleportAllySummoned");
				}
				else
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_TeleportAlly");
				}
			}else{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_TeleportBase");
			}
		}
		if (NSV_Survival_Lethal && a_spell->HasKeyword(NSV_Survival_Lethal))
		{
			if (NSV_Tag_Ally && a_spell->HasKeyword(NSV_Tag_Ally))
			{
				if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_SurviveAllyUndead");
				}
				else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_SurviveAllySummoned");
				}
				else
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_SurviveAlly");
				}
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_SurviveBase");
			}
		}
		if (NSV_Generic_Buff && a_spell->HasKeyword(NSV_Generic_Buff))
		{
			if (NSV_Tag_Ally && a_spell->HasKeyword(NSV_Tag_Ally))
			{
				if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_BuffAllyUndead");
				}
				else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_BuffAllySummoned");
				}
				else if (NSV_Tag_Weapon && a_spell->HasKeyword(NSV_Tag_Weapon))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_BuffAllyWeapon");
				}
				else
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_BuffAlly");
				}
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_BuffBase");
			}
		}
		if (NSV_Generic_Debuff && a_spell->HasKeyword(NSV_Generic_Debuff))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_DebuffUndead");
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_DebuffSummoned");
			}
			else if (NSV_Tag_Weapon && a_spell->HasKeyword(NSV_Tag_Weapon))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_DebuffWeapon");
			}
			else
			{
				if (NSV_Tag_VampireLord_Ascendant && a_spell->HasKeyword(NSV_Tag_VampireLord_Ascendant))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_DebuffBase_VL_Ascendant");
				}
				else
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_DebuffBase");
				}
				
			}
		}

		if (NSV_Magic_Trap && a_spell->HasKeyword(NSV_Magic_Trap))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_TrapUndead");
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_TrapSummoned");
			}
			else if (NSV_Tag_Weapon && a_spell->HasKeyword(NSV_Tag_Weapon))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_TrapWeapon");
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_FF_TrapBase");
			}
		}

		if (a_effect)
		{
			detected_key = true;
		}
		return {detected_key, a_effect};
	}

	std::pair<bool, RE::EffectSetting *> hooks::OnMeleeHitHook::GetEffect_CC_Self(RE::SpellItem *a_spell, uint32_t &lastFormId){
		
		RE::EffectSetting *a_effect = nullptr;
		bool detected_key = false;

		const auto NSV_Magic_Cloak = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Cloak");
		const auto NSV_Magic_Armour = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Armour");
		const auto NSV_Magic_Control = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Control");
		const auto NSV_Magic_Heal = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Heal");
		const auto NSV_Magic_Invisibility = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Invisibility");
		const auto NSV_Magic_CalmSeduce = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_CalmSeduce");

		const auto NSV_Magic_Fear = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Fear");
		const auto NSV_Magic_Summon = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Summon");
		const auto NSV_Magic_Ward = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Ward");
		const auto NSV_Magic_Absorb = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Absorb");

		const auto NSV_Magic_Damage = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Damage");
		const auto NSV_Magic_Teleport = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Teleport");
		const auto NSV_Survival_Lethal = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Survival_Lethal");
		const auto NSV_Magic_Trap = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Trap");

		const auto NSV_Generic_Buff = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Generic_Buff");
		const auto NSV_Generic_Debuff = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Generic_Debuff");

		const auto NSV_Resource_Convert = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Resource_Convert");
		const auto NSV_Tag_Health_to = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Health_to");
		const auto NSV_Tag_Magicka_to = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Magicka_to");
		const auto NSV_Tag_Stamina_to = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Stamina_to");
		const auto NSV_Tag_pMagicka = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_pMagicka");
		const auto NSV_Tag_pStamina = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_pStamina");
		const auto NSV_Tag_pHealth = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_pHealth");

		const auto NSV_Tag_Undead = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Undead");
		const auto NSV_Tag_Vampiric = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Vampiric");
		const auto NSV_Tag_Summoned = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Summoned");

		const auto NSV_Tag_Ally = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Ally");
		const auto NSV_Tag_Enemy = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Enemy");
		const auto NSV_Tag_Weapon = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Weapon");
		const auto NSV_Tag_Area = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Area");

		const auto NSV_Tag_VampireLord_Ascendant = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_VampireLord_Ascendant");
		

		if (NSV_Resource_Convert && a_spell->HasKeyword(NSV_Resource_Convert))
		{
			if (NSV_Tag_Health_to && a_spell->HasKeyword(NSV_Tag_Health_to))
			{
				if (NSV_Tag_pMagicka && NSV_Tag_pStamina && a_spell->HasKeyword(NSV_Tag_pMagicka) && a_spell->HasKeyword(NSV_Tag_pStamina))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_Convert_Health_MagickaStamina");
				}
				else if (NSV_Tag_pMagicka && a_spell->HasKeyword(NSV_Tag_pMagicka))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_Convert_Health_Magicka");
				}
				else if (NSV_Tag_pStamina && a_spell->HasKeyword(NSV_Tag_pStamina))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_Convert_Health_Stamina");
				}
			}
			else if (NSV_Tag_Stamina_to && a_spell->HasKeyword(NSV_Tag_Stamina_to))
			{
				if (NSV_Tag_pMagicka && NSV_Tag_pHealth && a_spell->HasKeyword(NSV_Tag_pMagicka) && a_spell->HasKeyword(NSV_Tag_pHealth))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_Convert_Stamina_HealthMagicka");
				}
				else if (NSV_Tag_pMagicka && a_spell->HasKeyword(NSV_Tag_pMagicka))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_Convert_Stamina_Magicka");
				}
				else if (NSV_Tag_pHealth && a_spell->HasKeyword(NSV_Tag_pHealth))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_Convert_Stamina_Health");
				}
			}
			else if (NSV_Tag_Magicka_to && a_spell->HasKeyword(NSV_Tag_Magicka_to))
			{
				if (NSV_Tag_pStamina && a_spell->HasKeyword(NSV_Tag_pStamina))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_Convert_Magicka_Stamina");
				}
			}
		}

		if (NSV_Magic_Cloak && a_spell->HasKeyword(NSV_Magic_Cloak))
		{
			if (NSV_Tag_Ally && a_spell->HasKeyword(NSV_Tag_Ally))
			{
				if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_CloakAllyUndead");
				}
				else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_CloakAllySummoned");
				}
				else
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_CloakAlly");
				}
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_CloakBase");
			}
		}
		if (NSV_Magic_Armour && a_spell->HasKeyword(NSV_Magic_Armour))
		{
			if (NSV_Tag_Ally && a_spell->HasKeyword(NSV_Tag_Ally))
			{
				if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_ArmourAllyUndead");
				}
				else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_ArmourAllySummoned");
				}
				else
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_ArmourAlly");
				}
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_ArmourBase");
			}
		}
		if (NSV_Magic_Control && a_spell->HasKeyword(NSV_Magic_Control))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_ControlUndead");
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_ControlSummoned");
			}
			else if (NSV_Tag_Weapon && a_spell->HasKeyword(NSV_Tag_Weapon))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_ControlWeapon");
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_ControlBase");
			}
		}
		if (NSV_Magic_Heal && a_spell->HasKeyword(NSV_Magic_Heal))
		{
			if (NSV_Tag_Ally && a_spell->HasKeyword(NSV_Tag_Ally))
			{
				if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_HealAllyUndead");
				}
				else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_HealAllySummoned");
				}
				else
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_HealAlly");
				}
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_HealBase");
			}
		}
		if (NSV_Magic_Invisibility && a_spell->HasKeyword(NSV_Magic_Invisibility))
		{
			if (NSV_Tag_Ally && a_spell->HasKeyword(NSV_Tag_Ally))
			{
				if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_InvisibilityAllyUndead");
				}
				else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_InvisibilityAllySummoned");
				}
				else
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_InvisibilityAlly");
				}
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_InvisibilityBase");
			}
		}
		if (NSV_Magic_CalmSeduce && a_spell->HasKeyword(NSV_Magic_CalmSeduce))
		{
			a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_CalmSeduceBase");
		}
		if (NSV_Magic_Fear && a_spell->HasKeyword(NSV_Magic_Fear))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_FearUndead");
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_FearSummoned");
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_FearBase");
			}
		}
		if (NSV_Magic_Summon && a_spell->HasKeyword(NSV_Magic_Summon))
		{
			if (NSV_Tag_Ally && a_spell->HasKeyword(NSV_Tag_Ally))
			{
				if (NSV_Tag_Weapon && a_spell->HasKeyword(NSV_Tag_Weapon))
				{

					if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
					{
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_SummonAllyWeaponUndead");
					}
					else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
					{
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_SummonAllyWeaponSummoned");
					}
					else
					{
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_SummonAllyWeapon");
					}
				}
				else
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_SummonAlly");
				}
			}
			else if (NSV_Tag_Weapon && a_spell->HasKeyword(NSV_Tag_Weapon))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_SummonWeapon");
			}
		}
		if (NSV_Magic_Ward && a_spell->HasKeyword(NSV_Magic_Ward))
		{
			if (NSV_Tag_Ally && a_spell->HasKeyword(NSV_Tag_Ally))
			{
				if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_WardAllyUndead");
				}
				else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_WardAllySummoned");
				}
				else
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_WardAlly");
				}
			}
			else
			{
				//GetSingleton()->Clone_Spell(a_spell);
				if (NSV_Tag_VampireLord_Ascendant && a_spell->HasKeyword(NSV_Tag_VampireLord_Ascendant))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_WardBase_VL_Ascendant");
				}
				else
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_WardBase");
				}
				
			}
		}
		if (NSV_Magic_Absorb && a_spell->HasKeyword(NSV_Magic_Absorb))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_AbsorbUndead");
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_AbsorbSummoned");
			}
			else
			{

				if (NSV_Tag_VampireLord_Ascendant && a_spell->HasKeyword(NSV_Tag_VampireLord_Ascendant))
				{
				}
				else
				{
                    GetSingleton()->Clone_Spell(a_spell, lastFormId);
                    switch (GetSingleton()->GenerateRandomInt(1, 5))
					{
					case 1:
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_AbsorbBase_1");
						break;

					case 2:
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_AbsorbBase_2");
						break;

					case 3:
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_AbsorbBase_3");
						break;

					case 4:
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_AbsorbBase_4");
						break;

					case 5:
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_AbsorbBase_5");
						break;

					default:
						break;
					}
				}
				
			}
		}
		if (NSV_Magic_Damage && a_spell->HasKeyword(NSV_Magic_Damage))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_DamageUndead");
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_DamageSummoned");
			}
			else
			{

				if (NSV_Tag_VampireLord_Ascendant && a_spell->HasKeyword(NSV_Tag_VampireLord_Ascendant))
				{
				}
				else
				{
                    GetSingleton()->Clone_Spell(a_spell, lastFormId);
                    switch (GetSingleton()->GenerateRandomInt(1, 5))
					{
					case 1:
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_DamageBase_1");
						break;

					case 2:
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_DamageBase_2");
						break;

					case 3:
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_DamageBase_3");
						break;

					case 4:
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_DamageBase_4");
						break;

					case 5:
						a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_DamageBase_5");
						break;

					default:
						break;
					}
				}
				
			}
		}
		if (NSV_Magic_Teleport && a_spell->HasKeyword(NSV_Magic_Teleport))
		{
			if (NSV_Tag_Enemy && a_spell->HasKeyword(NSV_Tag_Enemy))
			{
				if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_TeleportEnemyUndead");
				}
				else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_TeleportEnemySummoned");
				}
				else
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_TeleportEnemy");
				}
			}
			else if (NSV_Tag_Ally && a_spell->HasKeyword(NSV_Tag_Ally))
			{
				if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_TeleportAllyUndead");
				}
				else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_TeleportAllySummoned");
				}
				else
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_TeleportAlly");
				}
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_TeleportBase");
			}
		}
		if (NSV_Survival_Lethal && a_spell->HasKeyword(NSV_Survival_Lethal))
		{
			if (NSV_Tag_Ally && a_spell->HasKeyword(NSV_Tag_Ally))
			{
				if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_SurviveAllyUndead");
				}
				else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_SurviveAllySummoned");
				}
				else
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_SurviveAlly");
				}
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_SurviveBase");
			}
		}
		if (NSV_Generic_Buff && a_spell->HasKeyword(NSV_Generic_Buff))
		{
			if (NSV_Tag_Ally && a_spell->HasKeyword(NSV_Tag_Ally))
			{
				if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_BuffAllyUndead");
				}
				else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_BuffAllySummoned");
				}
				else if (NSV_Tag_Weapon && a_spell->HasKeyword(NSV_Tag_Weapon))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_BuffAllyWeapon");
				}
				else
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_BuffAlly");
				}
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_BuffBase");
			}
		}
		if (NSV_Generic_Debuff && a_spell->HasKeyword(NSV_Generic_Debuff))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_DebuffUndead");
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_DebuffSummoned");
			}
			else if (NSV_Tag_Weapon && a_spell->HasKeyword(NSV_Tag_Weapon))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_DebuffWeapon");
			}
			else
			{
				if (NSV_Tag_VampireLord_Ascendant && a_spell->HasKeyword(NSV_Tag_VampireLord_Ascendant))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_DebuffBase_VL_Ascendant");
				}
				else
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_DebuffBase");
				}

			}
		}

		if (NSV_Magic_Trap && a_spell->HasKeyword(NSV_Magic_Trap))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_TrapUndead");
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_TrapSummoned");
			}
			else if (NSV_Tag_Weapon && a_spell->HasKeyword(NSV_Tag_Weapon))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_TrapWeapon");
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_Self_CC_TrapBase");
			}
		}

		if (a_effect)
		{
			detected_key = true;
		}
		return {detected_key, a_effect};
	}

	std::pair<bool, RE::EffectSetting *> hooks::OnMeleeHitHook::GetEffect_FF_TL(RE::SpellItem *a_spell, uint32_t &lastFormId){

		RE::EffectSetting *a_effect = nullptr;
		bool detected_key = false;

		const auto NSV_Magic_Cloak = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Cloak");
		const auto NSV_Magic_Armour = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Armour");
		const auto NSV_Magic_Control = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Control");
		const auto NSV_Magic_Heal = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Heal");
		const auto NSV_Magic_Invisibility = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Invisibility");
		const auto NSV_Magic_CalmSeduce = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_CalmSeduce");

		const auto NSV_Magic_Fear = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Fear");
		const auto NSV_Magic_Summon = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Summon");
		const auto NSV_Magic_Ward = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Ward");
		const auto NSV_Magic_Absorb = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Absorb");

		const auto NSV_Magic_Damage = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Damage");
		const auto NSV_Magic_Teleport = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Teleport");
		const auto NSV_Survival_Lethal = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Survival_Lethal");
		const auto NSV_Magic_Trap = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Trap");

		const auto NSV_Generic_Buff = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Generic_Buff");
		const auto NSV_Generic_Debuff = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Generic_Debuff");

		const auto NSV_Tag_Undead = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Undead");
		const auto NSV_Tag_Vampiric = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Vampiric");
		const auto NSV_Tag_Summoned = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Summoned");

		const auto NSV_Tag_Ally = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Ally");
		const auto NSV_Tag_Enemy = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Enemy");
		const auto NSV_Tag_Weapon = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Weapon");
		const auto NSV_Tag_Area = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Area");

		const auto NSV_Tag_VampireLord_Ascendant = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_VampireLord_Ascendant");
		

		if (NSV_Magic_Summon && a_spell->HasKeyword(NSV_Magic_Summon))
		{
			if (NSV_Tag_Ally && a_spell->HasKeyword(NSV_Tag_Ally))
			{
				if (NSV_Tag_VampireLord_Ascendant && a_spell->HasKeyword(NSV_Tag_VampireLord_Ascendant))
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TL_FF_SummonAlly_VL_Ascendant");
				}
				else
				{
					a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TL_FF_SummonAlly");
				}
				
			}
			else if (NSV_Tag_Weapon && a_spell->HasKeyword(NSV_Tag_Weapon))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TL_FF_SummonWeapon");
			}
		}

		if (NSV_Magic_Trap && a_spell->HasKeyword(NSV_Magic_Trap))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TL_FF_TrapUndead");
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TL_FF_TrapSummoned");
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TL_FF_TrapBase");
			}
		}

		if (a_effect)
		{
			detected_key = true;
		}
		return {detected_key, a_effect};
	}

	std::pair<bool, RE::EffectSetting *> hooks::OnMeleeHitHook::GetEffect_CC_TL(RE::SpellItem *a_spell, uint32_t &lastFormId){

		RE::EffectSetting *a_effect = nullptr;
		bool detected_key = false;

		const auto NSV_Magic_Cloak = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Cloak");
		const auto NSV_Magic_Armour = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Armour");
		const auto NSV_Magic_Control = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Control");
		const auto NSV_Magic_Heal = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Heal");
		const auto NSV_Magic_Invisibility = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Invisibility");
		const auto NSV_Magic_CalmSeduce = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_CalmSeduce");

		const auto NSV_Magic_Fear = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Fear");
		const auto NSV_Magic_Summon = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Summon");
		const auto NSV_Magic_Ward = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Ward");
		const auto NSV_Magic_Absorb = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Absorb");

		const auto NSV_Magic_Damage = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Damage");
		const auto NSV_Magic_Teleport = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Teleport");
		const auto NSV_Survival_Lethal = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Survival_Lethal");
		const auto NSV_Magic_Trap = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Trap");

		const auto NSV_Generic_Buff = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Generic_Buff");
		const auto NSV_Generic_Debuff = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Generic_Debuff");

		const auto NSV_Tag_Undead = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Undead");
		const auto NSV_Tag_Vampiric = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Vampiric");
		const auto NSV_Tag_Summoned = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Summoned");

		const auto NSV_Tag_Ally = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Ally");
		const auto NSV_Tag_Enemy = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Enemy");
		const auto NSV_Tag_Weapon = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Weapon");
		const auto NSV_Tag_Area = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_Area");

		const auto NSV_Tag_VampireLord_Ascendant = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Tag_VampireLord_Ascendant");
		

		if (NSV_Magic_Summon && a_spell->HasKeyword(NSV_Magic_Summon))
		{
			if (NSV_Tag_Ally && a_spell->HasKeyword(NSV_Tag_Ally))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TL_CC_SummonAlly");
			}
			else if (NSV_Tag_Weapon && a_spell->HasKeyword(NSV_Tag_Weapon))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TL_CC_SummonWeapon");
			}
		}

		if (NSV_Magic_Trap && a_spell->HasKeyword(NSV_Magic_Trap))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TL_CC_TrapUndead");
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TL_CC_TrapSummoned");
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID<RE::EffectSetting>("NSV_TL_CC_TrapBase");
			}
			
		}

		if (a_effect)
		{
			detected_key = true;
		}
		return {detected_key, a_effect};
	}
}




// bool OnMeleeHitHook::AddCondition(BGSKeyword* a_keyword)
// {
// 	if (!GetKeywordIndex(a_keyword)) {
// 		std::vector<BGSKeyword*> copiedData{ keywords, keywords + numKeywords };
// 		copiedData.emplace_back(a_keyword);
// 		CopyKeywords(copiedData);
// 		return true;
// 	}
// 	return false;
// }

// void OnMeleeHitHook::CopyEffect(const std::vector<RE::Effect*>& a_copiedData)
// {
// 	const auto oldData = keywords;

// 	const auto newSize = a_copiedData.size();
// 	const auto newData = calloc<RE::Effect*>(newSize);
// 	std::ranges::copy(a_copiedData, newData);

// 	numKeywords = static_cast<std::uint32_t>(newSize);
// 	keywords = newData;

// 	free(oldData);
// }

// auto spelldata = a_actor->As<RE::TESNPC>()->GetSpellList();
// auto numSpells = spelldata->numSpells;
// auto spells = spelldata->spells;
// std::vector<RE::SpellItem*> spellList{ spells, spells + numSpells };

// a_value = a_ini.GetAllValues(a_section, a_key, a_value);
// bool commented = false;
// for (auto it = a_value.begin(); it != a_value.end(); ++it) {
// 	if (it) {
// 		if (!commented) {
// 			commented = true;
// 			a_ini.SetValue(a_section, a_key, it, a_comment);
// 		} else {
// 			a_ini.SetValue(a_section, a_key, it);
// 		}
// 	}
// }

// RE::BSTArray<RE::TESForm *>::iterator position = std::find(a_active_list_1->forms.begin(), a_active_list_1->forms.end(), limbospell);
// if (position == a_active_list_1->forms.end())
// {
// 	a_active_list_1->AddForm(limbospell);
// }

// if (caster->GetCastingSource() == RE::MagicSystem::CastingSource::kLeftHand)
// {
// 	a_actor->NotifyAnimationGraph("BeginCastLeft");
// 	// a_actor->NotifyAnimationGraph("MLh_SpellFire_Event");
// }
// else
// {
// 	a_actor->NotifyAnimationGraph("BeginCastRight");
// 	// a_actor->NotifyAnimationGraph("MRh_SpellFire_Event");
// }

// if (a_actor->GetActorRuntimeData().currentProcess)
// {
// 	a_actor->GetActorRuntimeData().currentProcess->SetHeadtrackTarget(refr, refr->data.location);
// }
// caster->SpellCast(true, 1, a_spell);

// if (const auto slot = inv_spell->equipSlot)
// {
// 	auto H = RE::TESDataHandler::GetSingleton();
// 	bool left = false;
// 	if (const auto lefthand = H->LookupForm<RE::BGSEquipSlot>(0x13F43, "Skyrim.esm"))
// 	{
// 		if(slot == lefthand){
// 			left = true;
// 		}
// 	}

// 	RE::BSFixedString spellname = inv_spell->fullName;
// 	if (!spellname.empty())
// 	{
// 		//logger::info("Name {} Spellfullname {}"sv, a_actor->GetName(), spellname);
// 		bool added = false;
// 		switch (hash(spellname.c_str(), spellname.size()))
// 		{
// 		case "Fireball"_h:

// 			if(left || spellname.contains("LeftHand")){
// 				if (!a_actor->HasSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("NSV_FireballLeftHand")))
// 				{
// 					added = true;
// 					a_actor->AddSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("NSV_FireballLeftHand"));
// 				}
// 			}else if(spellname.contains("RightHand")){
// 				if (!a_actor->HasSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("NSV_FireballRightHand")))
// 				{
// 					added = true;
// 					a_actor->AddSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("NSV_FireballRightHand"));
// 				}
// 			}else{
// 				if (!a_actor->HasSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("NSV_Fireball")))
// 				{
// 					added = true;
// 					a_actor->AddSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("NSV_Fireball"));
// 				}
// 			}
// 			break;

// 		case "Flames"_h:

// 			if (left || spellname.contains("LeftHand"))
// 			{
// 				if (!a_actor->HasSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("NSV_FlamesLeftHand")))
// 				{
// 					added = true;
// 					a_actor->AddSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("NSV_FlamesLeftHand"));
// 				}
// 			}else if (spellname.contains("RightHand"))
// 			{
// 				if (!a_actor->HasSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("NSV_FlamesRightHand")))
// 				{
// 					added = true;
// 					a_actor->AddSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("NSV_FlamesRightHand"));
// 				}
// 			}
// 			else
// 			{
// 				if (!a_actor->HasSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("NSV_Flames")))
// 				{
// 					added = true;
// 					a_actor->AddSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("NSV_Flames"));
// 				}
// 			}
// 			break;

// 		case "Firebolt"_h:

// 			if (left || spellname.contains("LeftHand"))
// 			{
// 				if (!a_actor->HasSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("NSV_FireboltLeftHand")))
// 				{
// 					added = true;
// 					a_actor->AddSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("NSV_FireboltLeftHand"));
// 				}
// 			}else if (spellname.contains("RightHand"))
// 			{
// 				if (!a_actor->HasSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("NSV_FireboltRightHand")))
// 				{
// 					added = true;
// 					a_actor->AddSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("NSV_FireboltRightHand"));
// 				}
// 			}
// 			else
// 			{
// 				if (!a_actor->HasSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("NSV_Firebolt")))
// 				{
// 					added = true;
// 					a_actor->AddSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("NSV_Firebolt"));
// 				}
// 			}
// 			break;

// 		case "Incinerate"_h:

// 			if (left || spellname.contains("LeftHand"))
// 			{
// 				if (!a_actor->HasSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("NSV_IncinerateLeftHand")))
// 				{
// 					added = true;
// 					a_actor->AddSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("NSV_IncinerateLeftHand"));
// 				}
// 			}
// 			else if (spellname.contains("RightHand"))
// 			{
// 				if (!a_actor->HasSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("NSV_IncinerateRightHand")))
// 				{
// 					added = true;
// 					a_actor->AddSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("NSV_IncinerateRightHand"));
// 				}
// 			}
// 			else
// 			{
// 				if (!a_actor->HasSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("NSV_Incinerate")))
// 				{
// 					added = true;
// 					a_actor->AddSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("NSV_Incinerate"));
// 				}
// 			}
// 			break;

// 		case "Wall of Flames"_h:

// 			if (left || spellname.contains("LeftHand"))
// 			{
// 				if (!a_actor->HasSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("NSV_WallOfFlamesLeftHand")))
// 				{
// 					added = true;
// 					a_actor->AddSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("NSV_WallOfFlamesLeftHand"));
// 				}
// 			}
// 			else
// 			{
// 				if (!a_actor->HasSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("NSV_WallOfFlames")))
// 				{
// 					added = true;
// 					a_actor->AddSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("NSV_WallOfFlames"));
// 				}
// 			}
// 			break;

// 		case "Frostbite"_h:

// 			if (left || spellname.contains("LeftHand"))
// 			{
// 				if (!a_actor->HasSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("NSV_FrostbiteLeftHand")))
// 				{
// 					added = true;
// 					a_actor->AddSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("NSV_FrostbiteLeftHand"));
// 				}
// 			}
// 			else if (spellname.contains("RightHand"))
// 			{
// 				if (!a_actor->HasSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("NSV_FrostbiteRightHand")))
// 				{
// 					added = true;
// 					a_actor->AddSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("NSV_FrostbiteRightHand"));
// 				}
// 			}
// 			else
// 			{
// 				if (!a_actor->HasSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("NSV_Frostbite")))
// 				{
// 					added = true;
// 					a_actor->AddSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("NSV_Frostbite"));
// 				}
// 			}
// 			break;

// 		case "Ice Spike"_h:

// 			if (left || spellname.contains("LeftHand"))
// 			{
// 				if (!a_actor->HasSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("NSV_IceSpikeLeftHand")))
// 				{
// 					added = true;
// 					a_actor->AddSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("NSV_IceSpikeLeftHand"));
// 				}
// 			}
// 			else if (spellname.contains("RightHand"))
// 			{
// 				if (!a_actor->HasSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("NSV_IceSpikeRightHand")))
// 				{
// 					added = true;
// 					a_actor->AddSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("NSV_IceSpikeRightHand"));
// 				}
// 			}
// 			else
// 			{
// 				if (!a_actor->HasSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("NSV_IceSpike")))
// 				{
// 					added = true;
// 					a_actor->AddSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("NSV_IceSpike"));
// 				}
// 			}
// 			break;

// 		case "Ice Storm"_h:

// 			if (left || spellname.contains("LeftHand"))
// 			{
// 				if (!a_actor->HasSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("NSV_IceStormLeftHand")))
// 				{
// 					added = true;
// 					a_actor->AddSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("NSV_IceStormLeftHand"));
// 				}
// 			}
// 			else if (spellname.contains("RightHand"))
// 			{
// 				if (!a_actor->HasSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("NSV_IceStormRightHand")))
// 				{
// 					added = true;
// 					a_actor->AddSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("NSV_IceStormRightHand"));
// 				}
// 			}
// 			else
// 			{
// 				if (!a_actor->HasSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("NSV_IceStorm")))
// 				{
// 					added = true;
// 					a_actor->AddSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("NSV_IceStorm"));
// 				}
// 			}
// 			break;

// 		case "Icy Spear"_h:
// 			if (spellname.contains("RightHand"))
// 			{
// 				if (!a_actor->HasSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("NSV_IcySpearRightHand")))
// 				{
// 					added = true;
// 					a_actor->AddSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("NSV_IcySpearRightHand"));
// 				}
// 			}
// 			else
// 			{
// 				if (!a_actor->HasSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("NSV_IcySpear")))
// 				{
// 					added = true;
// 					a_actor->AddSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("NSV_IcySpear"));
// 				}
// 			}
// 			break;

// 		case "Wall of Frost"_h:

// 			if (left || spellname.contains("LeftHand"))
// 			{
// 				if (!a_actor->HasSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("NSV_WallOfFrostLeftHand")))
// 				{
// 					added = true;
// 					a_actor->AddSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("NSV_WallOfFrostLeftHand"));
// 				}
// 			}
// 			else
// 			{
// 				if (!a_actor->HasSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("NSV_WallOfFrost")))
// 				{
// 					added = true;
// 					a_actor->AddSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("NSV_WallOfFrost"));
// 				}
// 			}
// 			break;

// 		case "Lightning bolt"_h:

// 			if (!a_actor->HasSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("NSV_LightningBolt")))
// 			{
// 				added = true;
// 				a_actor->AddSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("NSV_LightningBolt"));
// 			}
// 			break;

// 		case "Sparks"_h:

// 			if (left || spellname.contains("LeftHand"))
// 			{
// 				if (!a_actor->HasSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("NSV_SparksLeftHand")))
// 				{
// 					added = true;
// 					a_actor->AddSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("NSV_SparksLeftHand"));
// 				}
// 			}
// 			else if (spellname.contains("RightHand"))
// 			{
// 				if (!a_actor->HasSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("NSV_SparksRightHand")))
// 				{
// 					added = true;
// 					a_actor->AddSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("NSV_SparksRightHand"));
// 				}
// 			}
// 			else
// 			{
// 				if (!a_actor->HasSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("NSV_Sparks")))
// 				{
// 					added = true;
// 					a_actor->AddSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("NSV_Sparks"));
// 				}
// 			}
// 			break;

// 		case "Chain Lightning"_h:

// 			if (left || spellname.contains("LeftHand"))
// 			{
// 				if (!a_actor->HasSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("NSV_ChainLightningLeftHand")))
// 				{
// 					added = true;
// 					a_actor->AddSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("NSV_ChainLightningLeftHand"));
// 				}
// 			}
// 			else if (spellname.contains("RightHand"))
// 			{
// 				if (!a_actor->HasSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("NSV_ChainLightningRightHand")))
// 				{
// 					added = true;
// 					a_actor->AddSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("NSV_ChainLightningRightHand"));
// 				}
// 			}
// 			else
// 			{
// 				if (!a_actor->HasSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("NSV_ChainLightning")))
// 				{
// 					added = true;
// 					a_actor->AddSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("NSV_ChainLightning"));
// 				}
// 			}
// 			break;

// 		case "Thunderbolt"_h:

// 			if (!a_actor->HasSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("NSV_Thunderbolt")))
// 			{
// 				added = true;
// 				a_actor->AddSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("NSV_Thunderbolt"));
// 			}
// 			break;

// 		case "Wall of Storms"_h:

// 			if (!a_actor->HasSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("NSV_WallOfStorms")))
// 			{
// 				added = true;
// 				a_actor->AddSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("NSV_WallOfStorms"));
// 			}
// 			break;

// 		case "Drain Life"_h:
// 		case "Vampiric Drain"_h:

// 			if (!a_actor->HasSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("NSV_crVampireDrain05")))
// 			{
// 				added = true;
// 				a_actor->AddSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("NSV_crVampireDrain05"));
// 			}
// 			break;

// 		default:
// 			// if (inv_spell->HasKeyword(NSV_Magic_Damage) || inv_spell->HasKeyword(NSV_Magic_Absorb))
// 			// {
// 			// 	//Clone_Spell(a_actor, inv_spell, NSV_Magic_Damage, NSV_Magic_Absorb);
// 			// }
// 			break;
// 		}

// 		if(added){
// 			logger::info("Name {} addedSpell {}"sv, a_actor->GetName(), spellname);
// 		}
// 	}
// }

// auto Effect_List = spell_copy->As<RE::SpellItem>()->effects;
// for (auto Effect : Effect_List)
// {
// 	if (Effect && Effect->baseEffect && Effect->baseEffect->data.aiScore && Effect->baseEffect->data.aiScore > 0.0)
// 	{
// 		std::string EDID = (clib_util::editorID::get_editorID(Effect->baseEffect));
// 		if (EDID.contains("NSV_"))
// 		{
// 			RE::BSTArray<RE::Effect *>::iterator e_position = std::find(Effect_List.begin(), Effect_List.end(), Effect);
// 			if (e_position != Effect_List.end())
// 			{
// 				Effect_List.erase(e_position);
// 			}
// 			spell_copies_list->AddForm(spell_copy);

// 			break;
// 		}
// 	}
// 	continue;
// }

// static auto fireKeyword = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("MagicDamageFire");
// static auto frostKeyword = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("MagicDamageFrost");
// static auto shockKeyword = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("MagicDamageShock");
// static auto healKeyword = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("MagicRestoreHealth");


// auto H = RE::TESDataHandler::GetSingleton();
// if (const auto spell_copies_list = H->LookupForm<RE::BGSListForm>(0x800, "NSV Spell Copies.esp"))
// {
// 	for (auto form : spell_copies_list->forms)
// 	{
// 		if (form && form->Is(RE::FormType::Spell))
// 		{
// 			auto s_spell = form->As<RE::SpellItem>();
// 			if (s_spell->fullName == e_spell->fullName)
// 			{
// 				if (s_spell->equipSlot == e_spell->equipSlot)
// 				{
// 					return;
// 				}
// 			}
// 		}
// 	}
// }

// effect->conditions.head = new RE::TESConditionItem;
// effect->conditions.head->data.functionData.function = RE::FUNCTION_DATA::FunctionID::kGetRandomPercent;
// effect->conditions.head->data.object = RE::CONDITIONITEMOBJECT::kSelf;
// bool less_than_or = false;
// if (OnMeleeHitHook::GetSingleton()->GenerateRandomInt(0/ 100) <= 50)
// {
// 	less_than_or = true;
// 	effect->conditions.head->data.flags.opCode = RE::CONDITION_ITEM_DATA::OpCode::kLessThanOrEqualTo;
// }else{
// 	effect->conditions.head->data.flags.opCode = RE::CONDITION_ITEM_DATA::OpCode::kGreaterThanOrEqualTo;
// }

// if (less_than_or){
// 	effect->conditions.head->data.comparisonValue.f = 2.0f;
// }else{
// 	effect->conditions.head->data.comparisonValue.f = 97.0f;
// }

// if (rSpell->GetCastingType() == RE::MagicSystem::CastingType::kFireAndForget)
// {
// 	if (const auto key_list = H->LookupForm<RE::BGSListForm>(0x832, "NPC Spell Variance.esp"))
// 	{
// 		if(rSpell->HasKeywordInList(key_list, false)){
// 			if (GetSingleton()->GenerateRandomDouble(0.0, 1.0) <= 0.3)
// 			{
//
// 			}
// 		}
// 	}
// 	else if (const auto perk_list = RE::TESForm::LookupByEditorID<RE::BGSListForm>("NSV_Perks_FormList"))
// 	{
// 		if (rSpell->data.castingPerk && perk_list->HasForm(rSpell->data.castingPerk))
// 		{
// 			if (GetSingleton()->GenerateRandomDouble(0.0, 1.0) <= 0.35)
// 			{
//
// 			}
// 		}
// 	}else{
// 		if (GetSingleton()->GenerateRandomDouble(0.0, 1.0) <= 0.45)
// 		{
//
// 		}
// 	}
// }else{
//
// }

// for (const auto equip_item : a_invent->unk118.items)
// {
// 	if (equip_item && equip_item.get())
// 	{
// 		logger::info("actor: {} itemSet1: {} scoreSet1: {:.2f} slotSet1: {} equipSlot1: {} equipUns1: {}"sv, a_controller->actorHandle.get().get()->GetName(), equip_item.get()->item->GetName(), a_invent->unk118.score, a_invent->unk118.slot, clib_util::editorID::get_editorID(equip_item.get()->itemSlot.equipSlot), equip_item.get()->itemSlot.slot);
// 	}
// }

// for (const auto equip_item : a_invent->unk148.items)
// {
// 	if (equip_item && equip_item.get())
// 	{
// 		logger::info("actor: {} itemSet2: {} scoreSet2: {:.2f} slotSet2: {} equipSlot2: {} equipUns2: {}"sv, a_controller->actorHandle.get().get()->GetName(), equip_item.get()->item->GetName(), a_invent->unk148.score, a_invent->unk148.slot, clib_util::editorID::get_editorID(equip_item.get()->itemSlot.equipSlot), equip_item.get()->itemSlot.slot);
// 	}
// }

// std::count(std::begin(arr), std::end(arr), value).

// std::vector<RE::SpellItem *>::iterator position = std::find(shouldhave_spelllist.begin(), shouldhave_spelllist.end(), s_spell);
// if (position == shouldhave_spelllist.end())
// {
// 	a_actor->RemoveSpell(s_spell);
// 	logger::info("N: {} removing non-matching spell: {}"sv, a_actor->GetName(), s_spell->fullName);
// }
// else
// {
// 	if (!s_spell->HasKeywordInList(clone_key_list, false))
// 	{
// 		a_actor->RemoveSpell(s_spell);
// 		logger::info("N: {} clone keyword not found removing spell: {}"sv, a_actor->GetName(), s_spell->fullName);
// 	}
// }



// for (auto &invent_item : *a_invent->inventoryItems)
// {
// 	if (invent_item && invent_item.get() && invent_item.get()->item)
// 	{
// 		int counter = 0;
// 		RE::NiPointer<RE::CombatInventoryItem> *position1 = nullptr;
// 		RE::NiPointer<RE::CombatInventoryItem> *position2 = nullptr;

// 		for (auto &alt_item : *a_invent->inventoryItems)
// 		{
// 			if (alt_item && alt_item.get() && alt_item.get()->item && alt_item.get()->item == invent_item.get()->item)
// 			{
// 				counter++;
// 				switch (counter)
// 				{
// 				case 1:
// 					position1 = &alt_item;
// 					break;

// 				case 2:
// 					position2 = &alt_item;
// 					break;
// 				}
// 			}
// 			if(counter > 2){
// 				break;
// 			}
// 		}

// 		if (counter <= 1)
// 		{
// 			continue;
// 		}
// 		else
// 		{
// 			if (position1->get()->itemScore <= 0.0f || position2->get()->itemScore <= 0.0f)
// 			{
// 				continue;
// 			}

// 			switch (GetSingleton()->GenerateRandomInt(1, 2))
// 			{
// 			case 0:
// 				/* code */
// 				break;

// 			case 1:
// 				position1->get()->itemScore = 0.0f;
// 				break;

// 			case 2:
// 				position2->get()->itemScore = 0.0f;
// 				break;
// 			}

// 		}
// 	}
// }

// for (const auto &book : dataHandler->GetFormArray<RE::TESObjectBOOK>())
// {
// 	if (book)
// 	{
// 		const auto spell = book->GetSpell();
// 		if (!spell)
// 		{
// 			continue;
// 		}
// 		playable_spells_list->AddForm(spell);
// 	}
// }

// if (const auto NSV_Copies = dataHandler->LookupModByName("NSV Spell Copies.esp"); NSV_Copies)
// {

// }

// if (const auto spell_copies_list = dataHandler->LookupForm<RE::BGSListForm>(0x800, "NSV Spell Copies.esp"); spell_copies_list)
// {
// 	//spell_copies_list->forms.clear();
// 	for (const auto &form : spell_copies_list->forms)
// 	{
// 		if(form){
// 			logger::info("existing form found. EDID: {} Name: {}"sv, clib_util::editorID::get_editorID(form), form->GetName());
// 		}
// 	}
// }

// if(espFound){

// 	if (const auto newform = dataHandler->LookupForm<RE::TESForm>(0x801, "NSV Spell Copies.esp"); spell_copies_list)
// 	{
// 	}

// 	auto firstid = static_cast<int>(id);

// 	logger::info("firstID: {}"sv, firstid);

// 	auto val = firstid += 16775168;

// 	for (auto &form : dataHandler->GetFormArray<RE::SpellItem>())
// 	{
// 		if (!form)
// 		{
// 			continue;
// 		}

// 		if (static_cast<int>(form->formID) < firstid && static_cast<int>(form->formID) >= val)
// 		{
// 			continue;
// 		}

// 		//logger::info("existing form found. ID: {}"sv, static_cast<int>(form->formID));
// 		logger::info("existing form found. EDID: {} Name: {}"sv, clib_util::editorID::get_editorID(form), form->GetName());
// 	}
// }

// const auto eitherhand = H->LookupForm<RE::BGSEquipSlot>(0x13F44, "Skyrim.esm");
// const auto bothhands = H->LookupForm<RE::BGSEquipSlot>(0x13F45, "Skyrim.esm");

// if (a_actor->HasKeywordString("NSV_SpellSword"))
// {
// 	if (a_actor->HasKeywordString("NSV_VampireLord"))
// 	{
// 		if (position1.get()->item->HasKeywordInList(Mod_Invent_SS_VL, false) || position2.get()->item->HasKeywordInList(Mod_Invent_SS_VL, false))
// 		{
// 			// do nothing to ascendant spells
// 		}
// 		else
// 		{
// 			if (position1.get()->item->HasKeywordInList(Mod_Invent_Right, true) || position2.get()->item->HasKeywordInList(Mod_Invent_Right, true))
// 			{
// 				position1.get()->itemSlot.slot = 1;
// 				position2.get()->itemSlot.slot = 1;

// 				position1.get()->itemSlot.equipSlot = righthand;
// 				position2.get()->itemSlot.equipSlot = righthand;
// 			}
// 			else
// 			{
// 				position1.get()->itemSlot.slot = 2;
// 				position2.get()->itemSlot.slot = 2;

// 				position1.get()->itemSlot.equipSlot = lefthand;
// 				position2.get()->itemSlot.equipSlot = lefthand;
// 			}
// 		}
// 	}
// 	else
// 	{
// 		if (position1.get()->item->HasKeywordInList(Mod_Invent_Right, true) || position2.get()->item->HasKeywordInList(Mod_Invent_Right, true))
// 		{
// 			position1.get()->itemSlot.slot = 1;
// 			position2.get()->itemSlot.slot = 1;

// 			position1.get()->itemSlot.equipSlot = righthand;
// 			position2.get()->itemSlot.equipSlot = righthand;
// 		}
// 		else
// 		{
// 			position1.get()->itemSlot.slot = 2;
// 			position2.get()->itemSlot.slot = 2;

// 			position1.get()->itemSlot.equipSlot = lefthand;
// 			position2.get()->itemSlot.equipSlot = lefthand;
// 		}
// 	}
// }
// else
// {
// 	if (position1.get()->item->HasKeywordInList(Mod_Invent_Right, false) || position2.get()->item->HasKeywordInList(Mod_Invent_Right, false))
// 	{

// 		position1.get()->itemSlot.slot = 1;
// 		position2.get()->itemSlot.slot = 1;

// 		position1.get()->itemSlot.equipSlot = righthand;
// 		position2.get()->itemSlot.equipSlot = righthand;
// 	}
// 	else if (position1.get()->item->HasKeywordInList(Mod_Invent_Left, false) || position2.get()->item->HasKeywordInList(Mod_Invent_Left, false))
// 	{
// 		position1.get()->itemSlot.slot = 2;
// 		position2.get()->itemSlot.slot = 2;

// 		position1.get()->itemSlot.equipSlot = lefthand;
// 		position2.get()->itemSlot.equipSlot = lefthand;
// 	}
// 	else
// 	{

// 		switch (GetSingleton()->GenerateRandomInt(1, 2))
// 		{
// 		case 1:
// 			position1.get()->itemSlot.slot = 1;
// 			position2.get()->itemSlot.slot = 1;

// 			position1.get()->itemSlot.equipSlot = righthand;
// 			position2.get()->itemSlot.equipSlot = righthand;

// 			break;

// 		case 2:
// 			position1.get()->itemSlot.slot = 2;
// 			position2.get()->itemSlot.slot = 2;

// 			position1.get()->itemSlot.equipSlot = lefthand;
// 			position2.get()->itemSlot.equipSlot = lefthand;
// 			break;
// 		}
// 	}
// }

// if (position1.get()->item->HasKeywordInList(Mod_Invent_SS_VL, false) || position2.get()->item->HasKeywordInList(Mod_Invent_SS_VL, false))
// {
// 	// do nothing to ascendant spells
// }
// else
// {
// 	//(position1.get()->itemSlot.equipSlot == position2.get()->itemSlot.equipSlot)

// 	if (position1.get()->item->HasKeywordInList(Mod_Invent_Left, false) || position2.get()->item->HasKeywordInList(Mod_Invent_Left, false))
// 	{
// 		position1.get()->itemSlot.slot = 2;
// 		position2.get()->itemSlot.slot = 2;

// 		position1.get()->itemSlot.equipSlot = lefthand;
// 		position2.get()->itemSlot.equipSlot = lefthand;
// 	}
// 	else if (position1.get()->item->HasKeywordInList(Mod_Invent_Right, false) || position2.get()->item->HasKeywordInList(Mod_Invent_Right, false))
// 	{

// 		position1.get()->itemSlot.slot = 1;
// 		position2.get()->itemSlot.slot = 1;

// 		position1.get()->itemSlot.equipSlot = righthand;
// 		position2.get()->itemSlot.equipSlot = righthand;
// 	}
// 	else
// 	{
// 		if (position1.get()->item->As<RE::SpellItem>())
// 		{
// 			switch (position1.get()->item->As<RE::SpellItem>()->GetCastingType())
// 			{
// 			case RE::MagicSystem::CastingType::kConcentration:

// 				position1.get()->itemSlot.slot = 1;
// 				position2.get()->itemSlot.slot = 1;

// 				position1.get()->itemSlot.equipSlot = righthand;
// 				position2.get()->itemSlot.equipSlot = righthand;
// 				break;

// 			case RE::MagicSystem::CastingType::kFireAndForget:

// 				position1.get()->itemSlot.slot = 2;
// 				position2.get()->itemSlot.slot = 2;

// 				position1.get()->itemSlot.equipSlot = lefthand;
// 				position2.get()->itemSlot.equipSlot = lefthand;
// 				break;

// 			default:
// 				break;
// 			}
// 		}
// 	}
// }

// OnMeleeHitHook::GetEquipped(a_actor, a_spell)
// logger::info("actor: {} RefCount: {}"sv, a_controller->actorHandle.get().get()->GetName(), static_cast<int>(position1.get()->GetRefCount()));
// logger::info("actor: {} unk1C: {} item: {}"sv, a_controller->actorHandle.get().get()->GetName(), position2.get()->unk1C, position2.get()->item->GetName());
// position2.get()->item = nullptr;

// if (shouldEquip)
// {
// 	if (OnMeleeHitHook::GetEquipped(a_actor, a_spell))
// 	{
// 		bool slot = a_slot == lefthand ? true : false;

// 		if (OnMeleeHitHook::HasSpellEquipped(a_actor, slot))
// 		{
// 			shouldEquip = false;

// 		}else
// 		{
// 			shouldEquip = false;
// 			alt_spell = OnMeleeHitHook::GetSingleton()->get_alt_spell(a_actor, a_spell, a_slot);
// 		}
// 	}

// }else
// {
// 	OnMeleeHitHook::GetSingleton()->register_inv_spell(a_actor, std::make_pair(a_spell, a_slot));
// }

// if (shouldEquip)
// {
// 	bool slot = a_slot == lefthand ? true : false;

// 	if (const auto c_spell = OnMeleeHitHook::GetSingleton()->GetEquippedSpell(a_actor, slot); c_spell)
// 	{
// 		RE::ActorEquipManager::GetSingleton()->UnequipObject(a_actor, c_spell);
// 	}

// }

// a_actor->HasKeywordString("NSV_ModCombatInvent_Exclude")
// OnMeleeHitHook::GetEquipped(a_actor, a_spell)

// if (a_actor->HasKeywordString("NSV_SpellSword"))
// {
// 	position1.get()->itemSlot.slot = 2;

// 	position1.get()->itemSlot.equipSlot = lefthand;
// }
// else
// {

// 	switch (GetSingleton()->GenerateRandomInt(1, 2))
// 	{
// 	case 1:
// 		position1.get()->itemSlot.slot = 1;

// 		position1.get()->itemSlot.equipSlot = righthand;

// 		break;

// 	case 2:
// 		position1.get()->itemSlot.slot = 2;

// 		position1.get()->itemSlot.equipSlot = lefthand;

// 		break;
// 	}
// }