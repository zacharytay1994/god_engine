--[IsComponent]
function C_Grid()
    local var = {
        --[SerializeInt]
        cellx = 0,
        --[SerializeInt]
        celly = 0,
        --[SerializeInt]
        cellz = 0
    }
    return function()
        return var
    end
end

GridCellUnit = 1

--[IsSystem]
function S_Grid(e)
    local transform = GetTransform(e)
    local cell
    transform.x = cellx * GridCellUnit
end
