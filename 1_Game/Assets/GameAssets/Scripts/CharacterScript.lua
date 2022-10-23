-- C_Character contains basic data that all characters (players and enemies) will need, such as maxHP,
-- currentHP, maxStamina, currentStamina, strength, defence. 


--[IsComponent]
function C_Character()
    local var = {
        --[SerializeInt]
        maxHP = 10,
        --[SerializeInt]
        currentHP = 10,
        --[SerializeInt]
        maxStamina = 10,
        --[SerializeInt]
        currentStamina = 10,
        --[SerializeInt]
        strength = 10 ,
        --[SerializeInt]
        defence = 10
    };
    return function()
        return var
    end
end

--[IsSystem]
function S_Character(e)
    
    -- press L to check character's coordinates on the grid
    if (CheckKeyDown(76) == true) then
        local cell = GetGridCell(e)
        print(cell.x, cell.y, cell.z)
    end

end