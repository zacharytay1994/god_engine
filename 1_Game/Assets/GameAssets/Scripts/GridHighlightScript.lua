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
	-- min x = -3, max x =  4
	-- min z = -6, max z =  1
	local c_gridmanager = GetComponent(e, "C_GridHighlightScript")
	if (c_gridmanager.is_init == false) then
		RefreshTileArray()
		
		HighlightTiles(GetEntity("Player"), 4)
		--ResetHighlightTiles()
	end
end

function ResetHighlightTiles()
	print("[GridHighlightScript] Resetting tile highlights...")
	local c_gridmanager = GetComponent(GetEntity("TileHighlighter"), "C_GridHighlightScript")
	for i=1,8 do
		for j=1,8 do
			ChangeTexture(c_gridmanager.tiles_array[i][j], "TileTexture")
		end
	end
end

function HighlightTiles(e, range)
	-- Not using pathfinding yet
	print("[GridHighlightScript] Highlighting tiles...")
	local c_gridmanager = GetComponent(GetEntity("TileHighlighter"), "C_GridHighlightScript")
	local origin_cell = GetGridCell(e)
	if(EntityName(e) == "Player") then
		origin_cell.y = origin_cell.y - 1
	end
	for i=-range,range do
		for j=-range,range do
			local cell_x = origin_cell.x + i
			local cell_z = origin_cell.z + j
			if (cell_x >= -3 and cell_x <= 4 and cell_z >= -6 and cell_z <= 1) then
				local height_diff = GetGridCell(c_gridmanager.tiles_array[cell_x + 4][cell_z + 7]).y - origin_cell.y
				if (height_diff < 0) then
					height_diff = 0
				end
				if (math.abs(i) + math.abs(j) + height_diff <= range) then
					if(height_diff ~= 0) then
						print(math.abs(i) + " " + math.abs(j) + " " + height_diff)
					end
					ChangeTexture(c_gridmanager.tiles_array[cell_x + 4][cell_z + 7], "DarkGreen")
				end
			end
		end
	end
	if(EntityName(e) == "Player") then
		origin_cell.y = origin_cell.y + 1
	end
end

function RefreshTileArray()
	print("[GridHighlightScript] Generating tile array...")
	local c_gridmanager = GetComponent(GetEntity("TileHighlighter"), "C_GridHighlightScript")
	local all_tiles = EntitiesWithScriptComponent("C_FloorTile")

	for i=1,8 do
		if(c_gridmanager.is_init == false) then
			c_gridmanager.tiles_array[i] = {}
		end
		for j=1,8 do
			for k=1,#all_tiles do
				local tile_cell = GetGridCell(all_tiles[k])
				if(tile_cell.x == (i - 4) and tile_cell.z == (j - 7)) then
					if(c_gridmanager.tiles_array[i][j] == nil or tile_cell.y > GetGridCell(c_gridmanager.tiles_array[i][j]).y) then
						c_gridmanager.tiles_array[i][j] = all_tiles[k]
					end
				end
			end
		end
	end
	
	c_gridmanager.is_init = true
end