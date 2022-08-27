--[IsComponent]
function C_new_script()
	local var = {
	}
	return function()
		return var
	end
end

--[IsSystem]
function S_new_script(e)
end