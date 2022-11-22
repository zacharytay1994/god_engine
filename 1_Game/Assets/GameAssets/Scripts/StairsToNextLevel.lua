--[IsComponent]
function C_StairsToNextLevel()
    local var = {
		trigger = false
	}
	
	return function()
		return var
	end
end

--[IsSystem]
function S_StairsToNextLevel(e)

    --local stairs = GetComponent(e, "C_StairsToNextLevel")
    --local player = GetComponent(e, "C_Character")
    --local cell = GetGridCell(e)

    --if(player.characterType == "Player" and player.isDead == false)
    --then
        --print (EntityName(e), "GridCell location:", cell.x, cell.y, cell.z)
    --end
end