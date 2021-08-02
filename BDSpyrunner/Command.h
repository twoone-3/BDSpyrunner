#pragma once
#include "pch.h"
#include <optional>
#include <tuple>
using type_id = uint16_t;
struct CommandRegistry;
struct CommandOrigin;
struct CommandOutput;
struct CommandParameterData;
enum struct AbilitiesIndex : int;

namespace {
extern CommandRegistry* CmdRegGlobal;
extern unordered_map<string, void*> parse_ptr;
}
// enum CommandPermissionLevel : char;
enum CommandPermissionLevel : char {
	Normal = 0,
	Privileged = 1,
	AutomationPlayer = 2,
	OperatorOnly = 3,
	ConsoleOnly = 4
};
template <typename T>
struct CommandSelectorResults;

enum struct CommandFlagValue : char {
	None = 0,
	Usage = 1,
	Visibility2 = 2,
	Visibility4 = 4,
	Visibility6 = 6,
	Sync = 8,
	Execute = 16,
	Type = 32,
	Cheat = 64,
};

struct CommandFlag {
	CommandFlagValue value;
};

struct CommandMessage {
	char filler[32];
public:
	string get(CommandOrigin const& x) {
		string(CommandMessage:: * rv)(CommandOrigin const&);
		*(void**)&rv =
			SYM("?getMessage@CommandMessage@@QEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$"
				"allocator@D@2@@std@@AEBVCommandOrigin@@@Z");
		return (this->*rv)(x);
	}
};

struct CommandVersion {
public:
	int Min = 1, Max = 0x7FFFFFFF;
	//  CommandVersion() : min(1), max(0x7FFFFFFF) {}
	//  CommandVersion(int min, int max) : min(min), max(max) {}
};

struct Command {
protected:
	int unk8;           // 8
	void* unk16;        // 16
	int unk24;          // 24
	unsigned char b28;  // 28
	CommandFlag flag;   // 29
	Command() {
		unk24 = -1;
		b28 = 5;
	}

public:
	virtual ~Command() { SymCall("??1Command@@UEAA@XZ", this); }
	virtual void execute(CommandOrigin const&, CommandOutput&) = 0;
	template <typename T>
	static bool checkHasTargets(CommandSelectorResults<T> const& a, CommandOutput& b) {
		bool (*sym)(CommandSelectorResults<T> const& a, CommandOutput & b);
		if constexpr (is_same<T, struct Actor>()) {
			sym = (decltype(sym))SYM(
				"??$checkHasTargets@VActor@@@Command@@KA_NAEBV?$CommandSelectorResults@VActor@@@@"
				"AEAVCommandOutput@@@Z");
		}
		else {
			sym = (decltype(sym))SYM(
				"??$checkHasTargets@VPlayer@@@Command@@KA_NAEBV?$CommandSelectorResults@VPlayer@@@@"
				"AEAVCommandOutput@@@Z");
		}
		return sym(a, b);
	}
};
enum struct CommandParameterDataType {
	NORMAL,
	ENUM,
	SOFT_ENUM
};
struct CommandRegistry {
#pragma region struct
	struct ParseToken;
	struct Symbol {
		unsigned val;
	};
	struct Overload {
		using FactoryFn = unique_ptr<Command>(*)(void);
		CommandVersion version;                    // 0
		FactoryFn factory;                         // 8
		vector<CommandParameterData> params;  // 16
		unsigned char unk;                         // 40
		Overload(CommandVersion version,
			FactoryFn factory,
			vector<CommandParameterData>&& args)
			: version(version),
			factory(factory),
			params(forward<vector<CommandParameterData>>(args)),
			unk(255) {
		}
	};
	struct Signature {
		string name;                                  // 0
		string desc;                                  // 32
		vector<CommandRegistry::Overload> overloads;  // 64
		CommandPermissionLevel perm;                       // 88
		CommandRegistry::Symbol main_symbol;               // 92
		CommandRegistry::Symbol alt_symbol;                // 96
		CommandFlag flag;                                  // 100
		int unk72;
		int unk76;
		int unk80;
		bool b84;

		Signature(string_view name,
			string_view desc,
			CommandPermissionLevel perm,
			CommandRegistry::Symbol symbol,
			CommandFlag flag)
			: name(name), desc(desc), perm(perm), main_symbol(symbol), flag(flag) {
		}
	};

	struct SoftEnum {
		string name;		// 0
		vector<string> list;// 32
	};
#pragma endregion struct definition
	/*
	 void registerCommand(
			const string& a, char const* b, CommandPermissionLevel c, CommandFlag d,
	CommandFlag e) {
			SymCall("?registerCommand@CommandRegistry@@QEAAXAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@PEBDW4CommandPermissionLevel@@UCommandFlag@@3@Z",
					void, void*, const string&, char const*, char, char, char)
					(this, string("test"), "testcmd", 0, 0, 0x40);
			//SymCall("?registerCommand@CommandRegistry@@QEAAXAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@PEBDW4CommandPermissionLevel@@UCommandFlag@@3@Z",
	void, void*, const string&, char const*, char, char, char)(this, a, b, 0, 0, 0);
	}*/
	void registerAlias(const string& a, const string& b) {
		SymCall<void, void*, const string&, const string&>("?registerAlias@CommandRegistry@@QEAAXV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@0@Z",
			this, a, b);
	}
	Signature* findCommand(const string& cmd) {
		return SymCall<Signature*>("?findCommand@CommandRegistry@@AEAAPEAUSignature@1@AEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@Z",
			this, &cmd);
	}
	void registerOverloadInternal(CommandRegistry::Signature* a0, struct CommandRegistry::Overload* a1) {
		SymCall("?registerOverloadInternal@CommandRegistry@@AEAAXAEAUSignature@1@AEAUOverload@1@@Z",
			this, a0, a1);
	}
	Symbol addEnumValuesInternal(const string& a0, const vector<pair<string, uint64_t>>& a1, type_id a2,
		bool (CommandRegistry::* a3)(void*, CommandRegistry::ParseToken const&, CommandOrigin const&, int, string&, vector<string>&) const) {
		return SymCall<Symbol>("?_addEnumValuesInternal@CommandRegistry@@AEAA?AVSymbol@1@AEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBV?$vector@U?$pair@_K_K@std@@V?$allocator@U?$pair@_K_K@std@@@2@@4@V?$typeid_t@VCommandRegistry@@@@P81@EBA_NPEAXAEBUParseToken@1@AEBVCommandOrigin@@HAEAV34@AEAV?$vector@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$allocator@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@2@@4@@Z@Z",
			this, &a0, &a1, a2, a3);
	}
	unsigned __int64 getEnumData(struct CommandRegistry::ParseToken const& a0) const {
		unsigned __int64 (CommandRegistry:: * rv)(struct CommandRegistry::ParseToken const&) const;
		*((void**)&rv) = SYM("?getEnumData@CommandRegistry@@AEBA_KAEBUParseToken@1@@Z");
		return (this->*rv)(a0);
	}

public:
	template <typename T>
	static auto getParseFn() {
		bool (CommandRegistry:: * ptr)(void*, CommandRegistry::ParseToken const&,
			CommandOrigin const&, int, string&,
			vector<string> &) const;
		*(void**)&ptr = parse_ptr[string(typeid(T).name())];
		if (!ptr) {
			printf("Cant parse cmd data %s\n", typeid(T).name());
			this_thread::sleep_for(chrono::seconds(10));
			exit(1);
		}
		return ptr;
	}

	template <typename Type>
	bool fakeparse(void*,
		ParseToken const&,
		CommandOrigin const&,
		int,
		string&,
		vector<string>&) const {
		return false;
	}
	bool parseEnumInt(void* target,
		CommandRegistry::ParseToken const& token,
		CommandOrigin const&,
		int,
		string&,
		vector<string>&) const {
		auto data = getEnumData(token);
		*(int*)target = (int)data;
		return true;
	}
	unsigned addEnumValues(const string& name,
		type_id tid,
		initializer_list<string> const& values) {
		vector<pair<string, uint64_t>> converted;
		uint64_t idx = 0;
		for (auto& value : values)
			converted.emplace_back(value, ++idx);
		return addEnumValuesInternal(name, converted, tid, &CommandRegistry::parseEnumInt).val;
	}
	void registerOverload(const string& name,
		Overload::FactoryFn factory,
		vector<CommandParameterData>&& args) {
		Signature* signature = const_cast<Signature*>(findCommand(name));
		if (!signature) {
			printf("no command found %s", name.c_str());
			exit(0);
		}
		auto& overload = signature->overloads.emplace_back(
			CommandVersion{}, factory, forward<vector<CommandParameterData>>(args));
		registerOverloadInternal(signature, &overload);
	}
};

struct CommandParameterData {
	using ParseFn = bool (CommandRegistry::*)(void*,
		CommandRegistry::ParseToken const&,
		CommandOrigin const&,
		int,
		string&,
		vector<string>&) const;
	type_id tid;  // 0
	ParseFn parser;                 // 8
	string name;               // 16
	char const* desc;               // 48
	int unk56;                      // 56
	CommandParameterDataType type;  // 60
	int offset;                     // 64
	int flag_offset;                // 68
	bool mand;                      // 72
	bool pad73;                     // 73
	CommandParameterData() : tid(0) {}
	CommandParameterData(type_id tid,
		ParseFn parser,
		string_view desc_,
		CommandParameterDataType type,
		char const* enumname,
		int offset,
		bool optional,
		int flag_offset)
		: tid(tid),
		parser(parser),
		name(desc_),
		desc(enumname),
		unk56(-1),
		type(type),
		offset(offset),
		flag_offset(flag_offset),
		mand(optional),
		pad73(false) {
	}
};
static_assert(offsetof(CommandParameterData, pad73) == 73);
static_assert(offsetof(CommandParameterData, name) == 16);
static_assert(offsetof(CommandParameterData, unk56) == 56);

//struct MCRESULT {
//	unsigned char filler[4];
//	operator bool() { return filler[0]; }
//	bool isSuccess() { return operator bool(); }
//};
//struct CommandContext {
//	string CMD;
//	CommandOrigin* Ori;
//	int Version = 10;
//	string& getCmd() { return CMD; }
//	struct CommandOrigin& getOrigin() {
//		return *Ori;
//	}
//	template <typename TP>
//	CommandContext(TP&& x, CommandOrigin* o) : CMD(forward<TP>(x)), Ori(o) {}
//};
//static_assert(offsetof(CommandContext, Ori) == 32);

namespace mce {
struct UUID;
};

struct NetworkIdentifier {
	char filler[144];
};
namespace Util {
struct HashString;
};
struct CommandOutputParameter {
	string str;
	int type;
	//__declspec(dllimport) CommandOutputParameter(const string&);
	//__declspec(dllimport) CommandOutputParameter(int);
	//__declspec(dllimport) CommandOutputParameter(BlockPos);
	//__declspec(dllimport) CommandOutputParameter(vector<Player const*> const&);
	//__declspec(dllimport) CommandOutputParameter(vector<string> const&);
	CommandOutputParameter(string str, int type) : str(str), type(type) {}
};
struct CommandOutput {
	void error(const string& a0, const vector<CommandOutputParameter>& a1 = {}) {
		SymCall("?error@CommandOutput@@QEAAXAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBV?$vector@VCommandOutputParameter@@V?$allocator@VCommandOutputParameter@@@std@@@3@@Z",
			this, &a0, &a1);
	}
	void success(const string& a0 , const vector<CommandOutputParameter>& a1 = {}) {
		SymCall("?success@CommandOutput@@QEAAXAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBV?$vector@VCommandOutputParameter@@V?$allocator@VCommandOutputParameter@@@std@@@3@@Z",
			this, &a0, &a1);
	}
	void addMessage(const string& a0 , const vector<CommandOutputParameter>& a1 = {}) {
		SymCall("?addMessage@CommandOutput@@AEAAXAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBV?$vector@VCommandOutputParameter@@V?$allocator@VCommandOutputParameter@@@std@@@3@W4CommandOutputMessageType@@@Z",
			this, &a0, &a1, 0);
	}
};

enum struct OriginType : char {
	Player = 0,
	Block = 1,
	MinecartBlock = 2,
	DevConsole = 3,
	Test = 4,
	AutomationPlayer = 5,
	ClientAutomation = 6,
	DedicatedServer = 7,
	Actor = 8,
	Virtual = 9,
	GameArgument = 10,
	ActorServer = 11
};

struct CommandOrigin {
	CommandOrigin(CommandOrigin const&) = delete;
	virtual ~CommandOrigin() = 0;
	virtual string& getRequestId() const = 0;
	virtual string getName() const = 0;
	virtual BlockPos getBlockPosition() const = 0;
	virtual Vec3 getWorldPosition() const = 0;
	virtual Level* getLevel() const = 0;
	virtual struct Dimension* getDimension() const = 0;
	virtual Actor* getEntity() const = 0;
	virtual enum CommandPermissionLevel getPermissionsLevel() const = 0;
	virtual unique_ptr<CommandOrigin> clone() const = 0;
	virtual bool unk1() const = 0;
	virtual bool unk2() const = 0;
	virtual bool hasChatPerms() const = 0;
	virtual bool hasTellPerms() const = 0;
	virtual bool canUseAbility(AbilitiesIndex) const = 0;
	virtual bool isWorldBuilder() const = 0;
	virtual bool canUseCommandsWithoutCheatsEnabled() const = 0;
	virtual bool isSelectorExpansionAllowed() const = 0;
	virtual NetworkIdentifier getSourceId_net() const = 0;
	virtual char getSourceSubId() const = 0;
	virtual CommandOrigin* getOutputReceiver() const = 0;
	virtual OriginType getOriginType() const = 0;
	virtual mce::UUID const& getUUID() const = 0;
	virtual void toCommandOriginData_stub() const = 0;
	virtual void handleCommandOutputCallback_stub(void*) = 0;
	virtual void _setUUID(mce::UUID const&) = 0;
};
//template <typename T>
//struct InvertableFilter {
//	T value;
//	bool inverted;
//};
struct CommandSelectorBase {
	void* filler[192 / 8];
	~CommandSelectorBase() {
		SymCall("??1CommandSelectorBase@@QEAA@XZ", this);
	}
	// CommandSelectorBase() {}
protected:
	CommandSelectorBase(bool isPlayer) {
		SymCall("??0CommandSelectorBase@@IEAA@_N@Z", this, isPlayer);
	}
};
template <typename T>
struct CommandSelector : CommandSelectorBase {
	CommandSelector() : CommandSelectorBase(is_same_v<T, Player>) {}
	CommandSelectorResults<T> results(CommandOrigin const& a0) {
		CommandSelectorResults<T>(CommandSelector<T>:: * rv)(CommandOrigin const& a0);
		if constexpr (is_same_v<Actor, T>) {
			*((void**)&rv) = SYM(
				"?results@?$CommandSelector@VActor@@@@QEBA?AV?$CommandSelectorResults@VActor@@@@"
				"AEBVCommandOrigin@@@Z");
			return (this->*rv)(a0);
		}
		else {
			*((void**)&rv) = SYM(
				"?results@?$CommandSelector@VPlayer@@@@QEBA?AV?$CommandSelectorResults@VPlayer@@@@"
				"AEBVCommandOrigin@@@Z");
			return (this->*rv)(a0);
		}
	}
};
template <typename T>
struct CommandSelectorResults {
	shared_ptr<vector<T*>> data;
	auto begin() { return data->begin(); }
	auto end() { return data->end(); }
	auto count() const { return data->size(); }
	auto empty() const { return data->empty(); }
};
namespace CMDREG {
template <typename T>
type_id getTPID() {
	return 0;
};
type_id& GETID(const char* name) {
	return *(type_id*)(SYM(name));
}
type_id ALLOCID() {
	auto& id = *((unsigned short*)SYM("?count@?$typeid_t@VCommandRegistry@@@@2GA"));
	return { id++ };
}
template <>
type_id getTPID<CommandMessage>() {
	return GETID(
		"?id@?1???$type_id@VCommandRegistry@@VCommandMessage@@@@YA?AV?$typeid_t@VCommandRegistry@@@"
		"@XZ@4V1@A");
}
template <>
type_id getTPID<bool>() {
	return GETID(
		"?id@?1???$type_id@VCommandRegistry@@_N@@YA?AV?$typeid_t@VCommandRegistry@@@@XZ@4V1@A");
}
template <>
type_id getTPID<int>() {
	return GETID(
		"?id@?1???$type_id@VCommandRegistry@@H@@YA?AV?$typeid_t@VCommandRegistry@@@@XZ@4V1@A");
}
template <>
type_id getTPID<float>() {
	return GETID(
		"?id@?1???$type_id@VCommandRegistry@@M@@YA?AV?$typeid_t@VCommandRegistry@@@@XZ@4V1@A");
}
template <>
type_id getTPID<string>() {
	return GETID(
		"?id@?1???$type_id@VCommandRegistry@@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@"
		"2@@std@@@@YA?AV?$"
		"typeid_t@VCommandRegistry@@@@XZ@4V1@A");
}
template <>
type_id getTPID<CommandSelector<Actor>>() {
	return GETID(
		"?id@?1???$type_id@VCommandRegistry@@V?$CommandSelector@VActor@@@@@@YA?AV?$typeid_t@"
		"VCommandRegistry@@@@XZ@4V1@"
		"A");
}
template <>
type_id getTPID<CommandSelector<Player>>() {
	return GETID(
		"?id@?1???$type_id@VCommandRegistry@@V?$CommandSelector@VPlayer@@@@@@YA?AV?$typeid_t@"
		"VCommandRegistry@@@@XZ@4V1@"
		"A");
}
template <typename T>
struct CEnum {
	static type_id myid;
	static string name;
	CEnum(const string& nam, initializer_list<string> const& values) {
		name = nam;
		if (myid.value == 65535) {
			myid = ALLOCID();
			if (CmdRegGlobal == nullptr) {
				cout << "CmdRegGlobal Not Set" << endl;
				this_thread::sleep_for(chrono::seconds(10));
				exit(1);
			}
			CmdRegGlobal->addEnumValues(name, myid, values);
		}
	}
};
template <typename T>
type_id CEnum<T>::myid(65535);
template <typename T>
string CEnum<T>::name;

struct IMyEnum {};

template <typename E>
struct MyEnum : IMyEnum {
	E val;
	operator E() { return val; }
};
struct Ioptional {};
template <typename T>
type_id typeid_getter() {
	if constexpr (is_base_of_v<IMyEnum, T>) {
		return CEnum<decltype(T::val)>::myid;
	}
	else {
		if constexpr (is_base_of_v<Ioptional, T>) {
			return typeid_getter<typename T::val>();
		}
		else {
			return getTPID<T>();
		}
	}
}
template <typename Dummy, typename... TP>
struct MakeOverload {
	//参数
	using container = tuple<remove_cv_t<remove_reference_t<TP>>...>;
	//副类
	struct sub : Command {
		container data;
		static uintptr_t cb;
		template <size_t... Index>
		bool invoke_impl(CommandOrigin const& a, CommandOutput& b, index_sequence<Index...>) {
			return ((bool (*)(CommandOrigin const&, CommandOutput&, TP...))cb)(
				a, b, get<Index>(data)...);
		}
		void execute(CommandOrigin const& a, CommandOutput& b) {
			constexpr auto size = tuple_size<container>::value;
			try {
				if (invoke_impl(a, b, make_index_sequence<size>{})) {
					//b.success(string("Success"));
				}

			}
			catch (exception e) {
				// b.error(I18N::CMD_EXCEPTION+e.what());
			}
			catch (string e) {
				// b.error(I18N::CMD_EXCEPTION+e);
			}
			catch (...) {
				// b.error(I18N::CMD_EXCEPTION);
			}
		}
		sub() {}
	};
	//这里
	static void (*new_cb)(container&);
	template <typename TX>
	void reg_impl_sub(uintptr_t off, const string& desc, vector<CommandParameterData>& vc) {
		off += offsetof(sub, data);
		if constexpr (is_base_of_v<Ioptional, TX>) {
			using TXX = typename TX::Tval;
			if constexpr (is_base_of_v<IMyEnum, TXX>) {
				vc.emplace_back(typeid_getter<TXX>(), &CommandRegistry::parseEnumInt, desc,
					CommandParameterDataType::ENUM,
					CEnum<decltype(TXX::val)>::name.c_str(),
					int(off + offsetof(TX, filler) + offsetof(TXX, val)), true,
					int(off + offsetof(TX, set)));
			}
			else {
				vc.emplace_back(typeid_getter<TXX>(), CommandRegistry::getParseFn<TXX>(), desc,
					CommandParameterDataType::NORMAL, nullptr,
					int(off + offsetof(TX, filler)), true,
					int(off + offsetof(TX, set)));
			}
		}
		else {
			if constexpr (is_base_of_v<IMyEnum, TX>) {
				vc.emplace_back(typeid_getter<TX>(), &CommandRegistry::parseEnumInt, desc,
					CommandParameterDataType::ENUM,
					CEnum<decltype(TX::val)>::name.c_str(),
					int(off + offsetof(TX, val)), false, -1);
			}
			else {
				vc.emplace_back(typeid_getter<TX>(), CommandRegistry::getParseFn<TX>(), desc,
					CommandParameterDataType::NORMAL, nullptr, int(off), false, -1);
			}
		}
	}
	template <size_t... Index>
	void reg_helper(vector<CommandParameterData>& vc, vector<string>& argn, index_sequence<Index...>) {
		container& cter = *(container*)0;
		(reg_impl_sub<remove_cv_t<remove_reference_t<decltype(get<Index>(cter))>>>(
			(uintptr_t)addressof(get<Index>(cter)) - (uintptr_t)addressof(cter),
			argn[Index], vc),
			...);
	}
	void regMe(const string& cname, vector<CommandParameterData>&& vc) {
		if (CmdRegGlobal == nullptr) {
			cout << "CmdRegGlobal Not Set" << endl;
			exit(1);
		}
		CmdRegGlobal->registerOverload(cname, &factory,
			forward<vector<CommandParameterData>>(vc));
	}
	template <typename... TP2>
	MakeOverload(Dummy*, void (*_new_cb)(container&), const string& cname,
		bool (*__cb)(CommandOrigin const&, CommandOutput&, TP...), TP2... argns) {
		static_assert(sizeof...(TP2) == sizeof...(TP),
			"every command arg should have a description");
		new_cb = _new_cb;
		sub::cb = (decltype(sub::cb))__cb;
		vector<CommandParameterData> vc;
		vector<string> argn;
		(argn.emplace_back(argns), ...);
		constexpr auto size = tuple_size<container>::value;
		reg_helper(vc, argn, make_index_sequence<size>{});
		regMe(cname, move(vc));
	}
	template <typename... TP2>
	MakeOverload(Dummy*, const string& cname,
		bool (*__cb)(CommandOrigin const&, CommandOutput&, TP...), TP2... argns) {
		static_assert(sizeof...(TP2) == sizeof...(TP),
			"every command arg should have a description");
		sub::cb = (decltype(sub::cb))__cb;
		vector<CommandParameterData> vc;
		vector<string> argn;
		(argn.emplace_back(argns), ...);
		constexpr auto size = tuple_size<container>::value;
		reg_helper(vc, argn, make_index_sequence<size>{});
		regMe(cname, move(vc));
	}
	static unique_ptr<Command> factory() {
		auto rv = make_unique<sub>();
		if (new_cb)
			new_cb(rv->data);
		return rv;
	}
};
template <typename Dummy, typename... TP>
uintptr_t MakeOverload<Dummy, TP...>::sub::cb;
template <typename Dummy, typename... TP>
void (*MakeOverload<Dummy, TP...>::new_cb)(
	tuple<remove_cv_t<remove_reference_t<TP>>...>&) = nullptr;
};  // namespace CMDREG
using CMDREG::CEnum, CMDREG::MakeOverload, CMDREG::MyEnum;
static_assert(sizeof(MakeOverload<void, int>) == 1);

#define CmdOverload(name2, cb, ...) { MakeOverload __ov1((struct name2 *)0, string(#name2), cb, __VA_ARGS__); }
#define CmdOverload2(name2, cb, cb2, ...) { MakeOverload __ov2((struct name2 *)0, cb2, #name2, cb, __VA_ARGS__); }
