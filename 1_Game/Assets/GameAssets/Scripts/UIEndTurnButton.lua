-- Click on the button to end player' turn.


--[IsComponent]
function C_EndTurnButton()
    local var = {
        --[SerializeString]
        EndTurnButton = "EndTurnButton",

        pressDelay = 2,

        timer = 0
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_EndTurnButton(e)
    
    local endTurnButton = GetGUIObject(e)
    
    -- checking if player entity exists
    local playerEntity = GetEntity("Player")
    if (playerEntity == -1) then
        print("[EndTurnButton.lua] ERROR: Player entity does not exist! Returning.")
        return
    end

    -- checking if turnordermanager entity exists
    local turnOrderManagerEntity = GetEntity("TurnOrderManager")
    local turnOrderManagerComponent
    if (turnOrderManagerEntity ~= -1) then
        turnOrderManagerComponent = GetComponent(turnOrderManagerEntity, "C_TurnOrderManager")
    else
        print("[EndTurnButton.lua] TurnOrderManager does not exist! Returning.")
        return
    end

    local endTurnButtonComponent = GetComponent(e, "C_EndTurnButton")
    if (endTurnButtonComponent.timer < endTurnButtonComponent.pressDelay) then
        
        endTurnButtonComponent.timer = endTurnButtonComponent.timer + GetDeltaTime()
    
    else

        if endTurnButton.pressed then
        
            -- to make sure this script isn't accidentally added other buttons
            if (EntityName(e) == "EndTurnButton") then
                
                -- only end the turn if it is actually the player's turn
                if (turnOrderManagerComponent.currentTurn == GetEntityData(playerEntity).id) then
				
					-- Reset all the movement highlights for player
                    ResetHighlightTiles()
					
                    -- trigger sound effect
                    InstancePrefab("SFX_EndTurn",0,0,0)
                    
                    -- signal turnOrderManager to move on to the next character's turn
                    turnOrderManagerComponent.nextTurn = true
                    print("[EndTurnButton.lua] EndTurnButton clicked! Ending player turn.")
                end
            end

            endTurnButtonComponent.timer = 0
        end
    end
end
