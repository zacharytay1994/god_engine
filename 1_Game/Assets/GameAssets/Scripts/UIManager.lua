

--[IsComponent]
function C_UIManager()
    local var = {
        --[SerializeString]
        UIManager = "UIManager",

        diceRolled = false,
        
        diceHasZero = false,

        buttonActionsList = {}
    };
    return function()
        return var
    end
end

--[IsSystem]
function S_UIManager(e)
    
    -- get list of all dice 
    -- if all dice components value ~= 0 then change the buttons UI to the correct icons

    local UIManagerComponent = GetComponent(e, "C_UIManager")
    
    -- Updating action buttons -----------------------------------------------------------------------------------------------------------
    -- only update action buttons on the player's turn

    local turnOrderManagerEntity = GetEntity("TurnOrderManager")
    local turnOrderManagerComponent
    if (turnOrderManagerEntity ~= -1) then
        turnOrderManagerComponent = GetComponent(turnOrderManagerEntity, "C_TurnOrderManager")
    end

    if (GetEntityData(GetEntity("Player")).id == turnOrderManagerComponent.currentTurn) then

        -- if have not gotten proper values from dice
        if (UIManagerComponent.diceRolled == false) then
            
            
            local diceList = EntitiesWithScriptComponent("C_DiceScript")
            local diceSettled = true
            
            -- print("UIManagerComponent.diceRolled == false")

            -- if any die has a value of 0 then they have not properly settled down
            for i = 1, #diceList do
                if (GetComponent(diceList[i], "C_DiceScript").value == 0) then
                    diceSettled = false
                end
            end
            
            -- press T
            if (diceSettled) then
                
                UIManagerComponent.diceRolled = true
                            
                -- change the button textures
                for j = 1, #diceList do
                    
                    currentDiceComponent = GetComponent(diceList[j], "C_DiceScript")

                    -- print("[UIManager.lua] Dice value:", currentDiceComponent.value)
                    
                    if (currentDiceComponent.value == 1 or currentDiceComponent.value == 2 or currentDiceComponent.value == 3) then
                        
                        UIManagerComponent.buttonActionsList[#UIManagerComponent.buttonActionsList + 1] = "FrontJab"
                        
                    else
                        
                        UIManagerComponent.buttonActionsList[#UIManagerComponent.buttonActionsList + 1] = "EnergyBolt"
                        
                    end
                end
                
                ChangeTexture(GetEntity("Button1"), UIManagerComponent.buttonActionsList[1])
                ChangeTexture(GetEntity("Button2"), UIManagerComponent.buttonActionsList[2])
                ChangeTexture(GetEntity("Button3"), UIManagerComponent.buttonActionsList[3])
                --ChangeTexture(GetEntity("Button3"), UIManagerComponent.buttonActionsList[3])
                
                -- print(TextureName(GetEntity("Button1")))
                
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

    end
    -- End of updating buttons -----------------------------------------------------------------------------------------------------------

    -- Updating turn order icons ---------------------------------------------------------------------------------------------------------




    -- End of updating turn order icons --------------------------------------------------------------------------------------------------













end




--[[ PSEUDOCODE

if starting a new turn cycle
    init the turnQueue array
    buildTurnQueue = false

if (GlobalStatemachine.CurrentState = CharacterTurn)
    while not every character has performed their turn
        perform turn
        if (nextTurn)
            nextTurn = false
            go to next character

buildTurnQueue = true
GlobalStatemachine.CurrentState = RandomEventState

--]]