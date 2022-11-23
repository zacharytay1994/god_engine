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
        iconList = {},
		
		-- Size of healthbar
		healthbar_size = 0.7,
		
		-- Health record, used to optimize the healthbar UI
		health_record = 0.0
    };
    return function()
        return var
    end
end

--[IsSystem]
function S_UIManager(e)
    

    local UIManagerComponent = GetComponent(e, "C_UIManager")
	
	if (UIManagerComponent.healthbar_size == 0.0) then
		UIManagerComponent.healthbar_size = GetGUIObject(GetEntity("HealthbarRed")).size.x
	end

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
                if (GetComponent(diceList[i], "C_DiceScript").value == -1) then
                    diceSettled = false
                end
            end
            
            if (diceSettled) then

                -- don't allow player to roll anymore for this turn
                UIManagerComponent.diceRolled = true

                local attackList = GetComponent(GetEntity("CombatManager"), "C_AttackList").attackList
                            
                -- change the button textures
                for j = 1, #diceList do
                    
                    currentDiceComponent = GetComponent(diceList[j], "C_DiceScript")

                    UIManagerComponent.actionButtonList[#UIManagerComponent.actionButtonList + 1] = attackList[(currentDiceComponent.value * 3) + currentDiceComponent.color][1]   
                    
                    -- -- old code, leave here for reference 
                    -- if (currentDiceComponent.value == 0) then                       
                    --     UIManagerComponent.actionButtonList[#UIManagerComponent.actionButtonList + 1] = "FrontJab"                       
                    -- elseif (currentDiceComponent.value == 1) then                       
                    --     -- UIManagerComponent.actionButtonList[#UIManagerComponent.actionButtonList + 1] = "BigSwing"                       
                    -- elseif (currentDiceComponent.value == 2) then                       
                    --     -- UIManagerComponent.actionButtonList[#UIManagerComponent.actionButtonList + 1] = "GroundSmash"                       
                    -- elseif (currentDiceComponent.value == 3) then
                    --     UIManagerComponent.actionButtonList[#UIManagerComponent.actionButtonList + 1] = "EnergyBolt"
					-- elseif (currentDiceComponent.value == 4) then
                    --     -- UIManagerComponent.actionButtonList[#UIManagerComponent.actionButtonList + 1] = "Projectile"             
					-- elseif (currentDiceComponent.value == 5) then
                    --     -- UIManagerComponent.actionButtonList[#UIManagerComponent.actionButtonList + 1] = "Corporikinesis"             
                    -- end

                    -- -- un-hardcoded solution 
                    -- -- (diceValue will be a value from 0 to 8, colorModifier will be a value from 1 to 3.
                    -- -- On the dice side, need to map 0 to FrontJab, 1 to BigSwing, 2 to GroundSmash, 3 to EnergyBolt, etc.)
                    -- UIManagerComponent.actionButtonList[#UIManagerComponent.actionButtonList + 1] = attackList[(diceValue * 3) + colorModifier][1]     
                    -- where attackList[(diceValue * 3) + colorModifier][1] is "frontJabBlue"
                    -- and there will be a texture called with the exact same name "frontJabBlue"
                    -- end of setting selected action --------------------------------------------------
                end
                
               print("before changing button textures")
               print("UIManagerComponent.actionButtonList[1]", UIManagerComponent.actionButtonList[1])
               print("UIManagerComponent.actionButtonList[2]", UIManagerComponent.actionButtonList[2])
               print("UIManagerComponent.actionButtonList[3]", UIManagerComponent.actionButtonList[3])

                -- change button textures to show the available actions
                ChangeTexture(GetEntity("Button1"), UIManagerComponent.actionButtonList[1])
                ChangeTexture(GetEntity("Button2"), UIManagerComponent.actionButtonList[2])
                ChangeTexture(GetEntity("Button3"), UIManagerComponent.actionButtonList[3])
                rint("after changing button textures")
                                
                -- reset dice value to zero
                -- for k = 1, #diceList do
                --     GetComponent(diceList[k], "C_DiceScript").value = 0
                -- end            
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
	
    local playerComponent = GetComponent(playerEntity, "C_Character")

    GetGUIText(GetEntity("StaminaIcon")).text = tostring(playerComponent.currentStamina)
    if(UIManagerComponent.health_record ~= playerComponent.currentHP) then
        UIManagerComponent.health_record = playerComponent.currentHP
        GetGUIText(GetEntity("HealthHeart")).text = tostring(playerComponent.currentHP)
        GetGUIObject(GetEntity("HealthbarRed")).size.x = (playerComponent.currentHP / playerComponent.maxHP) * UIManagerComponent.healthbar_size
    end
	
	-- enable roll dice button
	local rollDiceBtn = GetGUIObject(GetEntity("RollDiceButton"))
	if (GetEntityData(playerEntity).id == turnOrderManagerComponent.currentTurn and UIManagerComponent.diceRolled == false) then
		rollDiceBtn.active = true
		
		local diceList = EntitiesWithScriptComponent("C_DiceScript")
		for i = 1, #diceList do
			if (GetComponent(diceList[i], "C_DiceScript").is_rolling == true) then
				rollDiceBtn.active = false
				break
			end
		end
		
		if (rollDiceBtn.active == true) then
			if (rollDiceBtn.pressed) then
				for i = 1, #diceList do
					DiceScript_RollDice(diceList[i], GetComponent(diceList[i], "C_DiceScript"))
				end
				rollDiceBtn.active = false
			end
		end
	else
		rollDiceBtn.active = false
	end
	
	-- print(UIManagerComponent.healthbar_size)
	
    -- end of updating buttons -----------------------------------------------------------------------------------------------------------

    -- updating turn order icons ---------------------------------------------------------------------------------------------------------
    -- if (UIManagerComponent.characterIconsInit == false) then  
    --     print("[UIManager.lua] Initializing character icons")
    --     UIManagerComponent.iconList[#UIManagerComponent.iconList + 1] = GetEntity("TurnOne")
    --     UIManagerComponent.iconList[#UIManagerComponent.iconList + 1] = GetEntity("TurnTwo")
    --     UIManagerComponent.iconList[#UIManagerComponent.iconList + 1] = GetEntity("TurnThree")
    --     UIManagerComponent.characterIconsInit = true
    -- end

    -- if (turnOrderManagerComponent.refreshTurnOrderUI == true) then
    
    --     print("[UIManager.lua] Refreshing turn order UI.")
        
    --     for i = 1, #turnOrderManagerComponent.turnQueue do 
        
    --         if (GetComponent(turnOrderManagerComponent.turnQueue[i], "C_Character").characterType == "Player") then
    --             ChangeTexture(UIManagerComponent.iconList[i], "TritonTurn")

    --         elseif (GetComponent(turnOrderManagerComponent.turnQueue[i], "C_Character").characterType == "Squinky") then
    --             ChangeTexture(UIManagerComponent.iconList[i], "SquinkyTurn")
    --         end

    --         -- temp
    --         if (i == 3) then break end
    --     end

    --     turnOrderManagerComponent.refreshTurnOrderUI = false
    -- end

    -- -- darken icon for idle characters
    -- for k = 1, #UIManagerComponent.iconList do
                   
    --     -- if current character is active, switch to lighter icon
    --     if (turnOrderManagerComponent.queueIndex == k) then    
    --         if (TextureName(UIManagerComponent.iconList[k]) == "TritonDark") then
    --             ChangeTexture(UIManagerComponent.iconList[k], "TritonTurn")
    --         elseif (TextureName(UIManagerComponent.iconList[k]) == "SquinkyDark") then
    --             ChangeTexture(UIManagerComponent.iconList[k], "SquinkyTurn")
    --         end

    --     -- if current character is not active, switch to darker icon
    --     else 
    --         if (TextureName(UIManagerComponent.iconList[k]) == "TritonTurn") then
    --             ChangeTexture(UIManagerComponent.iconList[k], "TritonDark")
    --         elseif (TextureName(UIManagerComponent.iconList[k]) == "SquinkyTurn") then
    --             ChangeTexture(UIManagerComponent.iconList[k], "SquinkyDark")
    --         end
    --     end
    -- end
    -- End of updating turn order icons --------------------------------------------------------------------------------------------------
end