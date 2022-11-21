--[IsComponent]
function C_GridHighlightScript()
	local var = {
		tiles_array = {},
		is_init = false
	}
	
	return function()
		return var
	end
end

--[IsSystem]
function S_GridHighlightScript(e)
	-- min x = -3
	-- max x =  4
	-- min z = -6
	-- max z =  1
	local c_gridmanager = GetComponent(e, "C_GridHighlightScript")
	if (c_gridmanager.is_init == false) then
		local all_tiles = EntitiesWithScriptComponent("C_FloorTile")
		for i=1,8 do
			c_gridmanager.tiles_array[i] = {}
			for j=1,8 do
				for k=1,#all_tiles do
					local tile_cell = GetGridCell(all_tiles[k])
					if(tile_cell.x == (i - 4) and tile_cell.z == (j - 7)) then
						c_gridmanager.tiles_array[i][j] = all_tiles[k]
						break
					end
				end
			end
		end
		
		for i=1,8 do
			for j=1,8 do
				ChangeTexture(c_gridmanager.tiles_array[i][j], "DarkGreen")
			end
		end
		
		c_gridmanager.is_init = true
	end
	
end