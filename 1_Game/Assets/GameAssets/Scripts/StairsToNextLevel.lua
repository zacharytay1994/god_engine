--[IsComponent]
function C_StairsToNextLevel()
    local var = {
        --[SerializeBool]
		trigger = false
	}
	
	return function()
		return var
	end
end

SceneName = "Empty"

--[IsSystem]
function S_StairsToNextLevel(e)
    local player = GetEntity("Player")
    local player_grid_cell = GetGridCell(player)
    local stairs = GetComponent(e, "C_StairsToNextLevel")
    local cell = GetGridCell(e)

    if(stairs.trigger == false)
    then
        if(player_grid_cell.x == cell.x and player_grid_cell.y == cell.y + 1 and player_grid_cell.z == cell.z)
        then
            ChangeScene("Level_2",true)
            SceneName = "Level_2"
            stairs.trigger = true
        end
    end

end