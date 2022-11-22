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

--[IsSystem]
function S_StairsToNextLevel(e)
    print("Hi")
    local player = GetEntity("Player")
    --local player = GetComponent(e, "C_Character")
    local player_grid_cell = GetGridCell(player)
    local stairs = GetComponent(e, "C_StairsToNextLevel")
    
    local cell = GetGridCell(e)
    print (cell.x , cell.y , cell.z)
    -- if(player.characterType == "Player" and player.isDead == false)
    -- then
    --     print (EntityName(e), "GridCell location:", cell.x, cell.y, cell.z)
    -- end

    if(player.isDead == false and stairs.trigger == false)
    then
        print ("Player is alive")
        if(player_grid_cell.x == cell.x and player_grid_cell.y == cell.y + 1 and player_grid_cell.z == cell.z)
        then
            ChangeScene("Level_2",true)
            stairs.trigger = true
        end
    end

end