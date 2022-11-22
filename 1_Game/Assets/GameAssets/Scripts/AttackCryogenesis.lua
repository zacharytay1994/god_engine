-- This script will allow the player to perform a Cryogenesis attack.
-- This script will be attached to CombatManager.
-- CombatManager will run this script whenever the player uses Cryogenesis.

-- Cryogenesis can create an ice block (Blue). It can also freeze enemies for Pink / Gold.

-- TODO:
-- 1) Currently dice color is not checked, cryo is allowed to freeze everything
-- 2) IceTileScript (for melting the ice and dealing DoT to any character inside)

--[IsComponent]
function C_Cryogenesis()
    local var = {
        --[SerializeString]
        AttackName = "Cryogenesis",

        -- THE VARIABLES IN THIS SECTION IS REQUIRED BY ALL ATTACK SCRIPTS!!! -----------------------------------------
        -- the attacking entity, aka the player
        attacker = -1,
        
        -- can either be a character or a tile, depends on what the player clicked on, so need to check.
        defender = -1,
        
        -- cue to start check
        startCheck = false,
        
        -- marks the end of the check (used by PlayerAttack.lua)
        checkCompleted = false,
        
        -- if canAttack is true at the end of the checking process, then can go ahead with the attack
        canAttack = false,
        
        -- error message (why the attack failed)
        errorMessage = "[AttackCryogenesis.lua] Player not adjacent to enemy.",
        
        -- used to turn player to face the target
        playerRotation = 0,
        --END OF REQUIRED SECTION -------------------------------------------------------------------------------------

        -- x, y, z coordinates of where the ice block should be created
        iceLocation = { }

    }
    return function()
        return var
    end
end

--[IsSystem]
function S_AttackCryogenesis(e)
    
    attackComponent = GetComponent(e, "C_Cryogenesis")
    
    -- checking if there is a character standing on the tile where ice block is to be created ----------------------------
    -- Blue Cryo can only be used on empty block, while Pink and Gold Cryo can be used to freeze enemy.
    if (attackComponent.startCheck == true) then
        
        -- run the check only once per attack
        attackComponent.startCheck = false

        if (attackComponent.attacker == -1 or attackComponent.defender == -1) then 

            print("[AttackCryogenesis.lua] ERROR: Either attacker or defender is -1.")

        else
            
            -- check if cryo can be used in this spot
            if (CheckCryo(attackComponent.defender, e) == true) then
                
                -- passed the check, allow the rest of the script to run
                attackComponent.canAttack = true
                print("[AttackCryogenesis.lua] CheckCryo passed!")
            else
                
                -- failed the check, print fail message
                print("[AttackCryogenesis.lua] CheckCryo failed.")
            end
            
            -- this will allow PlayerAttack.lua to proceed
            attackComponent.checkCompleted = true

        end   
    end
    -- end of checking if player is able to use cryogenesis --------------------------------------------------------------
    

    -- if can use attack, turn the player and activate effects -----------------------------------------------------------
    if (attackComponent.canAttack == true) then 
        
        -- turn player to face enemy 
        transformComponent = GetTransform(attackComponent.attacker)
        transformComponent.rotation.y = attackComponent.playerRotation

        -- create an instance of a blue tile
        local iceTile = InstancePrefabParentedOnGridNow(GetEntity("Floor"), "IceTile", attackComponent.iceLocation[1], attackComponent.iceLocation[2], attackComponent.iceLocation[3])

        -- no screenshake for cryo
        
        -- trigger sound effect (if any)
        
        -- trigger particles (if any)
        
        -- play attack animation (if any)
    end
    -- end of attack effects ---------------------------------------------------------------------------------------------
    
    -- reset variables (do not reset checkCompleted and canAttack here. They will be reset by PlayerAttack.lua)
    attackComponent.startCheck = false
    attackComponent.attacker = -1
    attackComponent.defender = -1
    attackComponent.playerRotation = 0
end

function CheckCryo(clickedEntity, e)

    -- first, check whether the clickedEntity is a character or a tile
    local isCharacter = false
    local characterList = EntitiesWithScriptComponent("C_Character")
    for i = 1, #characterList do
        if (characterList[i] == clickedEntity) then
            isCharacter = true
        end
    end

    local cryoComponent = GetComponent(e, "C_Cryogenesis")

    -- if its a character, then cryo can only work if the strength is pink or gold
    if (isCharacter) then

        -- TODO:
        -- if pink or gold then return true
        -- else return false

        -- FOR NOW: just allow to freeze character even if its blue
        local clickedGrid = GetGridCell(clickedEntity)
        -- cryoComponent.iceLocation = GetTransform(clickedEntity).position

        cryoComponent.iceLocation[1] = clickedGrid.x
        cryoComponent.iceLocation[2] = clickedGrid.y
        cryoComponent.iceLocation[3] = clickedGrid.z

        print("[AttackCryogenesis.lua] Cryo target is a character! Freezing it.")

        return true

    -- else if its a tile, then need to check whether there is a character standing on that tile
    else

        local clickedGrid = GetGridCell(clickedEntity)
        local gridToCheck = { clickedGrid.x, clickedGrid.y + 1, clickedGrid.z }

        for j = 1, #characterList do

            local characterGrid = GetGridCell(characterList[j])

            if (gridToCheck[1] == characterGrid.x and gridToCheck[2] == characterGrid.y and gridToCheck[3] == characterGrid.z) then

                -- there is a character standing on the tile, then if pink or gold, return true

                -- but for now just return true regardless of color
                -- cryoComponent.iceLocation = GetTransform(clickedEntity).position

                cryoComponent.iceLocation = gridToCheck
                
                -- plus 2 to the y-position because we want it to be above the clickedTile
                print("[AttackCryogenesis.lua] Character found standing on selected tile! Freezing it.")
                return true

            end
        end

        -- code reach here means no character is standing on top, can freeze
        print("[AttackCryogenesis.lua] No one standing on selected tile! Freezing it.")
        -- cryoComponent.iceLocation = GetTransform(clickedEntity).position

        cryoComponent.iceLocation = gridToCheck
        -- plus 2 to the y-position because we want it to be above the clickedTile
        -- cryoComponent.iceLocation.y = cryoComponent.iceLocation.y + 2
        return true

    end


end