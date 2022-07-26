from mc import *


def cb(p: CommandOrigin, result):
	p.player.sendText(str(result))


c = Command('test', 'test cmd', CommandPermissionLevel.Any)
c.mandatory('ssss', ParameterType.String)
c.mandatory(
    'aaa', ParameterType.SoftEnum,
    c.setSoftEnum('asdasdasd', ['az,', 'azzzzz'])
)
c.overload(['ssss', 'aaa'])
c.setCallback(cb)
c.setup()