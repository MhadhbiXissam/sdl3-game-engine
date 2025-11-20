

import json
proc add*(x : cint , y : cint ) : cint {.exportc,dynlib.}  = 
    return x + y 

proc gimme(): cstring {.exportc.} =
  result = "Hey there C code! " & $rand(100)

# proc printNameFromJson*(s: cstring) {.exportc,dynlib.} =
#   try:
#     let obj = parseJson($(s))
#     if obj.hasKey("name"):
#       echo obj["name"].getStr
#   except JsonParsingError:
#     echo "Invalid JSON"

# # # Example usage


# when isMainModule:
#     var jsonStr = """{"name":"Issaù","age":30,"languages":["Nim","Python","C"]}"""
#     printNameFromJson(jsonStr)