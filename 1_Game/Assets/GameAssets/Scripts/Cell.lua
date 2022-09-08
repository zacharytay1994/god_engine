--[IsComponent]
function C_Cell()
    local var = {
        --[SerializeInt]
        x = 0,
        --[SerializeInt]
        y = 0,
        --[SerializeInt]
        z = 0
    }
    return function()
        return var
    end
end

GridCellUnit = 1

--[IsSystem]
function S_Cell(e)
    local transform = GetTransform(e)
    local cell = GetComponent(e, "C_Cell")
    transform.position.x = cell.x * GridCellUnit
    transform.position.y = cell.y * GridCellUnit
    transform.position.z = cell.z * GridCellUnit
end
