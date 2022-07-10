# pyr的代码补全文件
# Copyright (c) 2022 twoone3
# All rights reserved.
# last change: 2022.7.10

from enum import Enum


class TitleType(Enum):
	Clear = 0
	Reset = 1
	SetTitle = 2
	SetSubtitle = 3
	SetActionBar = 4
	SetDurations = 5
	TitleTextObject = 6
	SubtitleTextObject = 7
	ActionbarTextObject = 8


class TextType(Enum):
	RAW = 0
	CHAT = 1
	TRANSLATION = 2
	POPUP = 3
	JUKEBOX_POPUP = 4
	TIP = 5
	SYSTEM = 6
	WHISPER = 7
	ANNOUNCEMENT = 8
	JSON_WHISPER = 9
	JSON = 10


class Mirror(Enum):
	None_15 = 0
	X = 1
	Z = 2
	XZ = 3


class Rotation(Enum):
	None_14 = 0
	Rotate90 = 1
	Rotate180 = 2
	Rotate270 = 3
	Total = 4


class CommandPermissionLevel(Enum):
	Any = 0
	GameMasters = 1
	Admin = 2
	HostPlayer = 3
	Console = 4
	Internal = 5


class ParameterType(Enum):
	Bool = 0  # bool
	Int = 1  # int
	Float = 2  # float
	String = 3  # std::string
	Actor = 4  # CommandSelector<Actor>
	Player = 5  # CommandSelector<Player>
	BlockPos = 6  # CommandPosition
	Vec3 = 7  # CommandPositionFloat
	RawText = 8  # CommandRawText
	Message = 9  # CommandMessage
	JsonValue = 10  # Json::Value
	Item = 11  # CommandItem
	Block = 12  # Block const*
	Effect = 13  # MobEffect const*
	Enum = 14  # ENUM
	SoftEnum = 15  # SOFT_ENUM
	ActorType = 16  # ActorDefinitionIdentifier const*
	Command = 17  # std::unique_ptr<Command>
	WildcardSelector = 18  # WildcardCommandSelector<Actor>


class EffectChangedType(Enum):
	Add = 0
	Remove = 1
	Update = 2


class CommandParameterOption(Enum):
	None_ = 0  # TODO:make it avaliable
	EnumAutocompleteExpansion = 1
	HasSemanticConstraint = 2  # be used in block or item name enum
	EnumAsChainedCommand = 4  # be used in NewExecuteCommand


class GameType(Enum):
	Survival = 0
	Creative = 1
	Adventure = 2
	Spectator = 6


class SnbtFormat(Enum):
	PartialNewLine = 0
	AlwayNewLine = 1
	Minimize = 2


class ObjectiveSortOrder(Enum):
	Ascending = 0,
	Descending = 1


class BossEventColour(Enum):
	Grey = 0,
	Blue = 1,
	Red = 2,
	Green = 3,
	Yellow = 4,
	Purple = 5,
	White = 6


class ScriptFacing(Enum):
	Unknown = -1,
	Down = 0,
	Up = 1,
	North = 2,
	South = 3,
	West = 4,
	East = 5,


class TagType(Enum):
	End = 0
	Byte = 1
	Short = 2
	Int = 3
	Int64 = 4
	Float = 5
	Double = 6
	ByteArray = 7
	String = 8
	List = 9
	Compound = 10
	IntArray = 11


class Vec2:
	x: float
	y: float

	def __init__(self, x: float, y: float) -> None:
		pass

	def __repr__(self) -> str:
		pass


class Vec3:
	x: float
	y: float
	x: float

	def __init__(self, x: float, y: float, z: float) -> None:
		pass

	def __repr__(self) -> str:
		pass


class BlockPos:
	x: int
	y: int
	z: int

	def __init__(self, x: int, y: int, z: int) -> None:
		pass

	def __repr__(self) -> str:
		pass


class Block:
	name: str
	type: str
	id: str
	pos: BlockPos
	dim: int
	tile_data: int

	def setNbt(self, nbt):
		pass

	def getNbt(self) -> NBT:
		pass

	def hasContainer(self) -> bool:
		pass

	def getContainer(self) -> Container:
		pass

	def hasBlockEntity(self) -> bool:
		pass

	def getBlockEntity(self) -> BlockEntity:
		pass

	def removeBlockEntity(self) -> bool:
		pass


class Container:
	size: int
	type: str

	def addItem(self, item) -> bool:
		pass

	def addItemToFirstEmptySlot(self, item) -> bool:
		pass

	def hasRoomFor(self, item) -> bool:
		pass

	def removeItem(self, slot, number) -> bool:
		pass

	def getItem(self, slot) -> Item:
		pass

	def setItem(self, slot, item) -> bool:
		pass

	def getAllItems(self) -> list[Item]:
		pass

	def removeAllItems(self) -> bool:
		pass

	def isEmpty(self) -> bool:
		pass


class Item:
	name: str
	type: str
	id: int
	count: int
	aux: int

	def __init__(self):
		pass

	def set(self, other) -> bool:
		pass

	def clone(self) -> Item:
		pass

	def isNull(self) -> bool:
		pass

	def setNull(self) -> bool:
		pass

	def setAux(self, aux) -> bool:
		pass

	def setLore(self, lore: list[str]) -> bool:
		pass

	def setNbt(self, nbt) -> bool:
		pass

	def getNbt(self) -> NBT:
		pass


class Command:
	name: str

	def __init__(self, cmd: str, desc: str, level):
		pass

	def setEnum(self, name: str, enums: list[str]) -> str:
		pass

	def setAlias(self, alias: str) -> bool:
		pass

	def mandatory2(
	    self,
	    name,
	    type: ParameterType,
	    description: str = "",
	    identifier: str = "",
	    option=CommandParameterOption.None_
	) -> int:
		pass

	def mandatory(
	    self,
	    name,
	    type: ParameterType,
	    description: str = "",
	    option=CommandParameterOption.None_
	) -> int:
		pass

	def optional2(
	    self,
	    name,
	    type: ParameterType,
	    description: str = "",
	    identifier: str = "",
	    option=CommandParameterOption.None_
	) -> int:
		pass

	def optional(
	    self,
	    name,
	    type: ParameterType,
	    description: str = "",
	    option=CommandParameterOption.None_
	) -> int:
		pass

	def setSoftEnum(self, name, enums: list[str]) -> bool:
		pass

	def addSoftEnumValues(self, name, values: list[str]) -> bool:
		pass

	def removeSoftEnumValues(self, name, values: list[str]) -> bool:
		pass

	def getSoftEnumValues(self, name) -> list[str]:
		pass

	def getSoftEnumNames(self) -> list[str]:
		pass

	def overload(self, value=None) -> bool:
		pass

	def setCallback(self, callback) -> bool:
		pass

	def setup(self) -> bool:
		pass


class CommandOrigin:
	type: str
	type_name: str
	name: str
	pos: Vec3
	blockpos: BlockPos
	dim: int
	entity: Entity
	player: Player

	def getNbt(self) -> NBT:
		pass

	def toString(self) -> str:
		pass


class CommandOutput:
	empty: bool
	success_count: int

	def success(self, msg=None) -> bool:
		pass

	def error(self, msg) -> bool:
		pass

	def addMessage(self, msg) -> bool:
		pass

	def toString(self) -> str:
		pass


class CommandResult:

	def __repr__(self):
		pass

	def toObject(self):
		pass


class NBT:
	type: TagType

	@staticmethod
	def newByte(value: int) -> NBT:
		pass

	@staticmethod
	def newShort(value: int) -> NBT:
		pass

	@staticmethod
	def newInt(value: int) -> NBT:
		pass

	@staticmethod
	def newLong(value: int) -> NBT:
		pass

	@staticmethod
	def newFloat(value: float) -> NBT:
		pass

	@staticmethod
	def newDouble(value: float) -> NBT:
		pass

	@staticmethod
	def newByteArray(value: bytearray) -> NBT:
		pass

	@staticmethod
	def newString(value: str) -> NBT:
		pass

	@staticmethod
	def newIntArray() -> NBT:
		pass

	@staticmethod
	def newList() -> NBT:
		pass

	@staticmethod
	def newCompound() -> NBT:
		pass

	@staticmethod
	def fromSNBT(snbt: str) -> NBT:
		pass

	@staticmethod
	def fromBinary(bin: bytes) -> NBT:
		pass

	def __getitem__(self, key) -> NBT:
		pass

	def __setitem__(self, key, value):
		pass

	def asByte(self) -> int:
		pass

	def asShort(self) -> int:
		pass

	def asInt(self) -> int:
		pass

	def asInt64(self) -> int:
		pass

	def asFloat(self) -> float:
		pass

	def asDouble(self) -> float:
		pass

	def toBinary(self) -> bytes:
		pass

	def toJson(self, indent=4) -> str:
		pass

	def toSNBT(self, indent=4, format=SnbtFormat.PartialNewLine) -> str:
		pass

	def append(self, value: NBT):
		pass


class Logger:

	def __init__(self, title: str) -> None:
		pass

	def debug(self, msg: str):
		pass

	def info(self, msg: str):
		pass

	def warn(self, msg: str):
		pass

	def error(self, msg: str):
		pass

	def fatal(self, msg: str):
		pass

	def setTitle(self, title: str):
		pass

	def setFile(self, file: str) -> bool:
		pass

	def setPlayer(self, playe: Player) -> bool:
		pass


class Player:
	name: str
	pos: Vec3
	blockpos: BlockPos
	dim: int
	real_name: str
	xuid: str
	uuid: str
	perm_level: int
	game_mode: GameType
	max_health: int
	health: int
	in_air: bool
	in_water: bool
	sneaking: bool
	speed: float
	direction: Vec2
	unique_id: str
	lang_code: str

	ip: str
	avg_ping: int
	avg_packet_loss: int
	os: str
	clinet_id: str

	def isOP(self) -> bool:
		pass

	def setPermLevel(self, perm: CommandPermissionLevel) -> bool:
		pass

	def setGameMode(self, mode: GameType) -> bool:
		pass

	def runcmd(self, cmd) -> bool:
		pass

	def teleport(self, pos, dim) -> bool:
		pass

	def kill(self) -> bool:
		pass

	def kick(self, msg) -> bool:
		pass

	def sendText(self, msg, type=TextType.RAW) -> bool:
		pass

	def talkAs(self, msg) -> bool:
		pass

	def rename(self, name) -> bool:
		pass

	def setOnFire(self, time: int) -> bool:
		pass

	def transServer(self, ip: str, port=19132) -> bool:
		pass

	def crash(self) -> bool:
		pass

	def hurt(self, damage) -> bool:
		pass

	def refreshChunks(self) -> bool:
		pass

	def giveItem(self, item) -> bool:
		pass

	def clearItem(self, type: str) -> bool:
		pass

	def isSprinting(self) -> bool:
		pass

	def setSprinting(self, value) -> bool:
		pass

	def getBlockStandingOn(self) -> Block:
		pass

	def getHand(self) -> Item:
		pass

	def getOffHand(self) -> Item:
		pass

	def getInventory(self) -> Container:
		pass

	def getArmor(self) -> Container:
		pass

	def getEnderChest(self) -> Container:
		pass

	def getRespawnPosition(self) -> tuple[BlockPos, int]:
		pass

	def setRespawnPosition(self, pos, dim) -> bool:
		pass

	def refreshItems(self) -> bool:
		pass

	def getScore(self, key) -> bool:
		pass

	def setScore(self, key, value) -> bool:
		pass

	def addScore(self, key, value) -> bool:
		pass

	def reduceScore(self, key, value) -> bool:
		pass

	def deleteScore(self, key) -> bool:
		pass

	def setSidebar(
	    self, title, data: list[tuple[str, int]], order: ObjectiveSortOrder
	) -> bool:
		pass

	def removeSidebar(self) -> bool:
		pass

	def setBossBar(self, uid, name, percent, colour: BossEventColour) -> bool:
		pass

	def removeBossBar(self, uid=None) -> bool:
		pass

	def addLevel(self, value) -> bool:
		pass

	def getLevel(self) -> int:
		pass

	def resetLevel(self) -> None:
		pass

	def getXpNeededForNextLevel(self) -> int:
		pass

	def addExperience(self, value) -> bool:
		pass

	def sendSimpleForm(
	    self, title, content, buttons, images, callback
	) -> bool:
		pass

	def sendModalForm(
	    self, title, content, button1, button2, callback
	) -> bool:
		pass

	def sendCustomForm(self, str, callback) -> bool:
		pass

	def setExtraData(self, key, value) -> bool:
		pass

	def getExtraData(self, key) -> bool:
		pass

	def delExtraData(self, key) -> bool:
		pass

	def setNbt(self, nbt) -> bool:
		pass

	def getNbt(self) -> NBT:
		pass

	def addTag(self, tag: str) -> bool:
		pass

	def removeTag(self, tag: str) -> bool:
		pass

	def hasTag(self, tag: str) -> bool:
		pass

	def getAllTags(self) -> list[str]:
		pass

	def getEntityFromViewVector(self, max_distance=5.25) -> Entity:
		pass

	def getBlockFromViewVector(
	    self,
	    includeLiquid=False,
	    solidOnly=False,
	    maxDistance=5.25,
	    ignoreBorderBlocks=True,
	    fullOnly=False,
	) -> Block:
		pass

	# SimulatedPlayer API
	def isSimulatedPlayer(self) -> bool:
		pass

	def simulateSneak(self) -> bool:
		pass

	def simulateAttack(self, target: Entity or None) -> bool:
		pass

	def simulateDestory(self, face: ScriptFacing or None) -> bool:
		pass

	def simulateDisconnect(self) -> bool:
		pass

	def simulateInteract(self) -> bool:
		pass

	def simulateJump(self) -> bool:
		pass

	def simulateLocalMove(self) -> bool:
		pass

	def simulateWorldMove(self) -> bool:
		pass

	def simulateMoveTo(self) -> bool:
		pass

	def simulateLookAt(self) -> bool:
		pass

	def simulateSetBodyRotation(self) -> bool:
		pass

	def simulateNavigateTo(self) -> bool:
		pass

	def simulateUseItem(self) -> bool:
		pass

	def simulateStopDestroyingBlock(self) -> bool:
		pass

	def simulateStopInteracting(self) -> bool:
		pass

	def simulateStopMoving(self) -> bool:
		pass

	def simulateStopUsingItem(self) -> bool:
		pass

	def simulateStopSneaking(self) -> bool:
		pass

	def removeItem(self, slot, num) -> bool:
		pass

	def setHeadShow(self, text) -> bool:
		pass


class Entity:
	name: str
	type: str
	id: str
	pos: Vec3
	blockpos: BlockPos
	dim: int
	max_health: int
	health: int
	in_air: bool
	in_water: bool
	speed: float
	direction: Vec2
	unique_id: str

	def teleport(self) -> bool:
		pass

	def kill(self) -> bool:
		pass

	def hurt(self) -> bool:
		pass

	def setOnFire(self) -> bool:
		pass

	def isPlayer(self) -> bool:
		pass

	def toPlayer(self) -> bool:
		pass

	def isItemEntity(self) -> bool:
		pass

	def toItem(self) -> bool:
		pass

	def getBlockStandingOn(self) -> bool:
		pass

	def getArmor(self) -> bool:
		pass

	def hasContainer(self) -> bool:
		pass

	def getContainer(self) -> bool:
		pass

	def refreshItems(self) -> bool:
		pass

	def setNbt(self) -> bool:
		pass

	def getNbt(self) -> bool:
		pass

	def addTag(self) -> bool:
		pass

	def removeTag(self) -> bool:
		pass

	def hasTag(self) -> bool:
		pass

	def getAllTags(self) -> bool:
		pass

	def getEntityFromViewVector(self) -> bool:
		pass

	def getBlockFromViewVector(self) -> bool:
		pass


class BlockEntity:
	pos: Vec3
	type: str

	def setNbt(self) -> bool:
		pass

	def getNbt(self) -> bool:
		pass

	def getBlock(self) -> bool:
		pass


class Objective:
	name: str
	display_name: str

	def setDisplay(self) -> bool:
		pass

	def setScore(self) -> bool:
		pass

	def addScore(self) -> bool:
		pass

	def reduceScore(self) -> bool:
		pass

	def deleteScore(self) -> bool:
		pass

	def getScore(self) -> bool:
		pass


def getBbsVersion() -> str:
	pass


def getServerProtocolVersion() -> int:
	pass


def runCommand(cmd) -> bool:
	pass


def runCommandEx(cmd) -> tuple[str, bool]:
	pass


def setListener(event, callback) -> bool:
	pass


def registerCommand(
    name, desc, callback, perm=CommandPermissionLevel.GameMasters
) -> bool:
	pass


def getPlayer(name) -> list[Player]:
	pass


def getOnlinePlayers() -> list[Player]:
	pass


def getAllEntities() -> list[Entity]:
	pass


def setServerMotd(motd: str) -> bool:
	pass


def broadcastText(text, type=TextType.RAW) -> bool:
	pass


def broadcastTitle(
    text,
    type=TitleType.SetTitle,
    fade_in_duration=100,
    remain_duration=500,
    fade_out_duration=100
) -> bool:
	pass


def getBlock(pos, dim) -> Block:
	pass


def setBlock(pos, dim, block: Block or str, tile_data=None) -> bool:
	pass


def getStructure(
    pos1, pos2, dim, ignore_entities=True, ignore_blocks=False
) -> NBT:
	pass


def setStructure(
    nbt, pos, dim, mir=Mirror.None_15, rot=Rotation.None_14
) -> None:
	pass


def explode(
    pos, dim, power: float, destroy: bool, range: float, fire: bool
) -> None:
	pass


def spawnItem(item: Item, pos, dim) -> None:
	pass


def setSignBlockMessage(pos, dim, text) -> None:
	pass


def isSlimeChunk(x, z) -> bool:
	pass


def spawnMob(pos, dim, type) -> Entity:
	pass


def cloneMob(pos, dim, entity: Entity) -> Entity:
	pass


def spawnSimulatedPlayer(name, pos, dim) -> Player:
	pass


def spawnParticle(type, pos, dim) -> bool:
	pass


def setMotd(motd) -> bool:
	pass


def sendCmdOutput(output) -> bool:
	pass


def getDisplayObjective(slot) -> Objective:
	pass


def clearDisplayObjective(slot) -> Objective:
	pass


def getScoreObjective(name) -> Objective:
	pass


def newScoreObjective(name, display) -> Objective:
	pass


def removeScoreObjective(name) -> bool:
	pass


def getAllScoreObjectives() -> list[Objective]:
	pass


def setMaxPlayers(num) -> bool:
	pass


def getMoney(xuid) -> int:
	pass


def setMoney(xuid, num) -> bool:
	pass


def addMoney(xuid, num) -> bool:
	pass


def reduceMoney(xuid, num) -> bool:
	pass


def transMoney(xuid1, xuid2, num, notes) -> bool:
	pass


def getMoneyHistt(xuid, time) -> str:
	pass


def clearMoneyHistist(time) -> bool:
	pass
