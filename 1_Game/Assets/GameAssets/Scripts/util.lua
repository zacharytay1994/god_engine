
local function doSomething()
    print("[util.lua] doSomething()\n")
end

local function doMore()
    print("[util.lua] doMore()\n")
end

return { doSomething = doSomething, doMore = doMore }