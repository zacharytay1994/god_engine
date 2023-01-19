-- This script makes ice tiles melt when it's time is up.
-- Duration depends on the blue, pink or gold.

-- TODO:
-- 1) Apply fall damage when a character falls due to ice tile melting

--[IsComponent]
function C_IceTile()
    local var = {

        -- number of turns before ice tile melts
        duration = -1,

        -- the turn on which the ice tile was created
        turnCreated = -1

    }
    return function()
        return var
    end
end

--[IsSystem]
function S_IceTile(e)

    local iceTileComponent = GetComponent(e, "C_IceTile")

    if (iceTileComponent.duration ~= -1) then
    
        local turnOrderManagerEntity = GetEntity("TurnOrderManager")
        local turnOrderManagerComponent = nil
        if (turnOrderManagerEntity ~= -1) then
            turnOrderManagerComponent = GetComponent(turnOrderManagerEntity, "C_TurnOrderManager")
        end

        local currentTurn = turnOrderManagerComponent.turnCycleCounter

        if (iceTileComponent.turnCreated + iceTileComponent.duration == currentTurn) then
    
            -- drop any characters standing on it, and make sure they land safely
            IceTileDropCharacter(e)

            -- melt the ice tile
            RemoveInstance(e)
        
        end
    end
end

function IceTileDropCharacter(e)

    local iceTileGrid = GetGridCell(e)
    local characterList = EntitiesWithScriptComponent("C_Character")
    local characterToDrop = nil

    for i = 1, #characterList do
    
        local currentCharacterGrid = GetGridCell(characterList[i])

        if (currentCharacterGrid.x == iceTileGrid.x and currentCharacterGrid.y == iceTileGrid.y + 1 and currentCharacterGrid.z == iceTileGrid.z) then
        
            characterToDrop = characterList[i]
            
            -- TODO: APPLY FALL DAMAGE HERE
            -- local characterComponent = GetComponent(characterToDrop, "C_Character")

            -- MOVE CHARACTER DOWN HERE
            currentCharacterGrid.y = currentCharacterGrid.y - 1
            break
        end
    end
end