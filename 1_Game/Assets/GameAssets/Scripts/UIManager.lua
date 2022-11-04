-- This script manages Action Buttons textures and the Turn Order UI.
-- Action Button functionality is defined in ActionButton.lua.
-- May separate this script into smaller scripts during M3.

--[IsComponent]
function C_UIManager()
    local var = {
        --[SerializeString]
        UIManager = "UIManager",

        -- only allow one dice roll per turn
        diceRolled = false,
        
        -- list of all action buttons
        actionButtonList = {},

        -- add the character icon UI entities into iconList (only once)
        characterIconsInit = false,

        -- list of all character icons
        iconList = {} 
    };
    return function()
        return var
    end
end

--[IsSystem]
function S_UIManager(e)
    
    local UIManagerComponent = GetComponent(e, "C_UIManager")

    -- checking if player entity exists
    local playerEntity = GetEntity("Player")
    if (playerEntity == -1) then
        print("[UIManager.lua] Player entity is missing! Returning.")
        return
    end

    -- getting TurnOrderManager entity and component
    local turnOrderManagerEntity = GetEntity("TurnOrderManager")
    local turnOrderManagerComponent
    if (turnOrderManagerEntity ~= -1) then
        turnOrderManagerComponent = GetComponent(turnOrderManagerEntity, "C_TurnOrderManager")
    end
    
    -- updating action buttons -----------------------------------------------------------------------------------------------------------
    -- only update action buttons on the player's turn
    -- note that only the action buttons' textures are defined here. functionality is defined in ActionButton.lua
    if (GetEntityData(playerEntity).id == turnOrderManagerComponent.currentTurn) then

        -- wait until dice have finished rolling
        if (UIManagerComponent.diceRolled == false) then
                  
            local diceList = EntitiesWithScriptComponent("C_DiceScript")
            local diceSettled = true
            
            -- if any die has a value of 0 then they have not properly settled down
            for i = 1, #diceList do
                if (GetComponent(diceList[i], "C_DiceScript").value == 0) then
                    diceSettled = false
                end
            end
            
            if (diceSettled) then
                
                -- don't allow player to roll anymore for this turn
                UIManagerComponent.diceRolled = true
                            
                -- change the button textures
                for j = 1, #diceList do
                    
                    currentDiceComponent = GetComponent(diceList[j], "C_DiceScript")
                    
                    -- currently only 2 attack types, will modify this part when more attack types are implemented
                    if (currentDiceComponent.value == 1 or currentDiceComponent.value == 2 or currentDiceComponent.value == 3) then                       
                        UIManagerComponent.actionButtonList[#UIManagerComponent.actionButtonList + 1] = "FrontJab"                       
                    else              
                        UIManagerComponent.actionButtonList[#UIManagerComponent.actionButtonList + 1] = "EnergyBolt"                      
                    end
                end
                
                -- change button textures to show the available actions
                ChangeTexture(GetEntity("Button1"), UIManagerComponent.actionButtonList[1])
                ChangeTexture(GetEntity("Button2"), UIManagerComponent.actionButtonList[2])
                ChangeTexture(GetEntity("Button3"), UIManagerComponent.actionButtonList[3])
                                
                -- reset dice value to zero
                for k = 1, #diceList do
                    GetComponent(diceList[k], "C_DiceScript").value = 0
                end            
            end
        end
    else

        -- if not the player's turn, then reset diceRolled
        UIManagerComponent.diceRolled = false

        -- reset the buttons
        ChangeTexture(GetEntity("Button1"), "empty_btn")
        ChangeTexture(GetEntity("Button2"), "empty_btn")
        ChangeTexture(GetEntity("Button3"), "empty_btn")

        -- clear actionButtonList
        UIManagerComponent.actionButtonList = {}
    end
    -- end of updating buttons -----------------------------------------------------------------------------------------------------------

    -- updating turn order icons ---------------------------------------------------------------------------------------------------------
    if (UIManagerComponent.characterIconsInit == false) then  
        print("[UIManager.lua] Initializing character icons")
        UIManagerComponent.iconList[#UIManagerComponent.iconList + 1] = GetEntity("TurnOne")
        UIManagerComponent.iconList[#UIManagerComponent.iconList + 1] = GetEntity("TurnTwo")
        UIManagerComponent.iconList[#UIManagerComponent.iconList + 1] = GetEntity("TurnThree")
        UIManagerComponent.characterIconsInit = true
    end

    if (turnOrderManagerComponent.refreshTurnOrderUI == true) then
    
        print("[UIManager.lua] Refreshing turn order UI.")
        
        for i = 1, #turnOrderManagerComponent.turnQueue do 
        
            if (GetComponent(turnOrderManagerComponent.turnQueue[i], "C_Character").characterType == "Player") then
                ChangeTexture(UIManagerComponent.iconList[i], "TritonTurn")

            elseif (GetComponent(turnOrderManagerComponent.turnQueue[i], "C_Character").characterType == "Squinky") then
                ChangeTexture(UIManagerComponent.iconList[i], "SquinkyTurn")
            end
        end

        turnOrderManagerComponent.refreshTurnOrderUI = false
    end

    -- darken icon for idle characters
    for k = 1, #UIManagerComponent.iconList do
                   
        -- if current character is active, switch to lighter icon
        if (turnOrderManagerComponent.queueIndex == k) then    
            if (TextureName(UIManagerComponent.iconList[k]) == "TritonDark") then
                ChangeTexture(UIManagerComponent.iconList[k], "TritonTurn")
            elseif (TextureName(UIManagerComponent.iconList[k]) == "SquinkyDark") then
                ChangeTexture(UIManagerComponent.iconList[k], "SquinkyTurn")
            end

        -- if current character is not active, switch to darker icon
        else 
            if (TextureName(UIManagerComponent.iconList[k]) == "TritonTurn") then
                ChangeTexture(UIManagerComponent.iconList[k], "TritonDark")
            elseif (TextureName(UIManagerComponent.iconList[k]) == "SquinkyTurn") then
                ChangeTexture(UIManagerComponent.iconList[k], "SquinkyDark")
            end
        end
    end
    -- End of updating turn order icons --------------------------------------------------------------------------------------------------
end