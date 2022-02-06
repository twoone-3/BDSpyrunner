!> 此页待完善

# NBT
Named Binary Tag
## init(type, value)
  1. type: 数据类型  
     value: 数据  
     例: NBT('Int',3) NBT('Compound')
  2. type: SNBT  
     value: SNBT字符串  
     例: NBT('SNBT', snbt)
  3. type: Binary  
     value: bytes  
     例: NBT('Binary', bytes)

## getType() -> str
## toBinary() -> bytes
## toJson(indentation:int = 4) -> bytes
## toSNBT() -> str
## append(value:mc.NBT) -> None
