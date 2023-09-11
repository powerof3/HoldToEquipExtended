void PatchHoldToEquipTypes()
{
	{
		REL::Relocation<std::uintptr_t> target{ REL::Offset(0x0203F492) };

		struct HoldToEquipTypes_Code : Xbyak::CodeGenerator
		{
			HoldToEquipTypes_Code(std::uintptr_t a_address)
			{
				Xbyak::Label retnLabel;

				cmp(al, 35);  // books
				jz("ok");
				cmp(al, 34);  // armor
				jz("ok");
				cmp(al, 38);  // ingredient
				jz("ok");
				cmp(al, 48);  // weapons
				jz("ok");
				cmp(al, 54);  // alchemy
				jz("ok");

				jmp(ptr[rip + retnLabel]);

				L(retnLabel);
				dq(a_address + 0x8);

				L("ok");
				jmp(ptr[rip]);
				dq(a_address + 0x28);
			}
		};

		HoldToEquipTypes_Code code(target.address());
		code.ready();

		auto& trampoline = SFSE::GetTrampoline();
		trampoline.write_branch<6>(
			target.address(),
			trampoline.allocate(code));
	}

	logger::info("Patched Hold To Equip types");
}

void MessageCallback(SFSE::MessagingInterface::Message* a_msg) noexcept
{
	switch (a_msg->type) {
	case SFSE::MessagingInterface::kPostLoad:
		PatchHoldToEquipTypes();
		break;
	default:
		break;
	}
}

void InitializeLog()
{
	auto path = logger::log_directory();
	if (!path) {
		stl::report_and_fail("Failed to find standard logging directory"sv);
	}

	*path /= fmt::format(FMT_STRING("{}.log"), Version::PROJECT);
	auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);

	auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));

	log->set_level(spdlog::level::info);
	log->flush_on(spdlog::level::info);

	spdlog::set_default_logger(std::move(log));
	spdlog::set_pattern("[%H:%M:%S:%e] %v"s);

	logger::info(FMT_STRING("{} v{}"), Version::PROJECT, Version::NAME);
}

DLLEXPORT constinit auto SFSEPlugin_Version = []() noexcept {
	SFSE::PluginVersionData data{};

	data.PluginVersion(Version::MAJOR);
	data.PluginName(Version::PROJECT);
	data.AuthorName("powerofthree");
	data.UsesSigScanning(true);
	//data.UsesAddressLibrary(true);
	data.HasNoStructUse(true);
	//data.IsLayoutDependent(true);
	data.CompatibleVersions({ SFSE::RUNTIME_SF_1_7_23 });

	return data;
}();

DLLEXPORT bool SFSEAPI SFSEPlugin_Load(const SFSE::LoadInterface* a_sfse)
{
	InitializeLog();

	SFSE::Init(a_sfse);

	// do stuff
	SFSE::AllocTrampoline(56);

	SFSE::GetMessagingInterface()->RegisterListener(MessageCallback);

	return true;
}
