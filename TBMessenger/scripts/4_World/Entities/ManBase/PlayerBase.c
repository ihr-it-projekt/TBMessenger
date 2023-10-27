modded class PlayerBase
{
    private ref TBMMessageMenu tbm_messageMenu;
    private ref TBMMessageSystemMenu tbm_messageSystemMenu;

	private string playerUID;

	string GetPlayerId() {
		if (!playerUID) {
			playerUID = GetIdentity().GetId();
		}
		
		return playerUID;
	}

    override void SetActions(out TInputActionMap InputActionMap) {
        super.SetActions(InputActionMap);

        AddAction(TBMActionGiveNumber, InputActionMap);
    }
	
    void DisplayMessage(string message) {
        GetMessageMenu();
        tbm_messageMenu.SetText(message);
        tbm_messageMenu.Init();
        tbm_messageMenu.Show();
    }

    void UpdatePlayerAtDependencies() {
        if (tbm_messageSystemMenu && tbm_messageSystemMenu.IsVisible()) {
            tbm_messageSystemMenu.UpdatePlayer(this);
        }
    }

    private void InitMenu(TBMBaseMenu menu) {
        menu.SetConfig(GetConfig());
        menu.SetPlayer(this);
    }

	TBMMessageMenu GetMessageMenu() {
		tbm_messageMenu = new TBMMessageMenu();
		return tbm_messageMenu;
	}

	TBMMessageSystemMenu GetMessageSystemMenu() {
		tbm_messageSystemMenu = new TBMMessageSystemMenu();
		InitMenu(tbm_messageSystemMenu);
		return tbm_messageSystemMenu;
	}

	void RefreshMessageSystem() {
	    if (tbm_messageSystemMenu) {
	        tbm_messageSystemMenu.RefreshMessageSystem();
	    }
	}

	bool CloseMenu() {
	    bool hasDoAction = false;

		if (tbm_messageSystemMenu && tbm_messageSystemMenu.IsVisible()) {
		    tbm_messageSystemMenu.OnHide();
			hasDoAction = true;
		}
		
		return hasDoAction;
	}

	array<EntityAI> GetPlayerItems() {
        array<EntityAI> itemsArray = new array<EntityAI>;
        GetInventory().EnumerateInventory(InventoryTraversalType.INORDER, itemsArray);

        return itemsArray;
    }

    bool HasInInventory(string type) {
        array<EntityAI> items = GetPlayerItems();
        foreach(EntityAI item: items) {
            if (item.GetType() == type) {
                return true;
            }
        }
		return false;
    }

    array<EntityAI> GetItemsByTypeFromInventory(string type) {
        array<EntityAI> items = GetPlayerItems();
        array<EntityAI> itemsFound = new array<EntityAI>;
        foreach(EntityAI item: items) {
            if (item.GetType() == type) {
                itemsFound.Insert(item);
            }
        }
        return itemsFound;
    }

    TBMConfig GetConfig() {
        if (GetGame().IsServer()) {
            return TBMConfig.Get();
        }
        return TBMPlayerClientDB.Get().GetConfig();
    }

    bool CanOpenMessageMenu() {
        if (IsRestrained()) return false;
        if (IsUnconscious()) return false;
        if (!GetConfig()) return false;
        if (!GetConfig().messageConfig) {
			GetGame().RPCSingleParam(TBMPlayerBaseHelper.GetPlayer(), DAY_Z_LIFE_EVENT_GET_CONFIG, null, true);
            return false;
        }

        if (!GetConfig().messageConfig.mustHavePersonalRadio) return true;

        array<EntityAI> radios = GetItemsByTypeFromInventory("PersonalRadio");

        if (radios.Count() < 1) return false;

        if (!GetConfig().messageConfig.radioMustHaveBattery) return true;

        foreach(EntityAI radio: radios) {
            if (radio.GetInventory()) {
                for(int i = 0; i < radio.GetInventory().AttachmentCount(); i++ ) {
                    EntityAI attachment = radio.GetInventory().GetAttachmentFromIndex(i);
                    if(attachment && attachment.GetType() == "Battery9V") {
						Battery9V itemCast = Battery9V.Cast(attachment);

						if (itemCast && itemCast.GetQuantity() > 0) {
							float energy = itemCast.GetCompEM().GetEnergy();
							
							if (energy > 0) {
							    if (!GetConfig().messageConfig.radioMustBeOn) return true;
								
							    if (radio.GetCompEM().IsSwitchedOn()) return true;
							}
						}                        
                    }
                }
            }
        }

        return false;
    }

}
