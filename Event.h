#pragma once
#include <string>
using std::string;
enum class Event{
	onConsoleInput,
	onConsoleOutput,
	onSelectForm,
	onUseItem,
	onPlaceBlock,
	onDestroyBlock,
	onOpenChest,
	onOpenBarrel,
	onCloseChest,
	onCloseBarrel,
	onContainerChange,
	onChangeDimension,
	onMobDie,
	onMobHurt,
	onRespawn,
	onChat,
	onInputText,
	onCommandBlockUpdate,
	onInputCommand,
	onCommandBlockPerform,
	onPlyaerJoin,
	onPlyaerLeft,
	onPlayerAttack,
	onLevelExplode,
	onSetArmor,
	onFallBlockTransform,
	onUseRespawnAnchorBlock,
	onScoreChanged,
	onMove
};
static char toEvent(const string& s) {
	if (s == u8"后台输入")return 0;
	else if (s == u8"后台输出")return 1;
	else if (s == u8"选择表单")return 2;
	else if (s == u8"使用物品")return 3;
	else if (s == u8"放置方块")return 4;
	else if (s == u8"破坏方块")return 5;
	else if (s == u8"打开箱子")return 6;
	else if (s == u8"打开木桶")return 7;
	else if (s == u8"关闭箱子")return 8;
	else if (s == u8"关闭木桶")return 9;
	else if (s == u8"放入取出")return 10;
	else if (s == u8"切换维度")return 11;
	else if (s == u8"生物死亡")return 12;
	else if (s == u8"生物受伤")return 13;
	else if (s == u8"玩家重生")return 14;
	else if (s == u8"聊天消息")return 15;
	else if (s == u8"输入文本")return 16;
	else if (s == u8"更新命令方块")return 17;
	else if (s == u8"输入指令")return 18;
	else if (s == u8"命令方块执行")return 19;
	else if (s == u8"加入游戏")return 20;
	else if (s == u8"离开游戏")return 21;
	else if (s == u8"玩家攻击")return 22;
	else if (s == u8"世界爆炸")return 23;
	else if (s == u8"玩家穿戴")return 24;
	else if (s == u8"耕地破坏")return 25;
	else if (s == u8"使用重生锚")return 26;
	else if (s == u8"计分板改变")return 27;
	else if (s == u8"玩家移动")return 28;
	else return -1;
}