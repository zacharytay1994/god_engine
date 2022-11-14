-- This script stores data such as currently selected action.
-- Probably will not have a system.

-- This component is used by:
-- 1) ActionButton.lua

-- TODO:
-- 1) 

--[IsComponent]
function C_Player()
    local var = {
        --[SerializeString]
        Player = "Player",

        -- certain systems will run / not run based on currently selected action
        selectedAction = nil,

        -- used by ActionButton.lua to set button texture to empty after player uses the attack
        playerAttacked = false
    };
    return function()
        return var
    end
end

--[IsSystem]
function S_Player(e)  

    -- Pressing 1, 2, 3, 4, etc does the same thing as clicking the Action Buttons.
    -- Leaving these here for quick access.

    -- Currently some attacks are hardcoded (e.g. BigSwing). Before un-hardcoding them:
    -- 1) Need Action Button Texture
    -- 2) Need Dice to return a number between 0 - 8
    -- More detail in ActionButton.lua
    
    playerComponent = GetComponent(e, "C_Player")

    -- this part is for hotkey cheats, can remove this part if hotkeys are no longer needed ------------------------------
    local playerAttackComponent
    if (playerEntity ~= -1) then
        playerAttackComponent = GetComponent(e, "C_PlayerAttack")
    else
        print("[Player.lua] Player Entity not found! Returning.")
        return
    end
    ----------------------------------------------------------------------------------------------------------------------

    -- press 1
    if (CheckKeyPress(49)) then
        print("[Player.lua] FrontJab selected through hotkey!")
        playerComponent.selectedAction = "FrontJab"
        playerAttackComponent.selectedAttack = attackList[1]
        print("[ActionButton.lua] Selected Player attack:", playerAttackComponent.selectedAttack[1], 
                    "Base damage:", playerAttackComponent.selectedAttack[2], 
                    "Special property:", playerAttackComponent.selectedAttack[3], "\n")
        UnselectAllButtons()
    end

    -- press 2
    if (CheckKeyPress(50)) then
        print("[Player.lua] BigSwing selected through hotkey!")
        playerComponent.selectedAction = "BigSwing"
        playerAttackComponent.selectedAttack = attackList[4]
        print("[PlayerAttack.lua] Selected Player attack:", playerAttackComponent.selectedAttack[1], 
                    "Base damage:", playerAttackComponent.selectedAttack[2], 
                    "Special property:", playerAttackComponent.selectedAttack[3], "\n")

        UnselectAllButtons()
    end

    -- press 3
    if (CheckKeyPress(51)) then
        print("[Player.lua] EnergyBolt selected through hotkey!")
        playerComponent.selectedAction = "EnergyBolt"
        playerAttackComponent.selectedAttack = attackList[10]
        print("[ActionButton.lua] Selected Player attack:", playerAttackComponent.selectedAttack[1], 
                    "Base damage:", playerAttackComponent.selectedAttack[2], 
                    "Special property:", playerAttackComponent.selectedAttack[3], "\n")
        UnselectAllButtons()
    end

    -- press 4
    if (CheckKeyPress(52)) then 
        print("[Player.lua] Move selected through hotkey!")
        playerComponent.selectedAction = "Move"
        UnselectAllButtons()
    end

    -- press 5
    if (CheckKeyPress(53)) then 
        print("[Player.lua] Projectile selected through hotkey!")
        playerComponent.selectedAction = "Projectile"
        playerAttackComponent.selectedAttack = attackList[13]
            print("[ActionButton.lua] Selected Player attack:", playerAttackComponent.selectedAttack[1], 
                        "Base damage:", playerAttackComponent.selectedAttack[2], 
                        "Special property:", playerAttackComponent.selectedAttack[3], "\n")
        UnselectAllButtons()
    end
end

-- this is to prevent Action Buttons from resetting due to using attacks through hotkeys
function UnselectAllButtons()
    
    print("[Player.lua] Un-selecting all other Action Buttons!")
        local actionButtonList = EntitiesWithScriptComponent("C_ActionButton")
        for i = 1, #actionButtonList do
            GetComponent(actionButtonList[i], "C_ActionButton").buttonSelected = false
        end
end