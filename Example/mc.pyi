# pyr的代码补全文件

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

	def getNbt(self) -> "NBT":
		pass

	def hasContainer(self):
		pass

	def getContainer(self):
		pass

	def hasBlockEntity(self):
		pass

	def getBlockEntity(self):
		pass

	def removeBlockEntity(self):
		pass


class Container:
	size: int
	type: str

	def addItem(self):
		pass

	def addItemToFirstEmptySlot(self):
		pass

	def hasRoomFor(self):
		pass

	def removeItem(self):
		pass

	def getItem(self):
		pass

	def setItem(self):
		pass

	def getAllItems(self):
		pass

	def removeAllItems(self):
		pass

	def isEmpty(self):
		pass


class Item:
	name: str
	type: str
	id: int
	count: int
	aux: int

	def __init__(self):
		pass

	def set(self):
		pass

	def clone(self):
		pass

	def isNull(self):
		pass

	def setNull(self):
		pass

	def setAux(self):
		pass

	def setLore(self):
		pass

	def setNbt(self, nbt):
		pass

	def getNbt(self):
		pass


class Command:
	name: str

	def __init__(self, cmd: str, desc: str, level) -> None:
		pass

	def setEnum(self):
		pass

	def setAlias(self):
		pass

	def mandatory2(
	    self,
	    name,
	    type: ParameterType,
	    description: str = "",
	    identifier: str = "",
	    option=CommandParameterOption.None_
	):
		pass

	def mandatory(
	    self,
	    name,
	    type: ParameterType,
	    description: str = "",
	    option=CommandParameterOption.None_
	):
		pass

	def optional2(
	    self,
	    name,
	    type: ParameterType,
	    description: str = "",
	    identifier: str = "",
	    option=CommandParameterOption.None_
	):
		pass

	def optional(
	    self,
	    name,
	    type: ParameterType,
	    description: str = "",
	    option=CommandParameterOption.None_
	):
		pass

	def setSoftEnum(self, name, enums):
		pass

	def addSoftEnumValues(self):
		pass

	def removeSoftEnumValues(self):
		pass

	def getSoftEnumValues(self):
		pass

	def getSoftEnumNames(self):
		pass

	def overload(self, value=None):
		pass

	def setCallback(self, callback):
		pass

	def setup(self):
		pass


class CommandOrigin:
	type: str
	type_name: str
	name: str
	pos: Vec3
	blockpos: BlockPos
	dim: int
	entity: "Entity"
	player: "Player"

	def getNbt(self):
		pass

	def toString(self):
		pass


class CommandOutput:
	empty: bool
	success_count: int

	def success(self, msg=None):
		pass

	def error(self):
		pass

	def addMessage(self):
		pass

	def toString(self):
		pass


class CommandResult:

	def __repr__(self):
		pass

	def toObject(self):
		pass


class NBT:
	type: TagType

	@staticmethod
	def newByte() -> "NBT":
		pass

	@staticmethod
	def newShort() -> "NBT":
		pass

	@staticmethod
	def newInt() -> "NBT":
		pass

	@staticmethod
	def newLong() -> "NBT":
		pass

	@staticmethod
	def newFloat() -> "NBT":
		pass

	@staticmethod
	def newDouble() -> "NBT":
		pass

	@staticmethod
	def newByteArray() -> "NBT":
		pass

	@staticmethod
	def newString() -> "NBT":
		pass

	@staticmethod
	def newIntArray() -> "NBT":
		pass

	@staticmethod
	def newList() -> "NBT":
		pass

	@staticmethod
	def newCompound() -> "NBT":
		pass

	@staticmethod
	def fromSNBT(snbt) -> "NBT":
		pass

	@staticmethod
	def fromBinary() -> "NBT":
		pass

	def __getitem__(self, key):
		pass

	def __setitem__(self, key, value):
		pass

	def asByte(self):
		pass

	def asShort(self):
		pass

	def asInt(self):
		pass

	def asInt64(self):
		pass

	def asFloat(self):
		pass

	def asDouble(self):
		pass

	def toBinary(self):
		pass

	def toJson(self, indent=4):
		pass

	def toSNBT(self, indent=4, format=SnbtFormat.PartialNewLine):
		pass

	def append(self):
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

	def setFile(self, file: str):
		pass

	def setPlayer(self, player):
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

	def isOP(self):
		pass

	def setPermLevel(self):
		pass

	def setGameMode(self):
		pass

	def runcmd(self):
		pass

	def teleport(self):
		pass

	def kill(self):
		pass

	def kick(self):
		pass

	def tell(self):
		pass

	def talkAs(self):
		pass

	def rename(self):
		pass

	def setOnFire(self):
		pass

	def transServer(self):
		pass

	def crash(self):
		pass

	def hurt(self):
		pass

	def refreshChunks(self):
		pass

	def giveItem(self):
		pass

	def clearItem(self):
		pass

	def isSprinting(self):
		pass

	def setSprinting(self):
		pass

	def getBlockStandingOn(self):
		pass

	def getHand(self):
		pass

	def getOffHand(self):
		pass

	def getInventory(self):
		pass

	def getArmor(self):
		pass

	def getEnderChest(self):
		pass

	def getRespawnPosition(self):
		pass

	def setRespawnPosition(self):
		pass

	def refreshItems(self):
		pass

	def getScore(self):
		pass

	def setScore(self):
		pass

	def addScore(self):
		pass

	def reduceScore(self):
		pass

	def deleteScore(self):
		pass

	def setSidebar(self):
		pass

	def removeSidebar(self):
		pass

	def setBossBar(self):
		pass

	def removeBossBar(self):
		pass

	def addLevel(self):
		pass

	def getLevel(self):
		pass

	def resetLevel(self):
		pass

	def getXpNeededForNextLevel(self):
		pass

	def addExperience(self):
		pass

	def sendSimpleForm(self):
		pass

	def sendModalForm(self):
		pass

	def sendCustomForm(self):
		pass

	def setExtraData(self):
		pass

	def getExtraData(self):
		pass

	def delExtraData(self):
		pass

	def setNbt(self):
		pass

	def getNbt(self):
		pass

	def addTag(self):
		pass

	def removeTag(self):
		pass

	def hasTag(self):
		pass

	def getAllTags(self):
		pass

	def getEntityFromViewVector(self):
		pass

	def getBlockFromViewVector(
	    self,
	    includeLiquid=False,
	    solidOnly=False,
	    maxDistance=5.25,
	    ignoreBorderBlocks=True,
	    fullOnly=False,
	):
		pass

	# SimulatedPlayer API
	def isSimulatedPlayer(self):
		pass

	def simulateSneak(self):
		pass

	def simulateAttack(self):
		pass

	def simulateDestory(self):
		pass

	def simulateDisconnect(self):
		pass

	def simulateInteract(self):
		pass

	def simulateJump(self):
		pass

	def simulateLocalMove(self):
		pass

	def simulateWorldMove(self):
		pass

	def simulateMoveTo(self):
		pass

	def simulateLookAt(self):
		pass

	def simulateSetBodyRotation(self):
		pass

	def simulateNavigateTo(self):
		pass

	def simulateUseItem(self):
		pass

	def simulateStopDestroyingBlock(self):
		pass

	def simulateStopInteracting(self):
		pass

	def simulateStopMoving(self):
		pass

	def simulateStopUsingItem(self):
		pass

	def simulateStopSneaking(self):
		pass

	def removeItem(self, slot, num):
		pass

	def setHeadShow(text):
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

	def teleport(self):
		pass

	def kill(self):
		pass

	def hurt(self):
		pass

	def setOnFire(self):
		pass

	def isPlayer(self):
		pass

	def toPlayer(self):
		pass

	def isItemEntity(self):
		pass

	def toItem(self):
		pass

	def getBlockStandingOn(self):
		pass

	def getArmor(self):
		pass

	def hasContainer(self):
		pass

	def getContainer(self):
		pass

	def refreshItems(self):
		pass

	def setNbt(self):
		pass

	def getNbt(self):
		pass

	def addTag(self):
		pass

	def removeTag(self):
		pass

	def hasTag(self):
		pass

	def getAllTags(self):
		pass

	def getEntityFromViewVector(self):
		pass

	def getBlockFromViewVector(self):
		pass


class BlockEntity:
	pos: Vec3
	type: str

	def setNbt(self):
		pass

	def getNbt(self):
		pass

	def getBlock(self):
		pass


class Objective:
	name: str
	display_name: str

	def setDisplay(self):
		pass

	def setScore(self):
		pass

	def addScore(self):
		pass

	def reduceScore(self):
		pass

	def deleteScore(self):
		pass

	def getScore(self):
		pass


def getBbsVersion():
	pass


def getServerProtocolVersion():
	pass


def runCommand():
	pass


def runCommandEx():
	pass


def setListener(event, callback):
	pass


def registerCommand():
	pass


def getPlayer():
	pass


def getOnlinePlayers():
	pass


def getAllEntities():
	pass


def setServerMotd():
	pass


def broadcastText():
	pass


def broadcastTitle():
	pass


def getBlock():
	pass


def setBlock():
	pass


def getStructure():
	pass


def setStructure():
	pass


def explode():
	pass


def spawnItem():
	pass


def setSignBlockMessage():
	pass


def isSlimeChunk():
	pass


def spawnMob():
	pass


def cloneMob():
	pass


def spawnSimulatedPlayer():
	pass


def spawnParticle():
	pass


def setMotd():
	pass


def sendCmdOutput():
	pass


def getDisplayObjective():
	pass


def clearDisplayObjective():
	pass


def getScoreObjective():
	pass


def newScoreObjective(name, display):
	pass


def removeScoreObjective():
	pass


def getAllScoreObjectives():
	pass


def setMaxPlayers():
	pass


def getMoney(xuid):
	pass


def setMoney(xuid, num):
	pass


def addMoney(xuid, num):
	pass


def reduceMoney(xuid, num):
	pass


def transMoney(xuid1, xuid2, num, notes):
	pass


def getMoneyHistt(xuid, time):
	pass


def clearMoneyHistist(time):
	pass
