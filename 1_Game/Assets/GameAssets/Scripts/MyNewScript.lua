--[IsComponent]
function C_MyNewScript()
	local var = {
		--[SerializeString]
		mynewcomponent = "hello"
	}
	return function()
		return var
	end
end

--[IsSystem]
function S_MyNewScript(e)
end
