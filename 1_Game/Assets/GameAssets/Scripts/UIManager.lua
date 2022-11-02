

--[IsComponent]
function C_UIManager()
    local var = {
        --[SerializeString]
        UIManager = "UIManager",

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
    
    local diceList = EntitiesWithScriptComponent("C_DiceScript")
    local bool = true
    for i = 1, #diceList do
        if (GetComponent(diceList[i], "C_DiceScript").is_rolling == true and GetComponent(diceList[i], "C_DiceScript").is_init) then
            bool = false
        end
    end

    if (bool) then
        
        print("UIManagerComponent.diceHasZero == false")
        
        -- change the button textures
        for j = 1, #diceList do
            
            currentDiceComponent = GetComponent(diceList[j], "C_DiceScript")

            if (currentDiceComponent.value == 1 or currentDiceComponent.value == 2) then

                UIManagerComponent.buttonActionsList[#UIManagerComponent.buttonActionsList + 1] = "FrontJab"

            else
            
                UIManagerComponent.buttonActionsList[#UIManagerComponent.buttonActionsList + 1] = "EnergyBolt"

            end
        end

        ChangeTexture(GetEntity("Button1"), "Dice_Pink")
        ChangeTexture(GetEntity("Button2"), "Dice_Pink")
        ChangeTexture(GetEntity("Button3"), "Dice_Pink")
        --ChangeTexture(GetEntity("Button3"), UIManagerComponent.buttonActionsList[3])

        -- reset dice value to zero

    end

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